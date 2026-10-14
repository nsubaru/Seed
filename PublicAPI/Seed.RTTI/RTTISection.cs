using Seed.Core.Configurations;
using Seed.Core.RTTI;
using Seed.RTTI.Record;
using Seed.Serialization;
using System;
using System.Collections.Concurrent;
using System.Reflection;
using System.Text;

namespace Seed.RTTI;

/// <summary>
/// This type contain raw internal rtti section
/// <remarks>
/// <para>RTTI section in executable have name .type and have next structure:</para>
/// <para> .type$a  - Magic number (<c>Seed::Core::RTTI::SectionMagicNumber</c> variable)</para>
/// <para> .type$b  - Section header (have all neaded information and have type <c>Seed.RTTI.SectionHeaderType</c></para>
/// <para> .type$fa - Start of pointer subsection (this subsection store pointers to all type records)</para>
/// <para> .type$fz - End of pointer subsection</para>
/// <para> .type$va - Start of variable subsection (this subsection store type records of internal type <c>Seed.RTTI.MetaData</c>)</para>
/// <para> .type$vz - End of variable subsection</para>
/// <para> .type$z  - End of rtti section</para>
/// <para> 
/// For iterating over all rtti records this class iterate over pointer subsection and using addresses of meta data records build RttiTypeRecord 
/// Before and during iterating this class convert absolute pointers addresses to current one
/// </para>
/// </remarks>
/// </summary>
public unsafe class RTTISection : IDisposable, IEquatable<RTTISection?>
{
    static readonly string MagicNumer = "SEED_RXX_RTTI\0";
    private Byte* rawArrayPtr;
    private SizeT rawArrayLen;
    private GCHandle rawArrayPtrHandle;
    private SizeT headerOffset;
    private SizeT* fa;
    private SizeT fCount;
    private SizeT offset;
    private Boolean direction;
    private readonly object registeredRecordsGate = new();
    private volatile List<RttiTypeRecord>? registeredRecords;
    private bool disposedValue;
    internal byte[]? data;

    /// <summary>
    /// Empty ctor, create invalid instance
    /// </summary>
    public RTTISection()
    {
        this.data = null;
        this.direction = false;
        this.headerOffset = new SizeT();
        this.fa = null;
        this.fCount = 0;
        this.offset = new SizeT();
        this.disposedValue = false;
    }

    /// <summary>
    /// Load RTTI section from raw blob
    /// </summary>
    /// <param name="data">Blob with rtti section</param>
    public RTTISection(byte[] data)
    {
        if (data is null)
        {
            throw new ArgumentNullException(nameof(data));
        }

        this.data = (byte[])data.Clone();
        rawArrayPtrHandle = GCHandle.Alloc(this.data, GCHandleType.Pinned);
        rawArrayPtr = (Byte*)rawArrayPtrHandle.AddrOfPinnedObject();
        rawArrayLen = (SizeT)data.Length;
        try
        {
            FixupAddresses();
        }
        catch
        {
            rawArrayPtrHandle.Free();
            rawArrayPtr = null;
            this.data = null;
            disposedValue = true;
            throw;
        }
    }

