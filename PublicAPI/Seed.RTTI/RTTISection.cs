using Seed.RTTI.Record;
using Seed.RTTI.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Seed.RTTI;

public unsafe class RTTISection : IDisposable
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

	private List<RttiTypeRecord> registeredRecords;
    private bool disposedValue;

    public RTTISection(byte[] data)
    {
		rawArrayPtrHandle = GCHandle.Alloc(data, GCHandleType.Pinned);
        rawArrayPtr = (Byte*) rawArrayPtrHandle.AddrOfPinnedObject();
        rawArrayLen = (SizeT) data.Length;

        FixupAddreses();

        registeredRecords = new List<RttiTypeRecord>();
    }

    private void FixupAddreses()
    {
        var magicNumer = Encoding.ASCII.GetBytes(MagicNumer.ToCharArray());

        var magicNumberOffset = Utils.Utils.BinnarySearchInBlob(rawArrayPtr, rawArrayLen, magicNumer, 0);

        headerOffset = Utils.Utils.BinnarySearchInBlob(rawArrayPtr, rawArrayLen, magicNumer, magicNumberOffset + 1);

        if (magicNumberOffset == SizeT.MaxValue || headerOffset == SizeT.MaxValue)
            throw new InvalidDataException();

        var @base = rawArrayPtr;

        SectionHeaderType* header = (SectionHeaderType*) (rawArrayPtr + headerOffset);

        var origHeaderAddr = (SizeT) header->OriginHeaderAddr;

        bool direction = ((SizeT) @base) + magicNumberOffset < origHeaderAddr;

        SizeT offset =
            magicNumberOffset < origHeaderAddr
            ? origHeaderAddr - (((SizeT)@base) + magicNumberOffset)
            : ((((SizeT)@base) + magicNumberOffset) - origHeaderAddr);

        var fa = (SizeT*) ((direction ? ((SizeT) header->FaAddr) - offset : ((SizeT) header->FaAddr) + offset));
        var fz = (SizeT*) ((direction ? ((SizeT) header->FzAddr) - offset : ((SizeT) header->FzAddr) + offset));

        var count = (SizeT)(fz) - (SizeT)(fa);

        this.fa = (SizeT*) fa;
        this.fCount = count / sizeof(SizeT);
        this.offset = offset;
        this.direction = direction;
    }

    protected virtual void Dispose(bool disposing)
    {
        if (!disposedValue)
        {
            if (disposing)
            {
                // TODO: dispose managed state (managed objects)
            }

            rawArrayPtrHandle.Free();
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

    void LoadAllRegisteredTypes()
    {
        var @base = this.rawArrayPtr;

        SectionHeaderType* header = (SectionHeaderType*) (@base + this.headerOffset);

        SizeT* start = this.fa + 1;

        SizeT count = 0;
        for (SizeT i = 0; i < this.fCount; i++)
        {
            if (*(this.fa + i) != 0)
                count++;
        }

        var records = new List<RttiTypeRecord>((int) count);

        SizeT j = 0;
        for (SizeT i = 0; i < count; i++, j++)
        {
            while((SizeT) (start + j) == 0)
            {
                j++;
            }

            var relativePtrAddr = *(start + j);

            var absolutPreAddr = this.direction
                ? relativePtrAddr - this.offset
                : relativePtrAddr + this.offset;

            records.Add(new RttiTypeRecord(
                (MetaData*)absolutPreAddr,
                (SizeT)header->ImageBase,
                false));
        }

        this.registeredRecords = records;
    }

    internal List<RttiTypeRecord> GetAllRegisteredTypes()
    {
        if (this.registeredRecords.Count == 0)
            LoadAllRegisteredTypes();

        return this.registeredRecords;
    }

    public List<RttiTypeRecord> AllRegisteredTypes => GetAllRegisteredTypes();

    internal KernelVersionT GetKernelVersion()
    {
        var @base = this.rawArrayPtr;

        SectionHeaderType* header = (SectionHeaderType*)(@base + this.headerOffset);

        return header->KernelVersion;
    }

    public KernelVersionT KernelVersion => GetKernelVersion();
}
