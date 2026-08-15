namespace Seed.Core.RTTI;

[SerializableEnum("Seed::Core::CTTI::TypeTraits::ECvRefModificators")]
public enum ECvRefModificators : SizeT
{
    Ref,
    RValueRef,
    Const,
    Volatile
}
