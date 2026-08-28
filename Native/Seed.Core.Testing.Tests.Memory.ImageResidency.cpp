#include "Seed.Core.Testing.Tests.Memory.ImageResidency.h"

#include "Seed.Core.Memory.ImageResidency.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

using namespace Seed;
using namespace Seed::Core;
using namespace Seed::Core::Memory::ImageResidency;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace
{
    constexpr SizeT SyntheticImageBytes = 0x10000;
    constexpr UInt16 ImageFileMachineAmd64 = 0x8664;
    constexpr UInt16 ImageNtOptionalHeader64Magic = 0x020b;
    constexpr UInt32 ImageSectionMemDiscardable = 0x02000000;

    Boolean HasRegion(const ImageLayout& layout, ERegion region) noexcept
    {
        for (SizeT i = 0; i < layout.LogicalRangeCount; i++)
        {
            if (layout.LogicalRanges[i].Region == region)
            {
                return true;
            }
        }

        return false;
    }

    void SetSectionName(IMAGE_SECTION_HEADER& section, const char* name, SizeT length) noexcept
    {
        RtlZeroMemory(section.Name, IMAGE_SIZEOF_SHORT_NAME);
        for (SizeT i = 0; i < length; i++)
        {
            section.Name[i] = static_cast<Byte>(name[i]);
        }
    }

    void InitializeSyntheticImage(void* allocation) noexcept
    {
        RtlZeroMemory(allocation, SyntheticImageBytes);

        auto base = reinterpret_cast<SizeT>(allocation);
        auto dos = static_cast<IMAGE_DOS_HEADER*>(allocation);
        dos->e_magic = IMAGE_DOS_SIGNATURE;
        dos->e_lfanew = 0x100;

        auto nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
        nt->Signature = IMAGE_NT_SIGNATURE;
        nt->FileHeader.Machine = ImageFileMachineAmd64;
        nt->FileHeader.NumberOfSections = 4;
        nt->FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER64);
        nt->OptionalHeader.Magic = ImageNtOptionalHeader64Magic;
        nt->OptionalHeader.SectionAlignment = 0x1000;
        nt->OptionalHeader.FileAlignment = 0x200;
        nt->OptionalHeader.SizeOfHeaders = 0x1000;
        nt->OptionalHeader.SizeOfImage = SyntheticImageBytes;
        nt->OptionalHeader.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS] = {
            0x800,
            sizeof(IMAGE_TLS_DIRECTORY64)
        };

        auto sections = IMAGE_FIRST_SECTION(nt);
        SetSectionName(sections[0], ".text", 5);
        sections[0].VirtualAddress = 0x1000;
        sections[0].Misc.VirtualSize = 0x1800;

        SetSectionName(sections[1], ".rdata", 6);
        sections[1].VirtualAddress = 0x2800;
        sections[1].Misc.VirtualSize = 0x800;

        SetSectionName(sections[2], ".bss", 4);
        sections[2].VirtualAddress = 0x3000;
        sections[2].Misc.VirtualSize = 0x2000;

        SetSectionName(sections[3], ".rsrc", 5);
        sections[3].VirtualAddress = 0x6000;
        sections[3].Misc.VirtualSize = 0x1000;
        sections[3].Characteristics = ImageSectionMemDiscardable;

        auto tls = reinterpret_cast<IMAGE_TLS_DIRECTORY64*>(base + 0x800);
        tls->StartAddressOfRawData = base + 0x5000;
        tls->EndAddressOfRawData = base + 0x5080;
    }

    void* AllocateSyntheticImage()
    {
        void* allocation = nullptr;
        SIZE_T bytes = SyntheticImageBytes;
        NTSTATUS status = NtAllocateVirtualMemory(
            NtCurrentProcess(),
            &allocation,
            0,
            &bytes,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE);

        Asserts::AssertEqual(status, STATUS_SUCCESS, SourcePosition);
        return allocation;
    }

    void FreeAllocation(void* allocation) noexcept
    {
        SIZE_T bytes = 0;
        NtFreeVirtualMemory(NtCurrentProcess(), &allocation, &bytes, MEM_RELEASE);
    }
}