    private void FixupAddresses()
    {
        var magicNumer = Encoding.ASCII.GetBytes(MagicNumer.ToCharArray());
        int FindMagic(int start)
        {
            for (int index = start; index <= data!.Length - magicNumer.Length; ++index)
            {
                int matched = 0;
                while (matched < magicNumer.Length && data[index + matched] == magicNumer[matched])
                {
                    ++matched;
                }

                if (matched == magicNumer.Length)
                {
                    return index;
                }
            }

            return -1;
        }

        int firstMagic = FindMagic(0);
        int secondMagic = firstMagic < 0 ? -1 : FindMagic(firstMagic + 1);
        if (firstMagic < 0 || secondMagic < 0 || secondMagic > data!.Length - sizeof(SectionHeaderType))
        {
            throw new InvalidDataException("The blob does not contain a complete Seed RTTI header.");
        }

        var magicNumberOffset = (SizeT)firstMagic;
        headerOffset = (SizeT)secondMagic;
        var @base = rawArrayPtr;
        SectionHeaderType* header = (SectionHeaderType*)(rawArrayPtr + headerOffset);
        var origHeaderAddr = (SizeT)header->OriginHeaderAddr;
        if (origHeaderAddr == 0)
        {
            throw new InvalidDataException("The Seed RTTI origin address is invalid.");
        }

        SizeT ValidateRange(SizeT address, SizeT bytes)
        {
            if (address < origHeaderAddr || bytes > rawArrayLen)
            {
                throw new InvalidDataException("The Seed RTTI address is outside its exported section.");
            }

            SizeT relative = address - origHeaderAddr;
            if (relative > rawArrayLen - bytes || magicNumberOffset > rawArrayLen - bytes - relative)
            {
                throw new InvalidDataException("The Seed RTTI range is outside its exported section.");
            }

            return magicNumberOffset + relative;
        }

        SizeT faOffset = ValidateRange((SizeT)header->FaAddr, sizeof(SizeT));
        SizeT fzOffset = ValidateRange((SizeT)header->FzAddr, sizeof(SizeT));
        if (fzOffset <= faOffset || (fzOffset - faOffset) % sizeof(SizeT) != 0)
        {
            throw new InvalidDataException("The Seed RTTI pointer table is invalid.");
        }

        ValidateRange((SizeT)header->VaAddr, 0);
        ValidateRange((SizeT)header->VzAddr, 0);
        bool direction = ((SizeT)@base) + magicNumberOffset < origHeaderAddr;
        SizeT offset = (magicNumberOffset + (SizeT)@base) < origHeaderAddr ? origHeaderAddr - (((SizeT)@base) + magicNumberOffset) : ((((SizeT)@base) + magicNumberOffset) - origHeaderAddr);
        var fa = (SizeT*)(rawArrayPtr + faOffset);
        var count = fzOffset - faOffset;
        for (SizeT index = 0; index < count / sizeof(SizeT); ++index)
        {
            if (fa[index] == 0)
            {
                continue;
            }

            SizeT recordOffset = ValidateRange(fa[index], (SizeT)sizeof(MetaData));
            var record = (MetaData*)(rawArrayPtr + recordOffset);
            SizeT characters = record->TypeName.Size;
            if (characters == 0 || characters > rawArrayLen / sizeof(Char))
            {
                throw new InvalidDataException("The Seed RTTI type name length is invalid.");
            }

            ValidateRange(fa[index], 138 + characters * sizeof(Char));
            if (record->TypeName.Text[characters - 1] != '\0')
            {
                throw new InvalidDataException("The Seed RTTI type name is not terminated.");
            }
        }

        this.fa = (SizeT*)fa;
        this.fCount = count / sizeof(SizeT);
        this.offset = offset;
        this.direction = direction;
    }

    protected virtual void Dispose(bool disposing)
    {
        if (!disposedValue)
        {
            if (rawArrayPtrHandle != null && rawArrayPtrHandle.IsAllocated)
            {
                rawArrayPtrHandle.Free();
            }

            disposedValue = true;
        }
    }

    ~RTTISection()
    {
        Dispose(disposing: false);
    }

    public void Dispose()
    {
        Dispose(disposing: true);
        GC.SuppressFinalize(this);
    }

    private List<RttiTypeRecord> LoadAllRegisteredTypes()
    {
        var @base = this.rawArrayPtr;
        SectionHeaderType* header = (SectionHeaderType*)(@base + this.headerOffset);
        var records = new List<RttiTypeRecord>();
        for (SizeT i = 0; i < this.fCount; ++i)
        {
            var relativePtrAddr = this.fa[i];
            if (relativePtrAddr == 0)
            {
                continue;
            }

            var absolutPreAddr = this.direction ? relativePtrAddr - this.offset : relativePtrAddr + this.offset;
            records.Add(new RttiTypeRecord((MetaData*)absolutPreAddr, (SizeT)header->ImageBase, false));
        }

        return records;
    }

