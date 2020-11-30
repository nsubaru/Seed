#pragma once
#include "typeinfo"
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.IString.h"
#include "Seed.Core.Math.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Time.Base.h"
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
        inline namespace Exceptions {
            class IntegerOverflowException;
        }

		namespace Details
		{
			template<typename THeap, template<typename THeap> typename TString>
			class TConvertorsStringPart {
            private:
				using TBaseString = TString<THeap>;
			public:
#pragma region Converting to string
#pragma region Converting int types to string
#pragma region Convertinh signed int types to string
                ///<summary>Конвертація цілого числа типу Int8 до рядка</summary>
                ///<returns>Вертає рядкове представлення числа</returns>
                static TBaseString toString(Int8 val) {
                    if (val == 0) return TBaseString(L"0");
                    Boolean sign = Math::Sign(val);
                    Char buf[5];
                    SizeT i = 0;
                    if (sign) Math::Abs(val);
                    do {
                        buf[i] = L'0' + (val % 10);
                        i++;
                        val /= 10;
                    } while (val != 0);
                    if (sign) {
                        buf[i] = L'-';
                        i++;
                    }
                    buf[i] = L'\0';
                    i++;
                    TBaseString str;
                    str.CurrentSize = i;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                    Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                    return str;
                }

                ///<summary>Конвертація цілого числа типу Int16 до рядка</summary>
                ///<returns>Вертає рядкове представлення числа</returns>
                static TBaseString toString(Int16 val) {
                    if (val == 0) return TBaseString(L"0");
                    Boolean sign = Math::Sign(val);
                    Char buf[10];
                    if (sign) Math::Abs(val);
                    SizeT i = 0;
                    do {
                        buf[i] = L'0' + (val % 10);
                        i++;
                        val /= 10;
                    } while (val != 0);
                    if (sign) {
                        buf[i] = L'-';
                        i++;
                    }
                    buf[i] = L'\0';
                    i++;
                    TBaseString str;
                    str.CurrentSize = i;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                    Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                    return str;
                }

                ///<summary>Конвертація цілого числа типу Int32 до рядка</summary>
                ///<returns>Вертає рядкове представлення числа</returns>
                static TBaseString toString(Int32 val) {
                    if (val == 0) return TBaseString(L"0");
                    Boolean sign = Math::Sign(val);
                    Char buf[20];
                    SizeT i = 0;
                    if (sign) Math::Abs(val);
                    do {
                        buf[i] = L'0' + (val % 10);
                        i++;
                        val /= 10;
                    } while (val != 0);
                    if (sign) {
                        buf[i] = L'-';
                        i++;
                    }
                    buf[i] = L'\0';
                    i++;
                    TBaseString str;
                    str.CurrentSize = i;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                    Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                    return str;
                }
                ///<summary>Конвертація цілого числа типу Int64 до рядка</summary>
                ///<returns>Вертає рядкове представлення числа</returns>
                static TBaseString toString(Int64 val) {
                    if (val == 0) return TBaseString(L"0");
                    Boolean sign = Math::Sign(val);
                    Char buf[30];
                    SizeT i = 0;
                    if (sign) Math::Abs(val);
                    do {
                        buf[i] = L'0' + (val % 10);
                        i++;
                        val /= 10;
                    } while (val != 0);
                    if (sign) {
                        buf[i] = L'-';//
                        i++;
                    }
                    buf[i] = L'\0';
                    i++;
                    TBaseString str;
                    str.CurrentSize = i;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                    Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                    return str;
                }
#pragma endregion
#pragma region Convert unsigned int types to string
                ///<summary>Конвертація цілого числа типу UInt8 до рядка</summary>
                ///<returns>Вертає рядкове представлення числа</returns>
                static TBaseString toString(UInt8 val) {
                    if (val == 0) return TBaseString(L"0");
                    Char buf[5];
                    SizeT i = 0;
                    do {
                        buf[i] = L'0' + (val % 10);
                        i++;
                        val /= 10;
                    } while (val != 0);
                    buf[i] = L'\0';
                    i++;
                    TBaseString str;
                    str.CurrentSize = i;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                    Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                    return str;
                }

                ///<summary>Конвертація цілого числа типу UInt16 до рядка</summary>
                ///<returns>Вертає рядкове представлення числа</returns>
                static TBaseString toString(UInt16 val) {
                    if (val == 0) return TBaseString(L"0");
                    Char buf[10];
                    SizeT i = 0;
                    do {
                        buf[i] = L'0' + (val % 10);
                        i++;
                        val /= 10;
                    } while (val != 0);
                    buf[i] = L'\0';
                    i++;
                    TBaseString str;
                    str.CurrentSize = i;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                    Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                    return str;
                }

                ///<summary>Конвертація цілого числа типу UInt32 до рядка</summary>
                ///<returns>Вертає рядкове представлення числа</returns>
                static TBaseString toString(UInt32 val) {
                    if (val == 0) return TBaseString(L"0");
                    Char buf[15];
                    SizeT i = 0;
                    do {
                        buf[i] = L'0' + (val % 10);
                        i++;
                        val /= 10;
                    } while (val != 0);
                    buf[i] = L'\0';
                    i++;
                    TBaseString str;
                    str.CurrentSize = i;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                    Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                    return str;
                }

                ///<summary>Конвертація цілого числа до рядка</summary>
                ///<returns>Вертає рядкове представлення числа</returns>
                static TBaseString toString(UInt64 val) {
                    if (val == 0) return TBaseString(L"0");
                    Char buf[30];
                    SizeT i = 0;
                    do {
                        buf[i] = L'0' + (val % 10);
                        i++;
                        val /= 10;
                    } while (val != 0);
                    buf[i] = L'\0';
                    i++;
                    TBaseString str;
                    str.CurrentSize = i;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                    Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                    return str;
                }
#pragma endregion
#pragma endregion
                ///<summary>Конвертація логічного значення до рядка вертає <c>"True"</c> або <c>"False"</c></summary>
                ///<params name = "val">Логічне значення</params>
                ///<returns><c>"True"</c> якщо <c>val == true</c> інакше <c>"False"</c></returns>
                ForceInline static TBaseString toString(Boolean val) {
                    if (val) return TBaseString(L"true"); else return TBaseString(L"false");
                }
                ///<summary>Конвертація логічного значення до рядка вертає вміст рядка <c>IfTrue</c> або <c>IfFalse</c></summary>
                ///<params name = "val">Логічне значення</params>
                ///<params name = "IfTrue">Рядок який верне якщо <c>val == true</c></params>
                ///<params name = "IfFalse">Рядок який верне якщо <c>val == false</c></params>
                ///<returns><c>IfTrue</c> або <c>IfFalse</c> залежно від значення <c>val</c></returns>
                ForceInline static TBaseString toString(Boolean val, const TBaseString& IfTrue, const TBaseString& IfFalse) {
                    if (val) return IfTrue; else return IfFalse;
                }
#pragma endregion
#pragma region Converting string to types
#pragma region Converting float typed to string
                ///<summary>Конвертація дійсного числа з одинарною точністю до рядка з певною кількістю знаків після коми</summary>
                ///<param name = "val">Число яке конвертуватиме</param>
                ///<param name = "precision">Кількість знаків після коми</param>
                ///<returns>Рядкове представлення числа</returns>
                static TBaseString toString(Float val, UInt8 precision = 3) {
                    using namespace Math;
                    
                    if (val != val) return L"NaN";

                    if (Math::Equals(val, Infinity)) return L"Infinity"; //-V550
                    if (Math::Equals(val, -Infinity)) return L"-Infinity"; //-V550
                    if (Math::Equals(val, 0.0)) return L"0"; //-V550

                    if (precision > 10) precision = 10;

                    Boolean sign = Math::Sign(val);
                    if (sign) val *= -1.0;
                    Float IntPart = 0;
                    Float DoublePart = Math::ModD(val, IntPart);
                    DoublePart *= Math::Pow(10, precision);
                    Char buf[50] = { 0 };
                    SizeT size = 0;
                    while (DoublePart >= 1) {
                        buf[size] = L'0' + (Int32)(DoublePart) % 10; //-V2005 //-V2003
                        size++;
                        DoublePart /= 10;
                    }
                    if(size != precision) for (size; size < precision; size++) buf[size] = L'0';
                    buf[size] = L'.';
                    size++;
                    if (Math::Equals(IntPart, 0.0F)) { //-V550
                        buf[size] = L'0';
                        size++;
                    }
                    else
                        while (IntPart >= 1) {
                            buf[size] = L'0' + (Int32)(IntPart) % 10; //-V2005 //-V2003
                            size++;
                            IntPart /= 10;
                        }
                    if (sign) {
                        buf[size] = '-';
                        size++;
                    }
                    buf[size] = L'\0';
                    Seed::Core::Reverse(buf, size);
                    size++;
                    TBaseString str;
                    str.CurrentSize = size;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, (str.CurrentSize) * sizeof(Char));
                    return str;
                }

                ///<summary>Конвертація дійсного числа з подвійною точністю до рядка з певною кількістю знаків після коми</summary>
                ///<param name = "val">Число яке конвертуватиме</param>
                ///<param name = "precision">Кількість знаків після коми</param>
                ///<returns>Рядкове представлення числа</returns>
                static TBaseString toString(Double val, UInt8 precision = 5) {
                    using namespace Math;
                    
                    if (val != val) return L"NaN";

                    if (Math::Equals(val, Infinity)) return L"Infinity"; //-V550
                    if (Math::Equals(val, -Infinity)) return L"-Infinity"; //-V550
                    if (Math::Equals(val, 0.0)) return L"0"; //-V550

                    if (precision > 10) precision = 10;
                    Boolean sign = Math::Sign(val);
                    if (sign) val *= -1.0;
                    Double IntPart = 0;
                    Double DoublePart = Math::ModD(val, IntPart);
                    DoublePart *= Math::Pow(10, precision);
                    Char buf[50] = { 0 };
                    SizeT size = 0;
                    while (DoublePart >= 1) {
                        buf[size] = L'0' + (Int32)(DoublePart) % 10; //-V2005 //-V2003
                        size++;
                        DoublePart /= 10;
                    }
                    if(size != precision) for (size; size < precision; size++) buf[size] = L'0';
                    buf[size] = L'.';
                    size++;
                    if (Math::Equals(IntPart, 0.0)) { //-V550
                        buf[size] = L'0';
                        size++;
                    }
                    else
                        while (IntPart >= 1) {
                            buf[size] = L'0' + (Int32)(IntPart) % 10; //-V2005 //-V2003
                            size++;
                            IntPart /= 10;
                        }
                    if (sign) {
                        buf[size] = '-';
                        size++;
                    }
                    buf[size] = L'\0';
                    Seed::Core::Reverse(buf, size);
                    size++;
                    TBaseString str;
                    str.CurrentSize = size;
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                    Memory::Copy(buf, str.str, (str.CurrentSize) * sizeof(Char));
                    return str;
                }
#pragma endregion
#pragma region Converting to int types
#pragma region Converting to unsigned int types
                ///<summary>Перетворення рядка в ціле число</summary>
                ///<typeparam name = "T">Тип чиса</typeparam>
                ///<returns>Числове представлення</returns>
                ///<exception cref="Seed::Exceptions::NullPointException">якщо рядок пустий</exception>
                ///<exception cref="Seed::Exceptions::IntegerOverflowException">якщо число є завеликим</exception>
                ///<exception cref="Seed::Exceptions::InvalidFormatException">якщо в рядку є відміний від числа символ</exception>
                template<typename T = Seed::Int32>
                T toInt() const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if (This->CurrentSize == 0 || This->Lenght == 0)
                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This), 0);
                    if (This->CurrentSize > Math::MaxNumberCount<T>())
                        Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);

                    if constexpr (TypeTraits::IsSameV<TypeTraits::Unsigned<T>, T>) {
                        Math::SafeInt<T> ret;
                        for (SizeT i = 0; i < This->Lenght; i++) {
                            if (!(L'0' <= This->str[i] && This->str[i] <= L'9'))
                                Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                            ret += static_cast<UInt8>(This->str[i] - L'0');
                            if (i < This->CurrentSize - 2)
                                ret *= 10;
                        }
                        return ret.Value;
                    }
                    else {
                        Boolean sign(This->str[0] == '-');
                        SizeT i = 0;
                        if (This->str[0] == L'-' || This->str[0] == L'+')
                            i = 1;
                        Math::SafeInt<T> ret = 0;
                        for (i; i < This->Lenght; i++) {
                            if (!(L'0' <= This->str[i] && This->str[i] <= L'9'))
                                Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                            ret += This->str[i] - L'0';
                            if (i < This->CurrentSize - 2)
                                ret *= 10;
                        }
                        return Math::ConvertUnsignedToSigned<T>(ret.Value) * ((sign) ? -1 : 1);
                    }
                }
