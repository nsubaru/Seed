namespace Seed.Core.Settings;

[SerializableStruct("Seed::Core::Settings::SettingsData::KernelVersion")]
public struct KernelVersion
{
    [SerializableStructFieldManual(nameof(Major), "Seed::Core::UInt16")]
    public UInt16 Major;

    [SerializableStructFieldManual(nameof(Minor), "Seed::Core::UInt16")]
    public UInt16 Minor;

    [SerializableStructFieldManual(nameof(StableBuildNumber), "Seed::Core::UInt16")]
    public UInt16 StableBuildNumber;

    [SerializableStructFieldManual(nameof(BuildNumber), "Seed::Core::UInt16")]
    public UInt16 BuildNumber;
}
