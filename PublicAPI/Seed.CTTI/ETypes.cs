namespace Seed.CTTI;

[SeedSerializable("Seed::Core::CTTI::EType", ETypes.Enumeration)]
public enum ETypes : byte
{
    MetaData,
    Logic,
    Numeric,
    Enumeration,
    FloatNumeric,
    Character,
    Collection,
    String,
    FixedCollection,
    AssociableCollection,
    Box,
    Tuple,
    Pair,
    Pointer,
    Reference,
    FixedArray,
    Struct,
    Object,
    Dynamic,
    Abstract,
    Function,
    Union,
    Custom,
    Undefined = byte.MaxValue
}
