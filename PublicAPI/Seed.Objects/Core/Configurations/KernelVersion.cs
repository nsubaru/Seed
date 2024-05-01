namespace Seed.Core.Configurations;

[SerializableStruct("Seed::Core::Configurations::ConfigurationsData::KernelVersion")]
[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
public struct KernelVersionT
{
    [SerializableStructFieldManual(nameof(Major), "Seed::Core::UInt16")]
    [FieldOffset(0)]
    public UInt16 Major;

    [SerializableStructFieldManual(nameof(Minor), "Seed::Core::UInt16")]
    [FieldOffset(4)]
    public UInt16 Minor;

    [SerializableStructFieldManual(nameof(StableBuildNumber), "Seed::Core::UInt16")]
    [FieldOffset(4)]
    public UInt16 StableBuildNumber;

    [SerializableStructFieldManual(nameof(BuildNumber), "Seed::Core::UInt16")]
    [FieldOffset(6)]
    public UInt16 BuildNumber;

    public override string ToString()
    {
        return $"{Major}.{Minor}.{StableBuildNumber}.{BuildNumber}";
    }
}