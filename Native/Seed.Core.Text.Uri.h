#pragma once
#include "Seed.Core.Text.Base.h"
#include "Seed.Core.Containers.NonConcurrency.FixedArray.h"
#include "Seed.Core.Containers.NonConcurrency.FixedArrayImpl.h"

namespace Seed::Core::Text
{
    /// <summary>
    /// Encode and decode URI
    /// </summary>
    class Uri final {
    public:
        /// <summary>
        /// Encode string to URI format
        /// </summary>
        /// <typeparam name="THeap">String heap</typeparam>
        /// <param name="plainText">Text to encode</param>
        /// <returns>Encoded text</returns>
        template<typename THeap>
        static BaseString<THeap> Encode(const BaseString<THeap>& plainText) {
            BaseString<THeap> res(plainText.Size * 3Sz); 

            for (SizeT i = 0; i < plainText.Length; i++)
            {
                if (Characters::IsAlphaNumeric(plainText[i]) || plainText[i] == '-' || plainText[i] == '_' || plainText[i] == '.' || plainText[i] == '~')
                {
                    res += plainText[i];
                }
                else
                {
                    res += L'%';
                    res += String::AsHexInteger((Int8) plainText[i], false); // Ensure two-digit hex
                }
            }

            return res;
        }

        /// <summary>
        /// Decode URI string to plain text
        /// </summary>
        /// <typeparam name="THeap">String heap</typeparam>
        /// <param name="uri">URI encoded string</param>
        /// <returns>Plain text</returns>
        template<typename THeap>
        static BaseString<THeap> Decode(const BaseString<THeap>& uri) {
            BaseString<THeap> res(uri.Size);
            BaseString<THeap> buffer(MemDataSizeOf<Char>() * 2Sz);

            for (SizeT i = 0; i < uri.Length; i++)
            {
                if (uri[i] != L'%')
                {
                    res += uri[i];
                }
                else
                {
                    if (i + 2 < uri.Length && Characters::IsHexDigit(uri[i + 1]) && Characters::IsHexDigit(uri[i + 2]))
                    {
                        buffer.Shrink();
                        buffer += uri[i + 1];
                        buffer += uri[i + 2];
                        res += static_cast<Char>(buffer.HexToInteger<Int8>());
                        i += 2; // Skip over the two hex digits
                    }
                    else
                    {
                        Internals::Exceptions::ThrowInvalidArgumentException(SourcePosition, nameof(uri), L"Invalid percent-encoding in URI");
                    }
                }
            }

            return res;
        }
    };
}