bool Seed::Core::Testing::Test::Memory::ImageResidency::CurrentImagePolicyTest::Execute()
{
    ImageLayout automatic = {};
    ImageLayout pin = {};
    Asserts::AssertEqual(ResolveMappedImage(&__ImageBase, false, automatic), STATUS_SUCCESS, SourcePosition);
    Asserts::AssertEqual(ResolveMappedImage(&__ImageBase, true, pin), STATUS_SUCCESS, SourcePosition);

    Asserts::AssertTrue(HasRegion(automatic, ERegion::Text), SourcePosition);
    Asserts::AssertTrue(HasRegion(automatic, ERegion::ReadOnlyData), SourcePosition);
    Asserts::AssertTrue(HasRegion(automatic, ERegion::Data), SourcePosition);
    Asserts::AssertTrue(HasRegion(automatic, ERegion::ExceptionDirectory), SourcePosition);
    Asserts::AssertTrue(HasRegion(automatic, ERegion::ExceptionData), SourcePosition);
    Asserts::AssertTrue(HasRegion(automatic, ERegion::Rtti), SourcePosition);
    Asserts::AssertFalse(HasRegion(automatic, ERegion::Bss), SourcePosition);
    Asserts::AssertTrue(HasRegion(pin, ERegion::Bss), SourcePosition);

    for (SizeT i = 0; i < pin.LogicalRangeCount; i++)
    {
        Asserts::AssertEqual(reinterpret_cast<SizeT>(pin.LogicalRanges[i].Address) % pin.PageSize, static_cast<SizeT>(0), SourcePosition);
        Asserts::AssertEqual(pin.LogicalRanges[i].Bytes % pin.PageSize, static_cast<SizeT>(0), SourcePosition);
    }

    for (SizeT i = 1; i < pin.PhysicalRangeCount; i++)
    {
        SizeT previousEnd = reinterpret_cast<SizeT>(pin.PhysicalRanges[i - 1].Address) + pin.PhysicalRanges[i - 1].Bytes;
        Asserts::AssertTrue(previousEnd < reinterpret_cast<SizeT>(pin.PhysicalRanges[i].Address), SourcePosition);
    }

    Asserts::AssertNotEqual(ResolveMappedImage(nullptr, false, automatic), STATUS_SUCCESS, SourcePosition);
    return true;
}

bool Seed::Core::Testing::Test::Memory::ImageResidency::SyntheticImagePolicyTest::Execute()
{
    void* allocation = AllocateSyntheticImage();
    InitializeSyntheticImage(allocation);

    ImageLayout automatic = {};
    ImageLayout pin = {};
    Asserts::AssertEqual(ResolveMappedImage(allocation, false, automatic), STATUS_SUCCESS, SourcePosition);
    Asserts::AssertEqual(ResolveMappedImage(allocation, true, pin), STATUS_SUCCESS, SourcePosition);

    Asserts::AssertTrue(HasRegion(automatic, ERegion::Text), SourcePosition);
    Asserts::AssertTrue(HasRegion(automatic, ERegion::ReadOnlyData), SourcePosition);
    Asserts::AssertTrue(HasRegion(automatic, ERegion::TlsTemplate), SourcePosition);
    Asserts::AssertFalse(HasRegion(automatic, ERegion::Bss), SourcePosition);
    Asserts::AssertTrue(HasRegion(pin, ERegion::Bss), SourcePosition);
    Asserts::AssertEqual(automatic.PhysicalRangeCount, static_cast<SizeT>(2), SourcePosition);
    Asserts::AssertEqual(pin.PhysicalRangeCount, static_cast<SizeT>(1), SourcePosition);

    auto base = reinterpret_cast<SizeT>(allocation);
    auto dos = static_cast<IMAGE_DOS_HEADER*>(allocation);
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    auto sections = IMAGE_FIRST_SECTION(nt);

    nt->Signature = 0;
    Asserts::AssertNotEqual(ResolveMappedImage(allocation, false, automatic), STATUS_SUCCESS, SourcePosition);
    nt->Signature = IMAGE_NT_SIGNATURE;

    sections[0].Misc.VirtualSize = Math::Max<UInt32>();
    Asserts::AssertNotEqual(ResolveMappedImage(allocation, false, automatic), STATUS_SUCCESS, SourcePosition);
    sections[0].Misc.VirtualSize = 0x1800;

    auto tls = reinterpret_cast<IMAGE_TLS_DIRECTORY64*>(base + 0x800);
    tls->EndAddressOfRawData = base + SyntheticImageBytes + 1;
    Asserts::AssertNotEqual(ResolveMappedImage(allocation, false, automatic), STATUS_SUCCESS, SourcePosition);
    tls->EndAddressOfRawData = base + 0x5080;

    dos->e_lfanew = 0x80000;
    Asserts::AssertNotEqual(ResolveMappedImage(allocation, false, automatic), STATUS_SUCCESS, SourcePosition);

    FreeAllocation(allocation);
    return true;
}

