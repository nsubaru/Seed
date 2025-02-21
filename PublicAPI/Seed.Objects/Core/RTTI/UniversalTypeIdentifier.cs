using Seed.Core.Configurations;
using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.RTTI;

[SerializableStruct("Seed::Core::RTTI::UniversalTypeIdentifier")]
[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
public unsafe struct UniversalTypeIdentifier
{
    [FieldOffset(0)]
    public UUID Id;

    [SerializableStructField]
    [FieldOffset(0)]
    public KernelVersionT KernelVersion;

    [SerializableStructField]
    [FieldOffset(8)]
    public ID TypeId;

    public override bool Equals(object? obj)
    {
        var item = (UniversalTypeIdentifier?)obj;

        return this == item;
    }

    public override int GetHashCode()
    {
        return base.GetHashCode();
    }

    public override string ToString()
    {
        return this.Id.ToString();
    }

    static public bool operator ==(UniversalTypeIdentifier? item1, UniversalTypeIdentifier? item2)
    {
        return item1?.Id == item2?.Id;
    }

    static public bool operator !=(UniversalTypeIdentifier? item1, UniversalTypeIdentifier? item2)
    {
        return item1?.Id == item2?.Id;
    }
}