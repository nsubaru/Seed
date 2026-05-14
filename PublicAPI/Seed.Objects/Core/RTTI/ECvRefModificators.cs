namespace Seed.Core.RTTI;

[SerializableEnum("Seed::Core::RTTI::ECvRefModificators")]
public enum ECvRefModificators : SizeT
{
    Ref = 1,
    RValueRef = 2,
    Const = 4,
    Volatile = 8
}