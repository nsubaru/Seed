namespace Seed.Core.Crypt;

[SerializableEnum("Seed::Core::Crypt::EAes256Implementations")]
public enum EAes256Implementations : UInt8
{
    BasicInstructions = 0,
    DoubleBlockInstructions = 1,
    QuadBlockInstructions = 2
}
