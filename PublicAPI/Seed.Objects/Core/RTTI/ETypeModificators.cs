namespace Seed.Core.RTTI;

[SerializableEnum("Seed::Core::RTTI::ETypeModificators")]
public enum ETypeModificators : byte
{
    Const,
    Volatile,
    Serializable,
    TemplateType
}