bool Seed::Core::Testing::Test::Memory::ImageResidency::SessionLifecycleTest::Execute()
{
    DisableSession();
    Snapshot before = QuerySnapshot();

    Asserts::AssertEqual(EnablePrimarySession(), STATUS_SUCCESS, SourcePosition);
    Asserts::AssertEqual(EnablePrimarySession(), STATUS_SUCCESS, SourcePosition);
    Snapshot enabled = QuerySnapshot();
    Asserts::AssertEqual(enabled.State, ESessionState::Enabled, SourcePosition);

    Boolean sawPrimaryText = false;
    Boolean sawPrimaryBss = false;
    for (SizeT i = 0; i < enabled.RegionCount; i++)
    {
        const auto& region = enabled.Regions[i];
        if (region.Image != EImage::PrimarySeed)
        {
            continue;
        }

        sawPrimaryText = sawPrimaryText || region.Region == ERegion::Text;
        sawPrimaryBss = sawPrimaryBss || region.Region == ERegion::Bss;
        Asserts::AssertEqual(region.ResidentPages, region.Pages, SourcePosition);
        Asserts::AssertEqual(region.LockedPages, region.Pages, SourcePosition);
    }

    Asserts::AssertTrue(sawPrimaryText, SourcePosition);
    Asserts::AssertTrue(sawPrimaryBss, SourcePosition);
    Asserts::AssertEqual(DisableSession(), STATUS_SUCCESS, SourcePosition);
    Asserts::AssertEqual(DisableSession(), STATUS_SUCCESS, SourcePosition);

    Snapshot after = QuerySnapshot();
    Asserts::AssertEqual(after.State, ESessionState::Disabled, SourcePosition);
    if (before.CurrentWorkingSetStatus == STATUS_SUCCESS && after.CurrentWorkingSetStatus == STATUS_SUCCESS)
    {
        Asserts::AssertEqual(after.CurrentMinimumWorkingSet, before.CurrentMinimumWorkingSet, SourcePosition);
        Asserts::AssertEqual(after.CurrentMaximumWorkingSet, before.CurrentMaximumWorkingSet, SourcePosition);
    }

    return true;
}

bool Seed::Core::Testing::Test::Memory::ImageResidency::PartialFailureRollbackTest::Execute()
{
    DisableSession();

    void* allocation = nullptr;
    SIZE_T allocationBytes = 0x1000;
    Asserts::AssertEqual(
        NtAllocateVirtualMemory(
            NtCurrentProcess(),
            &allocation,
            0,
            &allocationBytes,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE),
        STATUS_SUCCESS,
        SourcePosition);

    Asserts::AssertEqual(
        RegisterExternalMapping(EImage::Win32RttiExport, ERegion::Win32RttiExport, allocation, allocationBytes),
        STATUS_SUCCESS,
        SourcePosition);

    void* staleAddress = allocation;
    FreeAllocation(allocation);
    Asserts::AssertNotEqual(EnablePrimarySession(), STATUS_SUCCESS, SourcePosition);

    Snapshot failed = QuerySnapshot();
    Asserts::AssertEqual(failed.State, ESessionState::Disabled, SourcePosition);
    for (SizeT i = 0; i < failed.RegionCount; i++)
    {
        if (failed.Regions[i].Image == EImage::PrimarySeed)
        {
            Asserts::AssertEqual(failed.Regions[i].LockedPages, static_cast<SizeT>(0), SourcePosition);
        }
    }

    UnregisterExternalMapping(staleAddress);
    Asserts::AssertEqual(DisableSession(), STATUS_SUCCESS, SourcePosition);
    return true;
}