    /// <summary>
    /// Creates an independent list of this section's cached record wrappers in section order.
    /// </summary>
    /// <returns>Record wrappers that remain valid for the section's lifetime.</returns>
    internal List<RttiTypeRecord> GetAllRegisteredTypes()
    {
        if (disposedValue)
        {
            throw new ObjectDisposedException(nameof(RTTISection));
        }

        var records = registeredRecords;
        if (records is null)
        {
            lock (registeredRecordsGate)
            {
                records = registeredRecords;
                if (records is null)
                {
                    records = LoadAllRegisteredTypes();
                    registeredRecords = records;
                }
            }
        }

        return records.ToList();
    }

    /// <summary>
    /// Registered record wrappers in section order, valid for this section's lifetime.
    /// The list is independent of the cache; entries may be invalid and must be checked before use.
    /// </summary>
    public List<RttiTypeRecord> AllRegisteredTypes => GetAllRegisteredTypes();

    /// <summary>
    /// Return kernel version with which this section was builded
    /// </summary>
    /// <returns>KernelVersion struct</returns>
    internal KernelVersionT GetKernelVersion()
    {
        var @base = this.rawArrayPtr;
        SectionHeaderType* header = (SectionHeaderType*)(@base + this.headerOffset);

        return header->KernelVersion;
    }

    /// <summary>
    /// Kernel version with which this section was builded
    /// </summary>
    public KernelVersionT KernelVersion => GetKernelVersion();

    private static unsafe bool Compare(byte[] a, byte[] b)
    {
        if (a.Length != b.Length)
        {
            return false;
        }

        int len = a.Length;
        unsafe
        {
            fixed (byte* ap = a, bp = b)
            {
                long* alp = (long*)ap, blp = (long*)bp;
                for (; len >= 8; len -= 8)
                {
                    if (*alp != *blp)
                    {
                        return false;
                    }

                    alp++;
                    blp++;
                }

                byte* ap2 = (byte*)alp, bp2 = (byte*)blp;
                for (; len > 0; len--)
                {
                    if (*ap2 != *bp2)
                    {
                        return false;
                    }

                    ap2++;
                    bp2++;
                }
            }
        }

        return true;
    }

    /// <summary>
    /// Equal to obj
    /// </summary>
    /// <param name="obj">Comparable obj</param>
    /// <returns>true only if data is same</returns>
    public override bool Equals(object obj)
    {
        if (obj == null)
        {
            return this == null;
        }

        if (obj.GetType() != this.GetType())
        {
            return false;
        }

        RTTISection other = (RTTISection)obj;
        if (this.data == null && other.data == null)
        {
            return true;
        }

        if (this.data == null || other.data == null)
        {
            return false;
        }

        if (this.data.Length != other.data.Length)
        {
            return false;
        }

        return Compare(this.data, other.data);
    }

    /// <summary>
    /// Equal to obj
    /// </summary>
    /// <param name="other">Comparable section</param>
    /// <returns>true only if data is same</returns>
    public bool Equals(RTTISection? other)
    {
        if (other == null)
        {
            return this == null;
        }

        if (this.data == null && other.data == null)
        {
            return true;
        }

        if (this.data == null || other.data == null)
        {
            return false;
        }

        if (this.data.Length != other.data.Length)
        {
            return false;
        }

        return Compare(this.data, other.data);
    }

    /// <summary>
    /// Generate unique hash code using data array
    /// </summary>
    /// <returns>Hash code value</returns>
    public override int GetHashCode()
    {
        if (data == null)
        {
            return 0;
        }

        return 1768953197 + EqualityComparer<byte[]>.Default.GetHashCode(data);
    }

    /// <summary>
    /// Returns the section information
    /// </summary>
    /// <returns>Section information</returns>
    public SectionDetails GetSectionDetails()
    {
        if (this.data == null)
        {
            throw new NullReferenceException();
        }

        var @base = this.rawArrayPtr;
        SectionHeaderType* header = (SectionHeaderType*)(@base + this.headerOffset);

        return new SectionDetails
        {
            SectionSize = (SizeT)data.LongLength,
            SectionHeaderOffset = headerOffset,
            KernelVersion = header->KernelVersion,
            OriginHeaderAddr = header->OriginHeaderAddr,
            FaAddr = header->FaAddr,
            FzAddr = header->FzAddr,
            VaAddr = header->VaAddr,
            VzAddr = header->VzAddr,
            ImageBase = header->ImageBase,
            TotalCount = fCount - 1,
            ValidCount = (SizeT)registeredRecords.LongCount(x => x.IsValid),
            Offset = offset
        };
    }

