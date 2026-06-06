using Microsoft.Extensions.Logging;
using MoreLinq;
using Seed.CTTI;
using Seed.CTTI.MetaData;
using System.Collections.Generic;
using System.Reflection;
using System.Text;

namespace Seed.Serialization;

internal struct ObjectPropertiesFieldsInformation
{
    public String Name { get; set; }
    public String TypeName { get; set; }
    public UUID FullTypeId { get; set; }
}

public partial class SeedSerializerFactory
{
    Type? GetObjectProperties(RTTI.Record.RttiTypeRecord record)
    {
        var type = serializableTypes
            .FirstOrDefault(x => x.Attribute!.SeedKernelTypeName == record.TypeName).Type;

        if (type == null)
        {
            logger?.LogInformation("Can not found correspondent Seed type for {0}", record.TypeName);
            return null;
        }

        var clrTypeAttribute = type.GetCustomAttribute<SerializableObjectPropetiesAttribute>()!;
        var objectType = allRttiTypes.First(x => x.IsValid && x.TypeName == clrTypeAttribute.TargetObjectTypeName);

        var props = new List<ObjectPropertiesFieldsInformation>() { new ObjectPropertiesFieldsInformation() { TypeName = Constants.Types.UInt64, Name = "ObjId", FullTypeId = allRttiTypes.First(x => x.TypeName == Constants.Types.UInt64).FullTypeId.Id } };
        props.AddRange((from prop in objectType.Properties select new ObjectPropertiesFieldsInformation() { TypeName = prop.TypeName, Name = prop.Name, FullTypeId = prop.FullTypeId.Id }));

        foreach (var item in props)
        {
            var isExistManualPropAttrb = type
                .GetProperties()
                .Where(prop => prop
                    .GetCustomAttributes(typeof(SerializableStructFieldManualAttribute), false)
                    .Any(attrbs => (attrbs as SerializableStructFieldManualAttribute)!
                    .FieldInformation.FieldName == item.Name))
                .FirstOrDefault() != null;

            var isExistManualFieldAttrb = type
                .GetFields()
                .Where(field => field
                    .GetCustomAttributes(typeof(SerializableStructFieldManualAttribute), false)
                    .Any(attrbs => (attrbs as SerializableStructFieldManualAttribute)!
                    .FieldInformation.FieldName == item.Name))
                .FirstOrDefault() != null;

            var autoPropType = type
                .GetProperties()
                .Where(prop => prop.Name == item.Name)
                .Where(prop => prop
                    .GetCustomAttributes(typeof(SerializableStructFieldAttribute), false).Any())
                .FirstOrDefault();

            var autoFieldType = type
                .GetFields()
                .Where(field => field.Name == item.Name)
                .Where(field => field
                    .GetCustomAttributes(typeof(SerializableStructFieldAttribute), false).Any())
                .FirstOrDefault();

            var isExistAutoPropAttrb = autoPropType != null;
            var isExistAutoFieldAttrb = autoFieldType != null;

            if (isExistAutoPropAttrb)
            {
                isExistAutoPropAttrb = supportedTypesList.Any(x => x.TypeName == item.TypeName && x.ClrType == autoPropType!.PropertyType);

                if (!isExistAutoPropAttrb)
                {
                    var fieldRecord = allRttiTypes.FirstOrDefault(x => x.FullTypeId.Id == item.FullTypeId);

                    isExistAutoPropAttrb = fieldRecord != null && BuildClrType(fieldRecord!) != null;
                }
            }

            if (isExistAutoFieldAttrb)
            {
                isExistAutoFieldAttrb = supportedTypesList.Any(x => x.TypeName == item.TypeName && x.ClrType == autoFieldType!.FieldType);

                if (!isExistAutoFieldAttrb)
                {
                    var fieldRecord = allRttiTypes.FirstOrDefault(x => x.FullTypeId.Id == item.FullTypeId);

                    isExistAutoFieldAttrb = fieldRecord != null && BuildClrType(fieldRecord!) != null;
                }
            }

            if (!(isExistManualPropAttrb || isExistManualFieldAttrb || isExistAutoPropAttrb || isExistAutoFieldAttrb))
            {
                logger?.LogWarning("ObjectProperties type {0} do not have required {1} propertie/field of type {2}. Seed type {3}", type.FullName, item.Name, item.TypeName, record.TypeName);
                return null;
            }
        }

        return type;
    }

    private void AddFieldsObjectPropertiesSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        var manualFields =
            from field in typeof(T).GetProperties()
            where field.GetCustomAttribute<SerializableStructFieldManualAttribute>() is not null
            select new { Field = field, FieldInformation = field!.GetCustomAttribute<SerializableStructFieldManualAttribute>()!.FieldInformation! };

        var autoFields =
            from field in typeof(T).GetProperties()
            where field.GetCustomAttribute<SerializableStructFieldAttribute>() is not null
            select new
            {
                Field = field,
                FieldInformation = new StructFieldInformation
                {
                    FieldName = field.Name,
                    FieldTypeName = field.PropertyType.GetCustomAttribute<SeedSerializableAttribute>()?.SeedKernelTypeName ?? string.Empty
                }
            };

        var fields = manualFields.Union(autoFields);

        if (!fields.Any())
        {
            return;
        }

        var type = serializableTypes
            .FirstOrDefault(x => x.Attribute!.SeedKernelTypeName == typeRecord.TypeName).Type;

        if (type == null)
        {
            return;
        }

        var clrTypeAttribute = type.GetCustomAttribute<SerializableObjectPropetiesAttribute>()!;
        var objectType = allRttiTypes.First(x => x.IsValid && x.TypeName == clrTypeAttribute.TargetObjectTypeName);

        var props = new List<ObjectPropertiesFieldsInformation>() { new ObjectPropertiesFieldsInformation() { TypeName = "Seed::Oore::SizeT", Name = "ObjId" } };
        props.AddRange((from prop in objectType.Properties select new ObjectPropertiesFieldsInformation() { TypeName = prop.TypeName, Name = prop.Name }));

        var orderedFields = props
            .Select(rttiField => fields
                .First(field => field.FieldInformation.FieldName == rttiField.Name)
                .Field.Name);

        foreach (var field in orderedFields)
        {
            code.AppendLine($"this.SerializeImpl(data.{field}, stream);");
        }
    }

    private void AddFieldsObjectPropertiesDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        var manualFields =
            from field in typeof(T).GetProperties()
            where field.GetCustomAttribute<SerializableStructFieldManualAttribute>() is not null
            select new { Field = field, FieldInformation = field!.GetCustomAttribute<SerializableStructFieldManualAttribute>()!.FieldInformation! };

        var autoFields =
            from field in typeof(T).GetProperties()
            where field.GetCustomAttribute<SerializableStructFieldAttribute>() is not null
            select new
            {
                Field = field,
                FieldInformation = new StructFieldInformation
                {
                    FieldName = field.Name,
                    FieldTypeName = field.PropertyType.GetCustomAttribute<SeedSerializableAttribute>()?.SeedKernelTypeName ?? string.Empty
                }
            };

        var fields = manualFields.Union(autoFields);

        if (!fields.Any())
        {
            return;
        }

        var type = serializableTypes
            .FirstOrDefault(x => x.Attribute!.SeedKernelTypeName == typeRecord.TypeName).Type;

        if (type == null)
        {
            return;
        }

        var clrTypeAttribute = type.GetCustomAttribute<SerializableObjectPropetiesAttribute>()!;
        var objectType = allRttiTypes.First(x => x.IsValid && x.TypeName == clrTypeAttribute.TargetObjectTypeName);

        var props = new List<ObjectPropertiesFieldsInformation>() { new ObjectPropertiesFieldsInformation() { TypeName = "Seed::Oore::SizeT", Name = "ObjId" } };
        props.AddRange((from prop in objectType.Properties select new ObjectPropertiesFieldsInformation() { TypeName = prop.TypeName, Name = prop.Name }));

        var orderedFields = props
            .Select(rttiField => fields
                .First(field => field.FieldInformation.FieldName == rttiField.Name)
                .Field.Name);

        foreach (var field in orderedFields)
        {
            code.AppendLine($"data.{field} = this.DeserializeImpl(data.{field}, stream);");
        }
    }
}
