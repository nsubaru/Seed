#pragma once
#include "Seed.Core.Exceptions.ABI.Internal.Types.h"


// Constants for decompression.
inline constexpr Seed::Int8 s_negLengthTab[16] =
{
    -1,    // 0
    -2,    // 1
    -1,    // 2
    -3,    // 3

    -1,    // 4
    -2,    // 5
    -1,    // 6
    -4,    // 7

    -1,    // 8
    -2,    // 9
    -1,    // 10
    -3,    // 11

    -1,    // 12
    -2,    // 13
    -1,    // 14
    -5,    // 15
};

inline constexpr Seed::UInt8 s_shiftTab[16] =
{
    32 - 7 * 1,    // 0
    32 - 7 * 2,    // 1
    32 - 7 * 1,    // 2
    32 - 7 * 3,    // 3

    32 - 7 * 1,    // 4
    32 - 7 * 2,    // 5
    32 - 7 * 1,    // 6
    32 - 7 * 4,    // 7

    32 - 7 * 1,    // 8
    32 - 7 * 2,    // 9
    32 - 7 * 1,    // 10
    32 - 7 * 3,    // 11

    32 - 7 * 1,    // 12
    32 - 7 * 2,    // 13
    32 - 7 * 1,    // 14
    0,             // 15
};

inline Seed::Int32 ReadInt(Seed::UInt8 ** buffer)
{
    int value = *(reinterpret_cast<int *>(*buffer));
    *buffer += sizeof(Seed::Int32);
    return value;
}

// TODO: make sure our overflow read is covered by xdata$aa so if we
// end up reading the first entry we don't go over a section.
inline Seed::UInt32 ReadUnsigned(Seed::UInt8 ** pbEncoding)
{
    Seed::UInt32 lengthBits = **pbEncoding & 0x0F;
    size_t negLength = s_negLengthTab[lengthBits];
    Seed::UInt32 shift = s_shiftTab[lengthBits];
    Seed::UInt32 result = *(reinterpret_cast<Seed::UInt32 *>(*pbEncoding - negLength - 4));

    result >>= shift;
    *pbEncoding -= negLength;

    return result;
}

inline Seed::UInt8 * imageRelToByteBuffer(uintptr_t imageBase, Seed::Int32 disp)
{
    return reinterpret_cast<Seed::UInt8 *>(imageBase + disp);
}

inline ptrdiff_t DecompFuncInfo(Seed::UInt8 * buffer, FuncInfo4 & FuncInfoDe, uintptr_t imageBase, Seed::Int32 functionStart, bool rawIP2StateRVA)
{
    Seed::UInt8 * buffer_start = buffer;
    FuncInfoDe.header.value = buffer[0];
    ++buffer;

    if (FuncInfoDe.header.BBT) {
        FuncInfoDe.bbtFlags = ReadUnsigned(&buffer);
    }

    if (FuncInfoDe.header.UnwindMap) {
        FuncInfoDe.dispUnwindMap = ReadInt(&buffer);
    }

    if (FuncInfoDe.header.TryBlockMap) {
        FuncInfoDe.dispTryBlockMap = ReadInt(&buffer);
    }

    if (rawIP2StateRVA)
    {
        FuncInfoDe.dispIPtoStateMap = ReadInt(&buffer);
    }
    else
    {
        // Find the correct one if this is a separated segment
        if (FuncInfoDe.header.isSeparated)
        {
            int dispToSegMap = ReadInt(&buffer);
            Seed::UInt8 * segBuffer = imageRelToByteBuffer(imageBase, dispToSegMap);
            Seed::UInt32 numSegEntries = ReadUnsigned(&segBuffer);

            // By default, an entry not found in the table corresponds to no
            // states associated with the segment
            FuncInfoDe.dispIPtoStateMap = 0;
            for (Seed::UInt32 i = 0; i < numSegEntries; i++)
            {
                Seed::Int32 segRVA = ReadInt(&segBuffer);
                int dispSegTable = ReadInt(&segBuffer);
                if (segRVA == functionStart)
                {
                    FuncInfoDe.dispIPtoStateMap = dispSegTable;
                    break;
                }
            }
        }
        // Otherwise, the table is directly encoded in the function info
        else
        {
            FuncInfoDe.dispIPtoStateMap = ReadInt(&buffer);
        }
    }

    if (FuncInfoDe.header.isCatch) {
        FuncInfoDe.dispFrame = ReadUnsigned(&buffer);
    }

    return buffer - buffer_start;
}

inline ptrdiff_t DecompFuncInfo(Seed::UInt8 * buffer, FuncInfo4 & FuncInfoDe, uintptr_t imageBase, Seed::Int32 functionStart)
{
    return DecompFuncInfo(buffer, FuncInfoDe, imageBase, functionStart, false);
}

inline uintptr_t __FuncRelToRealOffset(
    DispatcherContext   *pDC, //-V2558
    unsigned int        funcRelOffset
)
{
    return (uintptr_t)(pDC->ImageBase + pDC->FunctionEntry->BeginAddress + funcRelOffset);
}

inline uintptr_t __RVAtoRealOffset(
    DispatcherContext   *pDC, //-V2558
    int                 RVA
)
{
    return (uintptr_t)(pDC->ImageBase + RVA);
}

template<typename TBase, typename TOffset>
inline void* OffsetToAddress(TOffset offset, TBase base) {
    return reinterpret_cast<void*>((Seed::SizeT)(base) + (Seed::SizeT)(offset));
}