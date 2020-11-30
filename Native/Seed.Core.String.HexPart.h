#pragma once
#include "typeinfo"
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.IString.h"
#include "Seed.Core.Math.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.NonConcurency.List.h"
#include "Seed.Core.Containers.NonConcurency.Map.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)

namespace Seed
{
	inline namespace Core
	{
		namespace Details
		{
			template<typename THeap, template<typename THeap> typename TString>
			class THexStringPart {
            private:
				using TBaseString = TString<THeap>;
			protected:
				///<summary>Таблиця ASCI значеннь шістнадцятиричних букв для швидкого перетворення</summary>
				static constexpr int ASCIIHexToInt[] =
				{
					// ASCII
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
					-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

					// 0x80-FF (Omit this if you don't need to check for non-ASCII)
					-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
					-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
					-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
					-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
					-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
					-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
					-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
					-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
				};
            public:
                ///<summary>Масив символів шістнадцятирядкового числа</summary>
                static constexpr const auto HexNumbers = L"0123456789abcdef";
			public:
				///<summary>Конвертаця строки шістнадцятиричних символів в байтовий масив</summary>
                ///<param name = "stringData">Текстове представлення шістнадцятиричних символів</param>
                ///<returns>Масив байт</returns>
                ///<exception cref="Seed::Exceptions::NullPointException">Якщо рядок пустий</exception>
                ///<exception cref="Seed::Exceptions::InvalidFormatException">Якщо рядок містить не коректні символи або його розмір не кратний 2</exception>
                static Containers::BaseArray<THeap, Byte> hexToBinary(const TBaseString& stringData) {
                    if (stringData.Lenght == 0)
                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(&stringData), 0);
                    if (stringData.Lenght % 2)
                        Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);

                    SizeT Size = stringData.Lenght / 2;
                    SizeT i1 = 0, i2 = 0;
                    Containers::BaseArray<THeap, Byte> bytes(Size);
                    //Bite <-> 2 hex digits
                    union {
                        //1 Byte (8 bits)
                        Byte* binary;
                        //2 Hex digits (4x2 bits)
                        struct {
                            //Secon hex (4 bits)
                            Byte bit1 : 4;
                            //First hex (4 bits)
                            Byte bit0 : 4;
                        } *Hex;
                    } conv;
                    for (i1; i1 < Size; i1++) {
                        conv.binary = &bytes[i1];
                        switch (stringData[i2]) {
                        case L'0':
                            conv.Hex->bit0 = 0;
                            break;
                        case L'1':
                            conv.Hex->bit0 = 0x1;
                            break;
                        case L'2':
                            conv.Hex->bit0 = 0x2;
                            break;
                        case L'3':
                            conv.Hex->bit0 = 0x3;
                            break;
                        case L'4':
                            conv.Hex->bit0 = 0x4;
                            break;
                        case L'5':
                            conv.Hex->bit0 = 0x5;
                            break;
                        case L'6':
                            conv.Hex->bit0 = 0x6;
                            break;
                        case L'7':
                            conv.Hex->bit0 = 0x7;
                            break;
                        case L'8':
                            conv.Hex->bit0 = 0x8;
                            break;
                        case L'9':
                            conv.Hex->bit0 = 0x9;
                            break;
                        case L'A':
                        case L'a':
                            conv.Hex->bit0 = 0xa;
                            break;
                        case L'B':
                        case L'b':
                            conv.Hex->bit0 = 0xb;
                            break;
                        case L'C':
                        case L'c':
                            conv.Hex->bit0 = 0xc;
                            break;
                        case L'D':
                        case L'd':
                            conv.Hex->bit0 = 0xd;
                            break;
                        case L'E':
                        case L'e':
                            conv.Hex->bit0 = 0xe;
                            break;
                        case L'F':
                        case L'f':
                            conv.Hex->bit0 = 0xf;
                            break;
                        default:
                            Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                        }
                        i2++;
                        switch (stringData[i2]) {
                        case L'0':
                            conv.Hex->bit1 = 0;
                            break;
                        case L'1':
                            conv.Hex->bit1 = 0x1;
                            break;
                        case L'2':
                            conv.Hex->bit1 = 0x2;
                            break;
                        case L'3':
                            conv.Hex->bit1 = 0x3;
                            break;
                        case L'4':
                            conv.Hex->bit1 = 0x4;
                            break;
                        case L'5':
                            conv.Hex->bit1 = 0x5;
                            break;
                        case L'6':
                            conv.Hex->bit1 = 0x6;
                            break;
                        case L'7':
                            conv.Hex->bit1 = 0x7;
                            break;
                        case L'8':
                            conv.Hex->bit1 = 0x8;
                            break;
                        case L'9':
                            conv.Hex->bit1 = 0x9;
                            break;
                        case L'A':
                        case L'a':
                            conv.Hex->bit1 = 0xa;
                            break;
                        case L'B':
                        case L'b':
                            conv.Hex->bit1 = 0xb;
                            break;
                        case L'C':
                        case L'c':
                            conv.Hex->bit1 = 0xc;
                            break;
                        case L'D':
                        case L'd':
                            conv.Hex->bit1 = 0xd;
                            break;
                        case L'E':
                        case L'e':
                            conv.Hex->bit1 = 0xe;
                            break;
                        case L'F':
                        case L'f':
                            conv.Hex->bit1 = 0xf;
                            break;
                        default: Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                        }
                        i2++;
                    }
                    return bytes;
                }

                ///<summary>Перетворення числа, структури, класу чи покажчика в hex рядок</summary>
                ///<param name = "Data">Дані які перетворить в шістнадцятиричне представлення</param>
                template<typename T> requires Concepts::IsPod<T>
                ForceInline static TBaseString toHexString(const T& Data) {
                    return binaryToHex(Containers::Array<Byte>(reinterpret_cast<const Byte*>(&Data), sizeof(T)));
                }

                ///<summary>Перетворення байтового масиву в строку з 16 бітних символів (0-9a-f)</summary>
                ///<param name = "binaryData">Масив байтів</param>
                ///<returns>Рядкове представлення бінарних даних</returns>
                static TBaseString binaryToHex(const Containers::BaseArray<THeap, Byte>& binaryData) {
                    //Bite <-> 2 hex digits
                    union {
                        //1 Byte (8 bits)
                        const Byte* binary;
                        //2 Hex digits (4x2 bits)
                        const struct {
                            //Secon hex (4 bits)
                            Byte LowPart : 4;
                            //First hex (4 bits)
                            Byte HighPart : 4;
                        } *Hex;
                    } conv = { 0 };

                    if (binaryData.Count % 2 != 0)
                        Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);

                    auto str = TBaseString::CreateFilledString(binaryData.Count * 2);

                    SizeT i1 = 0, i2 = 0;
                    for (i1; i1 < binaryData.Count; i1++) {
                        conv.binary = reinterpret_cast<const Byte*>(&binaryData[i1]);

                        str.str[i2] = HexNumbers[conv.Hex->HighPart];
                        i2++;
                        str.str[i2] = HexNumbers[conv.Hex->LowPart];
                        i2++;
                    }

                    return str;
                }

