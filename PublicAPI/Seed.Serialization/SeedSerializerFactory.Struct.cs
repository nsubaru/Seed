using Microsoft.Extensions.Logging;
using Seed.CTTI;
using Seed.CTTI.MetaData;
using System.Reflection;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    Type? GetStructType(RTTI.Record.RttiTypeRecord record)
    {
        var type = TryToGetPredefinedType(record);
        if (type != null)
        {
            return type;
        }

        if (record.TypeName.StartsWith(Constants.Types.ObjectProperties))
        {
            return GetObjectProperties(record);
        }

        type = serializableTypes.FirstOrDefault(x => x.Attribute!.SeedKernelTypeName == record.TypeName).Type;
        if (type == null)
        {
            logger?.LogInformation("Can not found correspondent Seed type for {0}", record.TypeName);

            return null;
        }

        foreach (var item in record.Fields)
        {
            var isExistManualPropAttrb = type.GetProperties().Where(prop => prop.GetCustomAttributes(typeof(SerializableStructFieldManualAttribute), false).Any(attrbs => (attrbs as SerializableStructFieldManualAttribute)!.FieldInformation.FieldName == item.FieldName)).FirstOrDefault() != null;
            var isExistManualFieldAttrb = type.GetFields().Where(field => field.GetCustomAttributes(typeof(SerializableStructFieldManualAttribute), false).Any(attrbs => (attrbs as SerializableStructFieldManualAttribute)!.FieldInformation.FieldName == item.FieldName)).FirstOrDefault() != null;
            var autoPropType = type.GetProperties().Where(prop => prop.Name == item.FieldName).Where(prop => prop.GetCustomAttributes(typeof(SerializableStructFieldAttribute), false).Any()).FirstOrDefault();
            var autoFieldType = type.GetFields().Where(field => field.Name == item.FieldName).Where(field => field.GetCustomAttributes(typeof(SerializableStructFieldAttribute), false).Any()).FirstOrDefault();
            var isExistAutoPropAttrb = autoPropType != null;
            var isExistAutoFieldAttrb = autoFieldType != null;
            if (isExistAutoPropAttrb)
            {
                isExistAutoPropAttrb = supportedTypesList.Any(x => x.TypeName == item.TypeName && x.ClrType == autoPropType!.PropertyType);
                if (!isExistAutoPropAttrb)
                {
                    var fieldRecord = allRttiTypes.FirstOrDefault(x => x.FullTypeId == item.FullTypeId);
                    isExistAutoPropAttrb = fieldRecord != null && BuildClrType(fieldRecord!) != null;
                }
            }

            if (isExistAutoFieldAttrb)
            {
                isExistAutoFieldAttrb = supportedTypesList.Any(x => x.TypeName == item.TypeName && x.ClrType == autoFieldType!.FieldType);
                if (!isExistAutoFieldAttrb)
                {
                    var fieldRecord = allRttiTypes.FirstOrDefault(x => x.FullTypeId == item.FullTypeId);
                    isExistAutoFieldAttrb = fieldRecord != null && BuildClrType(fieldRecord!) != null;
                }
            }

            if (!(isExistManualPropAttrb || isExistManualFieldAttrb || isExistAutoPropAttrb || isExistAutoFieldAttrb))
            {
                logger?.LogWarning("Struct type {0} do not have required {1} propertie/field of type {2}. Seed type {3}", type.FullName, item.FieldName, item.TypeName, record.TypeName);

                return null;
            }
        }

        return type;
    }

    private void AddPropertiesStructSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        var manualProperties =
            from prop in typeof(T).GetProperties()
            where prop.GetCustomAttribute<SerializableStructFieldManualAttribute>() is not null
            select new
            {
                Property = prop,
                FieldInformation = prop!.GetCustomAttribute<SerializableStructFieldManualAttribute>()!.FieldInformation!
            };
        var autoProperties =
            from prop in typeof(T).GetProperties()
            where prop.GetCustomAttribute<SerializableStructFieldAttribute>() is not null
            select new
            {
                Property = prop,
                FieldInformation = new StructFieldInformation
                {
                    FieldName = prop.Name,
                    FieldTypeName = prop.PropertyType.GetCustomAttribute<SeedSerializableAttribute>()?.SeedKernelTypeName ?? string.Empty
                }
            };
        var properties = manualProperties.Union(autoProperties);
        if (!properties.Any())
        {
            return;
        }

        var orderedProperties = typeRecord.RttiType.Fields.Select(field => properties.First(prop => prop.FieldInformation.FieldName == field.FieldName).Property.Name);
        foreach (var field in orderedProperties)
        {
            code.AppendLine($"this.SerializeImpl(data.{field}, stream);");
        }
    }

    private void AddFieldsStructSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        var manualFields =
            from field in typeof(T).GetFields()
            where field.GetCustomAttribute<SerializableStructFieldManualAttribute>() is not null
            select new
            {
                Field = field,
                FieldInformation = field!.GetCustomAttribute<SerializableStructFieldManualAttribute>()!.FieldInformation!
            };
        var autoFields =
            from field in typeof(T).GetFields()
            where field.GetCustomAttribute<SerializableStructFieldAttribute>() is not null
            select new
            {
                Field = field,
                FieldInformation = new StructFieldInformation
                {
                    FieldName = field.Name,
                    FieldTypeName = field.FieldType.GetCustomAttribute<SeedSerializableAttribute>()?.SeedKernelTypeName ?? string.Empty
                }
            };
        var fields = manualFields.Union(autoFields);
        if (!fields.Any())
        {
            return;
        }

        var orderedFields = typeRecord.RttiType.Fields.Select(rttiField => fields.First(field => field.FieldInformation.FieldName == rttiField.FieldName).Field.Name);
        foreach (var field in orderedFields)
        {
            code.AppendLine($"this.SerializeImpl(data.{field}, stream);");
        }
    }

    private void AddStructSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (AddPredefinedStructSer<T>(ref code, typeRecord))
        {
            return;
        }

        AddPropertiesStructSer<T>(ref code, typeRecord);
        AddFieldsStructSer<T>(ref code, typeRecord);
    }

    private void AddPropertiesStructDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        var manualProperties =
            from prop in typeof(T).GetProperties()
            where prop.GetCustomAttribute<SerializableStructFieldManualAttribute>() is not null
            select new
            {
                Property = prop,
                FieldInformation = prop!.GetCustomAttribute<SerializableStructFieldManualAttribute>()!.FieldInformation!
            };
        var autoProperties =
            from prop in typeof(T).GetProperties()
            where prop.GetCustomAttribute<SerializableStructFieldAttribute>() is not null
            select new
            {
                Property = prop,
                FieldInformation = new StructFieldInformation
                {
                    FieldName = prop.Name,
                    FieldTypeName = prop.PropertyType.GetCustomAttribute<SeedSerializableAttribute>()?.SeedKernelTypeName ?? string.Empty
                }
            };
        var properties = manualProperties.Union(autoProperties);
        if (!properties.Any())
        {
            return;
        }

        var orderedProperties = typeRecord.RttiType.Fields.Select(field => properties.First(prop => prop.FieldInformation.FieldName == field.FieldName).Property.Name);
        foreach (var field in orderedProperties)
        {
            code.AppendLine($"data.{field} = this.DeserializeImpl(data.{field}, stream);");
        }
    }

    private void AddFieldsStructDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        var manualFields =
            from field in typeof(T).GetFields()
            where field.GetCustomAttribute<SerializableStructFieldManualAttribute>() is not null
            select new
            {
                Field = field,
                FieldInformation = field!.GetCustomAttribute<SerializableStructFieldManualAttribute>()!.FieldInformation!
            };
        var autoFields =
            from field in typeof(T).GetFields()
            where field.GetCustomAttribute<SerializableStructFieldAttribute>() is not null
            select new
            {
                Field = field,
                FieldInformation = new StructFieldInformation
                {
                    FieldName = field.Name,
                    FieldTypeName = field.FieldType.GetCustomAttribute<SeedSerializableAttribute>()?.SeedKernelTypeName ?? string.Empty
                }
            };
        var fields = manualFields.Union(autoFields);
        if (!fields.Any())
        {
            return;
        }

        var orderedFields = typeRecord.RttiType.Fields.Select(rttiField => fields.First(field => field.FieldInformation.FieldName == rttiField.FieldName).Field.Name);
        foreach (var field in orderedFields)
        {
            code.AppendLine($"data.{field} = this.DeserializeImpl(data.{field}, stream);");
        }
    }

    private void AddStructDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (AddPredefinedStructDer<T>(ref code, typeRecord))
        {
            return;
        }

        AddPropertiesStructDer<T>(ref code, typeRecord);
        AddFieldsStructDer<T>(ref code, typeRecord);
    }
}
