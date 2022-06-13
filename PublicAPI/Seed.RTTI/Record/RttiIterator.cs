using System.Collections;

namespace Seed.RTTI.Record;

public interface RttiIterableObject<T> where T : class, RttiIterableObject<T>, new()
{
    internal T CreateInstance(RttiIterator<T> iterator);
}

public unsafe class RttiIterator<T> : IEnumerator<T>
    where T : class, RttiIterableObject<T>, new()
{
    internal ItemBase* item;
    internal ItemBase* originalItem;
    internal SizeT idx;
    internal SizeT count;
    internal bool isValidCxxTypeId;
    internal SizeT imageBase;

    internal RttiIterator(void* item, SizeT idx, SizeT count, SizeT imageBase, bool isValidCxxTypeId)
    {
        this.originalItem = this.item = (ItemBase*)item;
        this.idx = idx;
        this.count = count;
        this.imageBase = imageBase;
        this.isValidCxxTypeId = isValidCxxTypeId;
    }

    private T GetObject()
    {
        return new T().CreateInstance(this);
    }

    T IEnumerator<T>.Current { get => GetObject(); }

    object IEnumerator.Current { get => GetObject(); }

    public bool MoveNext()
    {
        if (item == null || idx == count)
        {
            return false;
        }

        idx++;

        if (idx == count)
        {
            item = null;
            return false;
        }
        else
        {
            item = (ItemBase*)((byte*)item + item->SizeOfItem);
            return true;
        }

    }

    public void Reset()
    {
        idx = 0;
        item = originalItem;
    }

    public void Dispose() { }
}
