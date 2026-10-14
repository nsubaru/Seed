namespace Seed.Core.Threading;

[SerializableStruct("Seed::Core::Threading::ThreadableMetricsRecord")]
public struct ThreadableMetricsRecord
{
    [SerializableStructField]
    public SizeT TotalMemoryAllocated
    {
        get;
        set;
    }

    [SerializableStructField]
    public SizeT TotalMemoryFreed
    {
        get;
        set;
    }

    [SerializableStructField]
    public SizeT TotalMemblockAllocated
    {
        get;
        set;
    }

    [SerializableStructField]
    public SizeT TotalMemblockFreed
    {
        get;
        set;
    }

    [SerializableStructField]
    public SizeT CurrentMemoryUsage
    {
        get;
        set;
    }

    [SerializableStructField]
    public SizeT CurrentMemblockUsage
    {
        get;
        set;
    }

    [SerializableStructField]
    public SizeT ExceptionsCount
    {
        get;
        set;
    }
}

[SerializableStruct("Seed::Core::Threading::ThreadableMetricsArea")]
public struct ThreadableMetricsArea
{
    [SerializableStructField]
    public ThreadableMetricsRecord Metrics
    {
        get;
        set;
    }

    [SerializableStructField]
    public SizeT ParentsCount
    {
        get;
        set;
    }

    [SerializableStructField]
    public ThreadableMetricsRecord MetricsIncludingParents
    {
        get;
        set;
    }
}
