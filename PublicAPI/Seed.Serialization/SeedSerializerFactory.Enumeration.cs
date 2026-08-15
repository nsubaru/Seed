using Seed.CTTI;
using System;
using Microsoft.Extensions.Logging;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    Type? GetEnumUnderlyingType(RTTI.Record.RttiTypeRecord record, RTTI.RTTISection section)
    {
        var serializableTypes = AppDomain.CurrentDomain
            .GetAssemblies()
            .SelectMany(x => x.GetTypes()
                              .Where(x => Attribute.GetCustomAttribute(x, typeof(SeedSerializableAttribute)) != null
                        ))
            .Select(x => new { Type = x, Attribute = (Attribute.GetCustomAttribute(x, typeof(SeedSerializableAttribute)) as SeedSerializableAttribute) })
            .ToList();

        var type = serializableTypes
            .FirstOrDefault(x => x.Attribute!.SeedKernelTypeName == record.TypeName)!
            ?.Type;

        if (type == null)
        {
            return null;
        }

        var seedItems = record.Fields
            .Where(item => item.FieldName != "UnderlyingType")
            .Select(item => (Name: item.TypeName, Value: item.FieldName))
            .OrderBy(item => item.Name, StringComparer.Ordinal)
            .ToList();
        var managedItems = Enum.GetValues(type)
            .Cast<object>()
            .Select(value => (
                Name: Enum.GetName(type, value) ?? string.Empty,
                Value: Enum.Format(type, value, "d")))
            .OrderBy(item => item.Name, StringComparer.Ordinal)
            .ToList();

        if (seedItems.Count != 0 && !seedItems.SequenceEqual(managedItems))
        {
            var missingItems = seedItems.Except(managedItems)
                .Select(item => $"{item.Name}={item.Value}");
            var extraItems = managedItems.Except(seedItems)
                .Select(item => $"{item.Name}={item.Value}");

            logger?.LogWarning(
                "Enum type {0} does not match Seed type {1}. Missing managed items: {2}. Extra managed items: {3}",
                type.FullName,
                record.TypeName,
                string.Join(", ", missingItems),
                string.Join(", ", extraItems));

            return null;
        }

        return type;
    }

    private void AddEnumSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var value = ({GetFriendlyTypeName(typeof(T).GetEnumUnderlyingType())}) data;");
        code.AppendLine("this.SerializeImpl(value, stream);");
    }

    private void AddEnumDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var value = ({GetFriendlyTypeName(typeof(T).GetEnumUnderlyingType())}) data;");
        code.AppendLine("value = this.DeserializeImpl(value, stream);");
        code.AppendLine($"data = ({GetFriendlyTypeName(typeof(T))}) value;");
    }
}