#pragma endregion
#pragma endregion
#pragma region Converting to float point types
                ///<summary>Перетворення тексту в дійсне число Float. На початку може стояти символ '+' або '-'</summary>
                ///<returns>Числове представлення</returns>
                ///<exception cref="Seed::Exceptions::NullPointException">якщо рядок пустий</exception>
                ///<exception cref="Seed::Exceptions::IntegerOverflowException">якщо кількість цифр є більшою ніж можу вмістити число Float</exception>
                ///<exception cref="Seed::Exceptions::InvalidFormatException">якщо в строці є відміні від цифр і одної крапки символи</exception>
                Float toFloat() const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if (This->CurrentSize == 0)
                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This), 0);

                    if (This->CurrentSize > 18)
                        Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);

                    Boolean sign(This->str[0] == '-');
                    SizeT i = 0;
                    if (This->str[0] == L'-' || This->str[0] == L'+')
                        i = 1;

                    Float int_part = 0.0, double_part = 0.0;
                    for (i; i < This->Lenght && This->str[i] != L'.'; i++) {
                        if (!(L'0' <= This->str[i] && This->str[i] <= L'9'))
                            Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                        int_part += This->str[i] - L'0';
                        int_part *= 10;
                    }

                    int_part /= 10;

                    if (i != This->CurrentSize) {
                        for (SizeT j = This->Lenght - 1; j > i; j--)
                        {
                            if (!(L'0' <= This->str[j] && This->str[j] <= L'9'))
                                Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                            double_part += This->str[j] - L'0';
                            double_part /= 10;
                        }
                        int_part += double_part;
                    }

                    return int_part * ((sign) ? -1.0f : 1.0f);
                }

                ///<summary>Перетворення тексту в дійсне число Double. На початку може стояти символ '+' або '-'</summary>
                ///<returns>Числове представлення</returns>
                ///<exception cref="Seed::Exceptions::NullPointException">якщо рядок пустий</exception>
                ///<exception cref="Seed::Exceptions::IntegerOverflowException">якщо кількість цифр є більшою ніж можу вмістити число Double</exception>
                ///<exception cref="Seed::Exceptions::InvalidFormatException">якщо в строці є відміні від цифр і одної крапки символи</exception>
                Double toDouble() const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if (This->CurrentSize == 0)
                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This), 0);

                    if (This->CurrentSize > 18)
                        Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);

                    Boolean sign(This->str[0] == '-');
                    SizeT i = 0;
                    if (This->str[0] == L'-' || This->str[0] == L'+')
                        i = 1;

                    Double int_part = 0.0, double_part = 0.0;
                    for (i; i < This->Lenght && This->str[i] != L'.'; i++) {
                        if (!(L'0' <= This->str[i] && This->str[i] <= L'9'))
                            Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                        int_part += This->str[i] - L'0';
                        int_part *= 10;
                    }

                    int_part /= 10;
                    if (i != This->CurrentSize) {
                        for (SizeT j = This->Lenght - 1; j > i; j--)
                        {
                            if (!(L'0' <= This->str[j] && This->str[j] <= L'9'))
                                Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                            double_part += This->str[j] - L'0';
                            double_part /= 10;
                        }
                        int_part += double_part;
                    }
                    return int_part * ((sign) ? -1.0 : 1.0);
                }

                /// <summary>
                /// Перетворення рядка в логічне значення, 
                /// якщо 'true' або 'True' то вертає true, 
                /// якщо 'false' або 'False' то вертає false,
                /// інакше генерує помилку InvalidArgumentException
                /// </summary>
                /// <exception cref="Seed::Exceptions::InvalidArgumentException">Якщо рядок неможливо перетворити в логічне значення</exception>
                /// <returns>Логічне значення</returns>
                ForceInline Boolean toBoolean() const {
                    const TBaseString& This = static_cast<const TBaseString&>(*this);

                    if (This == L"true" || This == L"True")
                        return true;
                    else if (This == L"false" || This == L"False")
                        return false;
                    else
                        Internals::Exceptions::ThrowInvalidArgumentException(__FILE__, __FUNCSIG__, __LINE__, L"this", L"не вірне значення рядка");
                }
