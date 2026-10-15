using LanguageExt;
using LanguageExt.UnsafeValueAccess;
using Microsoft.CodeAnalysis;
using Seed.Core.Functionality;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    Type? GetBoxType(RTTI.Record.RttiTypeRecord record)
    {
        if (record.TemplateTypeName == Constants.Types.Either)
        {
            var leftType = GetTypeWithOffset(0, record);

            if (leftType == null)
            {
                return null;
            }

            var rightType = GetTypeWithOffset(1, record);

            if (rightType == null)
            {
                return null;
            }

            return typeof(Either<,>).MakeGenericType(leftType, rightType);
        }
        else if (record.TemplateTypeName == Constants.Types.Optional)
        {
            var innerType = GetTypeWithOffset(0, record);

            if (innerType == null)
            {
                return null;
            }

            return typeof(Optional<>).MakeGenericType(innerType);
        }
        else
        {
            var innerType = GetTypeWithOffset(0, record);
            
            if (innerType == null)
            {
                return null;
            }

            return typeof(Lazy<>).MakeGenericType(innerType);
        }
    }

    private void AddBoxSer<T>(ref StringBuilder code, TypeRecord record)
    {
        if (record.RttiType.TemplateTypeName == Constants.Types.Either)
        {
            code.AppendLine($"UInt64 idx = data.IsLeft ? 0ul : 1ul;");
            code.AppendLine($"SerializeImpl(idx, stream);");
            code.AppendLine($"data.Match(x => SerializeImpl(x, stream), x =>SerializeImpl(x, stream));");
        }
        else if (record.RttiType.TemplateTypeName == Constants.Types.Optional)
        {
            code.AppendLine($"var hasValue = data.HasValue;");
            code.AppendLine($"SerializeImpl(hasValue, stream);");
            code.AppendLine($"if (hasValue) {{ SerializeImpl(data.Value, stream); }}");
        }
        else
        {
            code.AppendLine($"SerializeImpl(data.Value, stream);");
        }
    }

    private void AddBoxDer<T>(ref StringBuilder code, TypeRecord record)
    {
        if (record.RttiType.TemplateTypeName == Constants.Types.Either)
        {
            code.AppendLine($"UInt64 idx = 0ul;");
            code.AppendLine($"idx = DeserializeImpl(idx, stream);");
            code.AppendLine($"if (idx == 0) {{");
            code.AppendLine($"var value = CreateObject<{GetFriendlyTypeName(typeof(T).GetGenericArguments().First())}>();");
            code.AppendLine($"data = {GetFriendlyTypeName(typeof(T))}.Left(DeserializeImpl(value, stream));");
            code.AppendLine($"}} else {{");
            code.AppendLine($"var value = CreateObject<{GetFriendlyTypeName(typeof(T).GetGenericArguments().Last())}>();");
            code.AppendLine($"data = {GetFriendlyTypeName(typeof(T))}.Right(DeserializeImpl(value, stream));");
            code.AppendLine($"}}");
        }
        else if (record.RttiType.TemplateTypeName == Constants.Types.Optional)
        {
            code.AppendLine($"bool hasValue = false;");
            code.AppendLine($"hasValue = DeserializeImpl(hasValue, stream);");
            code.AppendLine($"data = new {GetFriendlyTypeName(typeof(T))}();");
            code.AppendLine($"if(hasValue) {{");
            code.AppendLine($"var value = CreateObject<{GetFriendlyTypeName(typeof(T).GetGenericArguments().First())}>();");
            code.AppendLine($"value = DeserializeImpl(value, stream);");
            code.AppendLine($"data = value;");
            code.AppendLine($"}}");
        }
        else
        {
            code.AppendLine($"{GetFriendlyTypeName(typeof(T).GetGenericArguments().First())} value = data.Value;");
            code.AppendLine($"value = DeserializeImpl(value, stream);");
            code.AppendLine($"data = new {GetFriendlyTypeName(typeof(T))}(() => value);");
        }
    }
}
