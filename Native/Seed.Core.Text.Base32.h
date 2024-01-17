#pragma once
#include "Seed.Core.Text.Base.h"

namespace Seed::Core::Text
{
	/// <summary>
	/// Base32 converter class
	/// </summary>
	class Base32 final {
	private:
        static constexpr Char base32map[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

        static UInt8 DecodeSymbol(Char sym)
        {
            UInt8 i = 0;
            for (i = 0; i < 32; i++)
            {
                if (sym == base32map[i])
                    return i;
            }
            return Math::Max<UInt8>();
        }

	public:
        /// <summary>
        /// Converts Byte32 string to binnary data array
        /// </summary>
        /// <typeparam name="THeap">Result heap</typeparam>
        /// <typeparam name="UHeap">Input heap</typeparam>
        /// <param name="text">Input Base32 text</param>
        /// <returns>Binnary array</returns>
        template<typename THeap = Memory::Heap, typename UHeap = Memory::Heap>
        static Containers::BaseArray<THeap, Byte> Decode(const BaseString<UHeap>& text)
        {
            if (text.Length == 0)
            {
                return {};
            }

            if (text.Length % 8 != 0)
            {
                Throw(InvalidArgumentException(SourcePosition, nameof(text.Length), L"text.Lenght must be dividible by 8"));
            }

            SizeT blocksCount = text.Length / 8;
            
            SizeT restBytes = 0;
            if (text.IsEndWith("======"))
            {
                restBytes = 1;
            }
            else if (text.IsEndWith("===="))
            {
                restBytes = 2;
            }
            else if (text.IsEndWith("==="))
            {
                restBytes = 3;
            }
            else if (text.IsEndWith("="))
            {
                restBytes = 4;
            }

            if (restBytes != 0)
            {
                blocksCount--;
            }

            auto res = Containers::BaseArray<THeap, Byte>((blocksCount * 5) + restBytes);
            auto dest = (Byte*) res.GetRaw();
            auto src = (CharStr) (ConstCharStr) text;
            
            auto destLenBits = res.Count * 8;

            SizeT outBits = 0;

            for (; *src; src++)
            {
                Byte sym;
                SizeT sbits, dbits, b;

                if (Characters::IsSpace(*src) || *src == '-')
                {
                    continue;
                }

                sym = DecodeSymbol(*src);

                if (sym == Math::Max<SizeT>())
                {
                    Throw(InvalidFormatException(SourcePosition));
                }

                // See how many bits we get to use from this symbol 
                sbits = Min(5ull, destLenBits - outBits);
                if (sbits < 5)
                {
                    sym >>= (5 - sbits);
                }
                
                // Fill up the rest of the current byte
                dbits = 8 - (outBits & 7);
                b = Min(dbits, sbits);

                if (dbits == 8)
                {
                    dest[outBits / 8] = 0;  // Starting a new byte
                }
                
                dest[outBits / 8] |= (sym << (dbits - b)) >> (sbits - b);
                outBits += b;
                sbits -= b;

                // Start the next byte if there's space
                if (sbits > 0)
                {
                    dest[outBits / 8] = sym << (8 - sbits);
                    outBits += sbits;
                }
            }

            return res;
        }

        /// <summary>
        /// Convert data blob to Base32 string
        /// </summary>
        /// <typeparam name="THeap">Result heap</typeparam>
        /// <typeparam name="UHeap">Input heap</typeparam>
        /// <param name="blob">Input blob</param>
        /// <returns>Base32 string</returns>
        template<typename THeap = Memory::Heap, typename UHeap = Memory::Heap>
        static BaseString<THeap> Encode(const Containers::BaseArray<UHeap, Byte>& blob)
        {
            if (blob.Count == 0)
            {
                return {};
            }

            auto result = BaseString<THeap>::CreateFilledString(((blob.Count / 5) * 8) + (blob.Count % 5 != 0 ? 8 : 0), L'=');
            auto src = blob.GetRaw();
            auto srcLenBits = blob.Count * 8;
            auto dest = (CharStr) (ConstCharStr) result;
            auto didx = 0Sz;

            for (SizeT i = 0; i < srcLenBits; i += 5)
            {
                Char sym;
                SizeT sidx = i / 8;
                SizeT bitOffset = i % 8;

                if (bitOffset <= 3)
                {
                    // Entire symbol fits in that byte 
                    sym = src[sidx] >> (3 - bitOffset);
                }
                else
                {
                    // Use the bits we have left 
                    sym = src[sidx] << (bitOffset - 3);

                    // Use the bits from the next byte, if any 
                    if (i + 1 < srcLenBits)
                    {
                        sym |= src[sidx + 1] >> (11 - bitOffset);
                    }
                }

                sym &= 0x1f;

                // Pad incomplete symbol with 0 bits
                if (srcLenBits - i < 5)
                {
                    sym &= 0x1f << (5 + i - srcLenBits);
                }

                dest[didx++] = base32map[sym];
            }

            return result;
        }

    public:
        /// <summary>
        /// Converts Byte32 string to binnary data array
        /// </summary>
        /// <typeparam name="THeap">Result heap</typeparam>
        /// <typeparam name="UHeap">Input heap</typeparam>
        /// <param name="text">Input Base32 text</param>
        /// <returns>Binnary array</returns>
        template<typename THeap = Memory::Heap, typename UHeap = Memory::Heap>
        static Containers::BaseArray<THeap, Byte> Decode(BaseString<UHeap>&& text)
        {
            return Decode<THeap>(text);
        }

        /// <summary>
        /// Convert data blob to Base32 string
        /// </summary>
        /// <typeparam name="THeap">Result heap</typeparam>
        /// <typeparam name="UHeap">Input heap</typeparam>
        /// <param name="blob">Input blob</param>
        /// <returns>Base32 string</returns>
        template<typename THeap = Memory::Heap, typename UHeap = Memory::Heap>
        static BaseString<THeap> Encode(Containers::BaseArray<UHeap, Byte>&& blob)
        {
            return Encode<THeap>(blob);
        }
	};
}