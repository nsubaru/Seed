namespace Seed.Core.FS;

/// <summary>
/// File object type
/// </summary>
[SerializableEnum("Seed::Core::IO::FS::EFileObjectTypes")]
public enum EFileObjectTypes : UInt8
{
    /// <summary>
    /// File
    /// </summary>
    File,

    /// <summary>
    /// Directory
    /// </summary>
    Directory,

    /// <summary>
    /// Unknown type
    /// </summary>
    Unknown
};
