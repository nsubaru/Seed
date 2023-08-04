using Seed.CTTI;

namespace Seed.Core.FS;

/// <summary>
/// Volume information
/// </summary>
[SerializableStruct("Seed::Core::IO::FS::ObjectProvider::VolumeInformationType")]
public struct VolumeInformation
{
    /// <summary>
    /// Serial number
    /// </summary>
    [SerializableStructFieldManual("SerialNumber", "Seed::Core::UInt32")]
    public uint SerialNumber { get; set; }

    /// <summary>
    /// Creation time
    /// </summary>
    [SerializableStructFieldManual("CreationTime", "Seed::Core::Time::DateTime")]
    public Time.DateTime CreationTime { get; set; }

    /// <summary>
    /// Volume mark
    /// </summary>
    [SerializableStructFieldManual("Label", "Seed::Core::String")]
    public string Label { get; set; }

    /// <summary>
    /// File system name
    /// </summary>
    [SerializableStructFieldManual("FsName", "Seed::Core::String")]
    public string FsName { get; set; }

    /// <summary>
    /// Volume size
    /// </summary>
    [SerializableStructFieldManual("Size", "Seed::Core::IO::FS::VolumeSize")]
    public VolumeSize Size { get; set; }

    /// <summary>
    /// Volume attributes
    /// </summary>
    [SerializableStructFieldManual("Attributes", "Seed::Core::IO::FS::VolumeAttributes")]
    public VolumeAttributes Attributes { get; set; }
}