#pragma endregion
                ///<summary>Вертає рядкове представлення UUID</summary>
                ///<returns>Рядкове представлення UUID в вигляді {????????-????-????-????????????????}</returns>
                ForceInline static TBaseString toString(const UUID& id) {
                    auto ReservedSize = sizeof(UUID::UUIDStringWildCard) + CurrentSettings().MemoryManager.StringReserveSize;
                    CharStr buffer = reinterpret_cast<CharStr>(TBaseString::operator new[](sizeof(UUID::UUIDStringWildCard)));
                    Internals::ConvertUUIDtoCharStr(id, buffer);
                    return TBaseString(MoveRValue(buffer), sizeof(UUID::UUIDStringWildCard), ReservedSize);
                }

                /// <summary>
                /// Перетворення дати і часу в рядкове представлення
                /// </summary>
                /// <typeparam name="date">Об'єкт дати</typeparam>
                ForceInline static TBaseString toString(Seed::Core::Time::Date date) {
                    Char buffer[22] = { 0 };
                    Internals::ConvertDateToStringImpl(date, buffer);
                    return buffer;
                }

                /// <summary>
                /// Перетворення дати і часу в рядкове представлення
                /// </summary>
                /// <param name="time">Об'єкт часу і дати</param>
                ForceInline static TBaseString toString(Seed::Core::Time::DateTime time) {
                    Char buffer[22] = { 0 };
                    Internals::ConvertDateTimeToStringImpl(time, buffer);
                    return buffer;
                }

                /// <summary>
                /// Перетворення часу і дати в рядкове представлення
                /// </summary>
                /// <param name="dateSpan">Об'єкт відрізку дат</param>
                ForceInline static TBaseString toString(Seed::Core::Time::DateSpan dateSpan) {
                    Char buffer[22] = { 0 };
                    auto value = static_cast<Time::DateTime>(dateSpan);
                    Internals::ConvertDateTimeToStringImpl(value, buffer);
                    return buffer;
                }

                /// <summary>
                /// Перетворення часу і дати в рядкове представлення
                /// </summary>
                /// <param name="time">Об'єкт відрізку часу</param>
                ForceInline static TBaseString toString(Seed::Core::Time::TimeSpan time) {
                    Char buffer[22] = { 0 };
                    auto value = static_cast<Time::DateTime>(static_cast<Time::DateSpan>(time));
                    Internals::ConvertDateTimeToStringImpl(value, buffer);
                    return buffer;
                }

                /// <summary>
                /// Перетворення часу і дати в рядкове представлення
                /// </summary>
                /// <param name="time">Об'єкт діапазону часу</param>
                static TBaseString toString(Seed::Core::Time::TimeRange time) {
                    Char buffer[47] = { 0 };
                    Internals::ConvertDateTimeToStringImpl(time.StartTime, buffer);
                    buffer[21] = L' ';
                    buffer[22] = L'-';
                    buffer[23] = L' ';
                    auto value = static_cast<Time::DateTime>(time.StartTime.Ticks + time.Durration.Ticks);
                    Internals::ConvertDateTimeToStringImpl(value, buffer + 24);
                    return buffer;
                }

                /// <summary>
                /// Перетворює в рядок опис CxxRtti опис типу
                /// </summary>
                /// <param name="typeInfo">CxxRtti опис типу</param>
                /// <returns>Кодована назва типу</returns>
                ForceInline static TBaseString toString(const TypeInfo& typeInfo) {
                    return TBaseString<THeap>(const_cast<AChar*>(typeInfo.GetCxxName()));
                }

                ///<summary>Приведення бінарного тексту до рядкової зміної</summary>
                ///<params name = "array">Масив букв</params>
                ///<returns>Рядок з текстом з масиву</returns>
                InternalApiCall static TBaseString toString(const Containers::BaseArray<THeap, Seed::Byte>& array) {
                    TBaseString str;
                    str.CurrentSize = array.Count / sizeof(Char);
                    str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    str.str = (CharStr)TBaseString::operator new[](str.ReservedSize * sizeof(Char));//new Char[str.CurrentSize + 1];
                    Memory::Copy(array.GetRaw(), str.str, array.Count);
                    str.str[str.CurrentSize] = L'\0';
                    return str;
                }

                /// <summary>
                /// Перетворення перелічення в рядкове представлення
                /// </summary>
                /// <params name="enumValue">Числове значення перелічення</typeparam>
                template<typename T> requires TypeTraits::IsEnumV<T>
                ForceInline static TBaseString toString(T enumValue) {
                    return TBaseString::toString((TypeTraits::EnumUnderlyingTypeV<T>)enumValue);
                }

                /// <summary>
                /// Перетворення текстового представлення в значення перелічення
                /// </summary>
                /// <typeparam name="T">Тип перелічення</typeparam>
                /// <returns>Значення перелічення</returns>
                template<typename T> requires TypeTraits::IsEnumV<T>
                ForceInline T toEnum() const {
                    return (T) this->toInt<typename TypeTraits::EnumUnderlyingTypeV<T>>();
                }

                /// <summary>
                /// Перетворення розмірного типу в рядок
                /// </summary>
                /// <params name = "dataSize"> Значення </params>
                /// <params name = "formatSize"> Якщо true то використовується форматування, якщо ні то перетворюється як кількість байт </params>
                /// <returns> Рядкове предствлення </returns>
                ForceInline static TBaseString toString(Memory::DataSizeT dataSize, bool formatSize = false) {
                    if (!formatSize) {
                        return TBaseString::toString(dataSize.Bytes);
                    }
                    else {
                        SizeT data = dataSize.Bytes;

                        if (data < Memory::DataSizeT::Kilobyte) {
                            auto result = TBaseString::toString(dataSize.Bytes);
                            result += L" b";
                            return result;
                        }
                        else if (data < Memory::DataSizeT::Megabyte) {
                            auto result = TBaseString::toString(dataSize.Bytes / (Memory::DataSizeT::Kilobyte * 1.0), 3);
                            result += L" Kb";
                            return result;
                        }
                        else if (data < Memory::DataSizeT::Gigabyte) {
                            auto result = TBaseString::toString(dataSize.Bytes / (Memory::DataSizeT::Megabyte * 1.0), 3);
                            result += L" Mb";
                            return result;
                        }
                        else if (data < Memory::DataSizeT::Terabyte) {
                            auto result = TBaseString::toString(dataSize.Bytes / (Memory::DataSizeT::Gigabyte * 1.0), 3);
                            result += L" Gb";
                            return result;
                        }
                        else if (data < Memory::DataSizeT::Petabyte) {
                            auto result = TBaseString::toString(dataSize.Bytes / (Memory::DataSizeT::Terabyte * 1.0), 3);
                            result += L" Tb";
                            return result;
                        }
                        else if (data < Memory::DataSizeT::Exabyte) {
                            auto result = TBaseString::toString(dataSize.Bytes / (Memory::DataSizeT::Petabyte * 1.0), 3);
                            result += L" Pb";
                            return result;
                        }
                        else {
                            auto result = TBaseString::toString(dataSize.Bytes / (Memory::DataSizeT::Exabyte * 1.0), 3);
                            result += L" Eb";
                            return result;
                        }
                    }
                }

                ///<summary>Конвертація з рядкового в бінарне представлення UUID </summary>
                ///<returns>UUID</returns>
                ForceInline UUID toUUID() const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if (This->str == nullptr || This->CurrentSize == 0)
                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This), 0);
                    UUID res;
                    if (!Internals::ConvertCharStrToUUID(This->str, res)) {
                        Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                    }
                    return res;
                }
#pragma endregion
			protected:
                ForceInline virtual ~TConvertorsStringPart() = 0 {}
			};
		}
	}
}

#pragma warning(pop)