namespace Seed.Core.FS;

/// <summary>
/// Volume size information
/// </summary>
[SerializableStruct("Seed::Core::IO::FS::ObjectProvider::VolumeSize")]
public struct VolumeSize
{
    /// <summary>
    /// Number of clusters
    /// </summary>
    [SerializableStructFieldManual("TotalClusters", "Seed::Core::SizeT")]
    public ID TotalClusters { get; set; }
    /// <summary>
    /// Number of used clusters
    /// </summary>
    [SerializableStructFieldManual("AllocatedClusters", "Seed::Core::SizeT")]
    public ID AllocatedClusters { get; set; }
    /// <summary>
    /// Cluster size in bytes
    /// </summary>
    [SerializableStructFieldManual("ClusterSize", "Seed::Core::SizeT")]
    public ID ClusterSize { get; set; }
}
