namespace Seed.Core.Services;

[SerializableStruct("Seed::Core::Services::CPUInstructionSet")]
public struct CPUInstructionSet
{
    [SerializableStructField]
    public String Vendor
    {
        get;
        set;
    }

    [SerializableStructField]
    public String Brand
    {
        get;
        set;
    }

    [SerializableStructField]
    public UInt32 Stepping
    {
        get;
        set;
    }

    [SerializableStructField]
    public UInt32 Model
    {
        get;
        set;
    }

    [SerializableStructField]
    public UInt32 Family
    {
        get;
        set;
    }

    [SerializableStructField]
    public UInt64 Sets
    {
        get;
        set;
    }
}
