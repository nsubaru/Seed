using System.Collections;

namespace Seed.RTTI.Record;

public unsafe class RttiCollection<T> : IEnumerable<T> where T : class, RttiIterableObject<T>, new()
{
    internal Collection* @base;
    internal SizeT imageBase;
    internal bool isValidCxxTypeId;

    internal RttiCollection(Collection* @base, ulong imageBase, bool isValidCxxTypeId)
    {
        this.@base = @base;
        this.imageBase = imageBase;
        this.isValidCxxTypeId = isValidCxxTypeId;
    }

    public IEnumerator<T> GetEnumerator()
    {
        if (@base != null && @base->Count != 0)
        {
            return new RttiIterator<T>(
                (byte*)@base + @base->OffsetToFirstItem,
                0,
                @base->Count,
                imageBase,
                isValidCxxTypeId);
        }
        else
        {
            return new RttiIterator<T>(null, 0, 0, 0, isValidCxxTypeId);
        }
    }

    IEnumerator IEnumerable.GetEnumerator()
    {
        return GetEnumerator();
    }

    public SizeT Count => @base == null ? 0 : @base->Count;
}
