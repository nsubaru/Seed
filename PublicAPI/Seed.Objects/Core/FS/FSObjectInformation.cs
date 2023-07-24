namespace Seed.Core.FS;

[SerializableStruct("Seed::Core::IO::FS::FSObjectInformation")]
public struct FSObjectInformation
{
    [SerializableStructFieldManual("Flags", "Seed::Core::UInt8")]
    public UInt8 Flags { get; set; }

    [SerializableStructFieldManual("IsReparsePoint", "Seed::Core::Boolean")]
    public bool IsReparsePoint { get; set; }

    [SerializableStructFieldManual("Name", "Seed::Core::String")]
    public string Name { get; set; }

    [SerializableStructFieldManual("Size", "Seed::Core::Memory::DataSizeT")]
    public SizeT Size { get; set; }

    [SerializableStructFieldManual("Type", "Seed::Core::IO::FS::EFileObjectTypes")]
    public EFileObjectTypes Type { get; set; }
}
