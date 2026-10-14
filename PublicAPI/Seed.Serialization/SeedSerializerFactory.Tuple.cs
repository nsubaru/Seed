using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    Type? GetTupleType(RTTI.Record.RttiTypeRecord record)
    {
        List<Type> types = new List<Type>();
        foreach (var item in record.Fields)
        {
            var type = GetTypeWithOffset(types.Count, record);
            if (type == null)
            {
                return null;
            }

            types.Add(type);
        }

        if (types.Count == 0)
        {
            return typeof(ValueTuple);
        }
        else
        {
            var tp = typeof(ValueTuple).GetMethods(System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.Public).First(mth => mth.Name == "Create" && mth.GetParameters().Length == types.Count).ReturnType.GetGenericTypeDefinition();
            if (tp == null)
            {
                return null;
            }

            return tp.MakeGenericType(types.ToArray());
        }
    }

    private void AddTupleSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        switch (typeRecord.TypeName)
        {
            case "Seed::Core::Containers::Tuple":
                {
                    foreach (var field in typeof(T).GetFields().Select(x => x.Name))
                    {
                        code.AppendLine($"this.SerializeImpl(data.{field}, stream); ");
                    }

                    break;
                }

            default:
                throw new NotImplementedException($"Serialization of type  ' {typeRecord.TypeName} ' and type class ' {typeRecord.TypeName} ' not implemented");
        }
    }

    private void AddTupleDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        switch (typeRecord.TypeName)
        {
            case "Seed::Core::Containers::Tuple":
                {
                    foreach (var field in typeof(T).GetFields().Select(x => x.Name))
                    {
                        code.AppendLine($"var {field} = data.{field};");
                        code.AppendLine($"data.{field} = this.DeserializeImpl({field}, stream);");
                    }
                }

                break;
            default:
                throw new NotImplementedException($"Serialization of type  ' {typeRecord.TypeName} ' and type class ' {typeRecord.TypeName} ' not implemented");
        }
    }
}
