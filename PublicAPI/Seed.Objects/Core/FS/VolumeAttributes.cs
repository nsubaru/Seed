namespace Seed.Core.FS;

/// <summary>
/// Volume attributes
/// </summary>
[SerializableStruct("Seed::Core::IO::FS::ObjectProvider::VolumeAttributes")]
public struct VolumeAttributes
{
    /// <summary>
    /// Volume flags
    /// </summary>
    [SerializableStructFieldManual("Flags", "Seed::Core::SizeT")]
    public ID Flags { get; set; }
};