                ///<summary>Шістнадцятиричне представлення UInt64</summary>
                ///<param name = "Value">Вхідне значення</param>
                ///<returns>Рядкове представлення бінарних даних</returns>
                static TBaseString UInt64asHex(UInt64 Value) {
                    union ConvertT {
                        UInt64 Variable;
                        struct {
                            Byte LowPart : 4;
                            Byte HighPart : 4;
                        }
                        Code[sizeof(UInt64)];
                    } converter{ .Variable = Value };

                    TBaseString str(L"0x0000000000000000");

                    for (int i = 0; i < sizeof(Value) * 2; i += 2) {
                        str.str[i + 3] = HexNumbers[converter.Code[(sizeof(Value) - 1) - (i / 2)].LowPart];
                        str.str[i + 2] = HexNumbers[converter.Code[(sizeof(Value) - 1) - (i / 2)].HighPart];
                    }

                    return str;
                }

                ///<summary>Шістнадцятиричне представлення UInt64</summary>
                ///<param name = "Value">Вхідне значення</param>
                ///<returns>Рядкове представлення бінарних даних</returns>
                static TBaseString Int32asHex(Int32 Value) {
                    union ConvertT {
                        Int32 Variable;
                        struct {
                            Byte LowPart : 4;
                            Byte HighPart : 4;
                        }
                        Code[sizeof(Int32)];
                    } converter{ .Variable = Value };

                    TBaseString str(L"0x00000000");

                    for (int i = 0; i < sizeof(Value) * 2; i += 2) {
                        str.str[i + 3] = HexNumbers[converter.Code[(sizeof(Value) - 1) - (i / 2)].LowPart];
                        str.str[i + 2] = HexNumbers[converter.Code[(sizeof(Value) - 1) - (i / 2)].HighPart];
                    }

                    return str;
                }
			protected:
                ForceInline virtual ~THexStringPart() = 0 {}
			};
		}
	}
}

#pragma warning(pop)