    /// <summary>
    /// Section information
    /// </summary>
    public SectionDetails SectionDetails => GetSectionDetails();

    /// <summary>
    /// Returns section data
    /// </summary>
    public byte[] Bytes => data ?? Array.Empty<byte>();

    private T ReadAttribyteDataValueAsBuiltInType<T>(Byte* dataPtr)
        where T : unmanaged
    {
        return Marshal.PtrToStructure<T>((IntPtr)dataPtr);
    }

    #region Typed Attribute Value Readers
    /// <summary>
    /// Read attribute value
    /// </summary>
    /// <typeparam name="T">Value type</typeparam>
    /// <param name="attribute">Attribute object</param>
    /// <returns></returns>
    /// <exception cref="NotImplementedException">Reading custom data from data attribute unsupported</exception>
    /// <exception cref="InvalidCastException">Invalid type of attribute value</exception>
    /// <exception cref="ApplicationException">Invalid attribute value category</exception>
    public T GetAttributeValue<T>(RttiAttribute attribute) => attribute.ValueCategory switch
    {
        EAttributeValueCategories.None => (T)(object)new EmptyAttributeValueT
        {
        },
        EAttributeValueCategories.Data => GetDataAttributeValue<T>(attribute),
        EAttributeValueCategories.Text => GetTextAttributeValue<T>(attribute),
        EAttributeValueCategories.Type => GetTypeAttributeValue<T>(attribute),
        _ => throw new ApplicationException("Uknown attribute value category"),
    };

