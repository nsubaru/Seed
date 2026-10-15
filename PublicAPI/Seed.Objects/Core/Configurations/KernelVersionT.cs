namespace Seed.Core.Configurations;

[SerializableStruct("Seed::Core::Configurations::ConfigurationsData::KernelVersionT")]
[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
public struct KernelVersionT
{
    [SerializableStructField]
    [FieldOffset(0)]
    public UInt16 Major;

    [SerializableStructField]
    [FieldOffset(2)]
    public UInt16 Minor;

    [SerializableStructField]
    [FieldOffset(4)]
    public UInt16 StableBuildNumber;
    
    [SerializableStructField]
    [FieldOffset(6)]
    public UInt16 BuildNumber;

    public override string ToString()
    {
        return $"{Major}.{Minor}.{StableBuildNumber}.{BuildNumber}";
    }
}
