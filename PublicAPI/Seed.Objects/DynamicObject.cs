namespace Seed;

/// <summary>
/// Store object and Seed RTTI full type id
/// </summary>
[SerializableObject("Seed::Core::RTTI::Dynamic")]
public class DynamicObject
{
    /// <summary>
    /// Object value, can be null if object is invalid
    /// </summary>
    public object? Value { get; private set; }

    /// <summary>
    /// Object full type id, can be Guid.Zero and in that case object value equals null
    /// </summary>
    public Guid FullTypeId { get; private set; }

    /// <summary>
    /// Constructor with object and full type id
    /// </summary>
    /// <param name="value">Object value</param>
    /// <param name="fullTypeId">Seed RTTI full type id</param>
    /// <exception cref="ArgumentNullException">If object value is null</exception>
    public DynamicObject(object value, Guid fullTypeId)
    {
        Value      = value ?? throw new ArgumentNullException(nameof(value));
        FullTypeId = fullTypeId;
    }

    /// <summary>
    /// Empty ctor create invalid instance with value null
    /// </summary>
    public DynamicObject()
    {
        Value      = null;
        FullTypeId = new Guid();
    }

    /// <summary>
    /// If true than value available
    /// </summary>
    public bool IsValid => Value != null;
}