    /// <summary>
    /// For type attributes value is type identifier, 
    /// so we just return UniversalTypeIdentifier or search for type with same identifier depends on T
    /// </summary>
    /// <typeparam name="T"><see cref="UniversalTypeIdentifier"/> or <see cref="RttiTypeRecord"/></typeparam>
    /// <param name="attribute">Target attribute</param>
    /// <returns>Full type id as <see cref="UniversalTypeIdentifier"/> or type record as <see cref="RttiTypeRecord"/></returns>
    /// <exception cref="InvalidCastException">If T is not <see cref="UniversalTypeIdentifier"/> or <see cref="RttiTypeRecord"/></exception>
    private T GetTypeAttributeValue<T>(RttiAttribute attribute)
    {
        if (typeof(T) == typeof(UniversalTypeIdentifier))
        {
            return (T)(object)*(UniversalTypeIdentifier*)((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue);
        }
        else if (typeof(T) == typeof(RttiTypeRecord))
        {
            var fullTypeId = *(UniversalTypeIdentifier*)((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue);

            return (T)(object)this.GetAllRegisteredTypes().First(x => x.FullTypeId == fullTypeId);
        }
        else
        {
            throw new InvalidCastException($"Invalid requested type for reading type attribute value; expected UniversalTypeIdentifier or RttiTypeRecord.");
        }
    }

    /// <summary>
    /// For text attribute value is TextMetaData which contain size and null-termianted character array
    /// </summary>
    /// <typeparam name="T">Should be <see cref="String"/></typeparam>
    /// <param name="attribute">Target attribute</param>
    /// <returns>String value</returns>
    /// <exception cref="InvalidCastException">If T nor <see cref="String"/></exception>
    private static T GetTextAttributeValue<T>(RttiAttribute attribute)
    {
        if (typeof(T) != typeof(String))
        {
            throw new InvalidCastException($"Invalid type requested for reading text attribute value");
        }

        var text = (TextMetaData*)((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue);

        return (T)(object)Marshal.PtrToStringAuto((IntPtr)text->Text);
    }

    /// <summary>
    /// For data attribute we can read value as built-in type or use custom provider
    /// </summary>
    /// <typeparam name="T">Expected attribute value type</typeparam>
    /// <param name="attribute">Target attribute record</param>
    /// <returns>Attribute value</returns>
    /// <exception cref="InvalidCastException">If <see cref="IAttributeValueProvider{T}"/> not found for target attribute value or expected type invalid</exception>
    private T GetDataAttributeValue<T>(RttiAttribute attribute)
    {
        var value = TryReadAttributeDataValueAsBuiltInType(attribute);
        if (value is T)
        {
            return (T)value;
        }
        else if (value != null)
        {
            throw new InvalidCastException($"Invalid requested type for reading data attribute value");
        }
        else
        {
            var providerType = GetProviderType<T>();
            if (providerType != null)
            {
                var provider = (IAttributeValueProvider<T>)Activator.CreateInstance(providerType);

                return provider.GetAttributeValue((IntPtr)rawArrayPtr);
            }
            else
            {
                throw new InvalidCastException($"Invalid requested type for reading data attribute value");
            }
        }
    }

    #endregion
    #region Dynamic Attribute Value Readers
    /// <summary>
    /// Read attribute value
    /// </summary>
    /// <param name="attribute">Target attribute</param>
    /// <returns>null or attribute value if read success</returns>
    /// <exception cref="ApplicationException"></exception>
    public object? GetAnyAttributeValue(RttiAttribute attribute) => attribute.ValueCategory switch
    {
        EAttributeValueCategories.None => new EmptyAttributeValueT
        {
        },
        EAttributeValueCategories.Data => GetDataAttributeValue(attribute),
        EAttributeValueCategories.Text => GetTextAttributeValue(attribute),
        EAttributeValueCategories.AsciiText => GetTextAsciiAttributeValue(attribute),
        EAttributeValueCategories.Type => GetTypeAttributeValue(attribute),
        _ => throw new ApplicationException("Unknown attribute value category"),
    };

    /// <summary>
    /// For type attributes value is type identifier, so we just search for type with same identifier
    /// </summary>
    /// <param name="attribute">Target attribute</param>
    /// <returns>RttiTypeRecord</returns>
    private object GetTypeAttributeValue(RttiAttribute attribute)
    {
        var fullTypeId = *(UniversalTypeIdentifier*)((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue);

        return this.GetAllRegisteredTypes().First(x => x.FullTypeId == fullTypeId);
    }

    /// <summary>
    /// For text attribute value is TextMetaData which contain size and null-termianted character array
    /// </summary>
    /// <param name="attribute">Target attribute</param>
    /// <returns>String value</returns>
    private static object GetTextAttributeValue(RttiAttribute attribute)
    {
        var text = (TextMetaData*)((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue);

        return Marshal.PtrToStringAuto((IntPtr)text->Text);
    }

    /// <summary>
    /// For text attribute value is TextMetaData which contain size and null-termianted Ascii character array
    /// </summary>
    /// <param name="attribute">Target attribute</param>
    /// <returns>String value</returns>
    private static object GetTextAsciiAttributeValue(RttiAttribute attribute)
    {
        var text = (TextMetaData*)((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue);

        return Marshal.PtrToStringAnsi((IntPtr)text->Text);
    }

    /// <summary>
    /// For data attribute we can try read value as built-in type or use custom provider
    /// </summary>
    /// <param name="attribute">Target attribute</param>
    /// <returns>Attribute value or null, if provider (<see cref="IAttributeValueProvider{T}"/>) not found</returns>
    private object? GetDataAttributeValue(RttiAttribute attribute)
    {
        var value = TryReadAttributeDataValueAsBuiltInType(attribute);
        if (value != null)
        {
            return value;
        }

        var providerType = GetRuntimeProviderType(attribute.TypeName);
        if (providerType != null)
        {
            var provider = Activator.CreateInstance(providerType);
            var targetMethod = providerType.GetMethod(nameof(IAttributeValueProvider<int>.GetAttributeValue));

            return targetMethod.Invoke(attribute, new object[]
            {
                (IntPtr)rawArrayPtr
            });
        }

        // We can't read this value
        return null;
    }

    #endregion
    #region Attribute Value Readers Helpers
    /// <summary>
    /// For built-in types we can read data directly from attribute
    /// </summary>
    /// <param name="attribute">Target attribute record</param>
    /// <returns>Value or null if type not built-in</returns>
    private object? TryReadAttributeDataValueAsBuiltInType(RttiAttribute attribute)
    {
        return attribute.TypeName switch
        {
            Constants.Types.Int8 => ReadAttribyteDataValueAsBuiltInType<Int8>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.Int16 => ReadAttribyteDataValueAsBuiltInType<Int16>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.Int32 => ReadAttribyteDataValueAsBuiltInType<Int32>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.Int64 => ReadAttribyteDataValueAsBuiltInType<Int64>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.UInt8 => ReadAttribyteDataValueAsBuiltInType<UInt8>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.UInt16 => ReadAttribyteDataValueAsBuiltInType<UInt16>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.UInt32 => ReadAttribyteDataValueAsBuiltInType<UInt32>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.UInt64 => ReadAttribyteDataValueAsBuiltInType<UInt64>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.Float => ReadAttribyteDataValueAsBuiltInType<Single>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.Double => ReadAttribyteDataValueAsBuiltInType<Double>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.Boolean => ReadAttribyteDataValueAsBuiltInType<Boolean>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.UChar => ReadAttribyteDataValueAsBuiltInType<Char>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.Uuid => ReadAttribyteDataValueAsBuiltInType<Guid>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.DataSizeT => ReadAttribyteDataValueAsBuiltInType<SizeT>((Byte*)attribute.GetRecord() + attribute.GetRecord()->OffsetToAttributeValue),
            Constants.Types.EmptyAttributeValueT => EmptyAttributeValueT.Empty,
            _ => null
        };
    }

    /// <summary>
    /// Static cache for provider types by type
    /// </summary>
    private static readonly ConcurrentDictionary<Type, Type> providerTypeCache = new();

    /// <summary>
    /// Static cache for provider types by type name
    /// </summary>
    private static readonly ConcurrentDictionary<String, Type> runtimeProviderTypeCache = new();

    /// <summary>
    /// Get provider type for target attribute value type
    /// </summary>
    /// <typeparam name="T">Requested type</typeparam>
    /// <returns>Type of <see cref="IAttributeValueProvider{T}"/> if found, otherwise null</returns>
    private static Type? GetProviderType<T>()
    {
        var targetType = typeof(IAttributeValueProvider<T>);
        if (!providerTypeCache.TryGetValue(targetType, out var providerType))
        {
            providerType = AppDomain.CurrentDomain.GetAssemblies().SelectMany(assembly => assembly.DefinedTypes).FirstOrDefault(x => x.GetInterfaces().Any(i => i == targetType));
            if (providerType == null)
            {
                return null;
            }

            providerTypeCache[targetType] = providerType;
        }

        return providerType;
    }

    /// <summary>
    /// Check is type has <see cref="SerializableStructAttribute"/> or <see cref="SerializableObjectAttribute"/> attribute with target typename
    /// </summary>
    /// <param name="type">Target type</param>
    /// <param name="typeName">Target type name</param>
    /// <returns>Check result</returns>
    private static bool MatchesTypeNameAttribute(Type type, string typeName) => (type.GetCustomAttribute<SerializableStructAttribute>()?.SeedKernelTypeName == typeName) || (type.GetCustomAttribute<SerializableObjectAttribute>()?.SeedKernelTypeName == typeName);

    /// <summary>
    /// Get provider type for target attribute value type
    /// </summary>
    /// <param name="typeName">Attribute value typename</param>
    /// <returns>Type of <seealso cref="IAttributeValueProvider{T}"/> if found, otherwise null</returns>
    private static Type? GetRuntimeProviderType(String typeName)
    {
        if (!runtimeProviderTypeCache.TryGetValue(typeName, out var providerType))
        {
            providerType = AppDomain.CurrentDomain.GetAssemblies().SelectMany(assembly => assembly.GetTypes()).Where(type => type.GetInterfaces().Any(i => i.IsGenericType && i.GetGenericTypeDefinition() == typeof(IAttributeValueProvider<>))).Where(type =>
            {
                var genericArgument = type.GetInterfaces().First(i => i.IsGenericType && i.GetGenericTypeDefinition() == typeof(IAttributeValueProvider<>)).GetGenericArguments()[0];

                return MatchesTypeNameAttribute(genericArgument, typeName);
            }).FirstOrDefault();
            if (providerType == null)
            {
                return null;
            }

            runtimeProviderTypeCache[typeName] = providerType;
        }

        return providerType;
    }
    #endregion
}
