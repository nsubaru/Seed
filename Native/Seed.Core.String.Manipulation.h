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
#include "Seed.Core.CTTI.TypeName.Details.h"

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
			class TManipulationStringPart {
            private:
				using TBaseString = TString<THeap>;
			public:
                /// <summary>
                /// Вертає новий рядок який містить частину поточного рядка
                /// </summary>
                /// <param name="startPos">Позиція першого символа який потрібно скопіювати</param>
                /// <param name="count">Кількість символів</param>
                /// <returns>Новий рядок який містить частину поточного рядка</returns>
				TBaseString SubString(SizeT startPos, SizeT count = Math::Max<SizeT>()) const {
					if (count == 0)
						return TBaseString();

					const TBaseString* This = static_cast<const TBaseString*>(this);

					CharStr str = This->str;
					SizeT size = This->CurrentSize;

					if (startPos > size)
					{
						Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, startPos, size);
					}

					if (count == Math::Max<SizeT>() || startPos + count > size)
					{
						count = size - startPos;
					}

					return TBaseString(str + startPos, count);
				}

				///<summary>Зміна розміру буфера рядкової зміної</summary>
                ///<param name = "newSize">Новий розмір буфера без нуль-термінатора,
                ///<para> якщо менший ніж поточний розмір рядкової зміної то решту символів відкине</para></param>
                ///<returns>Силку на поточний рядок</returns>
                TBaseString& Resize(SizeT newSize) {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    newSize++;

                    if (This->str == nullptr)
                    {
                        This->str = static_cast<CharStr>(This->operator new[](sizeof(Char)* newSize));
                        This->str[0] = L'\0';
                        This->CurrentSize = 1;
                        This->ReservedSize = newSize;
                        return *This;
                    }     

                    if(This->CurrentSize > newSize) {
                        This->str[newSize - 1] = L'\0';
                        This->CurrentSize = newSize;
                    }

                    if(This->ReservedSize < newSize) {
                        CharStr buf = static_cast<CharStr>(This->operator new[](sizeof(Char)* newSize));
                        Memory::Copy(This->str, buf, This->CurrentSize * sizeof(Char));
                        This->ReservedSize = newSize;
                        This->operator delete[](This->str);
                        This->str = buf;
                    }
                    
                    return *This;
                }

                /// <summary> 
                /// Зміна розміру рядка з знищенням символів зліва 
                /// </summary>
                /// <param name = "newSize"> Новий розмір тексту без нуль-термінатора </param>
                /// <returns> Силку на поточний рядок </returns>
                TBaseString& LeftResize(SizeT newSize) {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    newSize++;

                    if (newSize > This->CurrentSize) {
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, newSize, This->CurrentSize);
                    }

                    auto delta = This->CurrentSize - newSize;

                    for (SizeT i = delta; i < This->CurrentSize; i++)
                    {
                        This->str[i - delta] = This->str[i];
                    }

                    This->str[newSize - 1] = L'\0';
                    This->CurrentSize = newSize;

                    return *This;
                }

                /// <summary>
                /// Резервування буферу рядка, якщо буфер вже достатній то нічого не відбудеться в іншому випадку відбудеться реалокація рядка
                /// </summary>
                /// <param name="newReservedSize">Новий розмір буфера рядка</param>
                /// <returns>Силка на поточнй об'єкт</returns>
                ForceInline TBaseString& Reserve(SizeT newReservedSize) {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    if (newReservedSize < This->ReservedSize)
                        return *This;

                    return This->Resize(newReservedSize);
                }

                /// <summary>Стиснення рядка до нового розміру</summary>
                /// <param name = "newSize">Новий фактичний розмір, якшо він більший ніж поточний то генерується помилка ThrowIndexOutOfRange</param>
                /// <exception cref="Exceptions::ThrowIndexOutOfRange">Новий фактичний розмір більший ніж поточний</exception>
                /// <returns>Силка на поточний рядок</returns>
                ForceInline TBaseString& Shrink(SizeT newSize) {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    if (newSize > This->CurrentSize)
                    {
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, newSize, This->CurrentSize);
                    }
                    This->CurrentSize = newSize;
                    This->str[newSize] = L'\0';
                    return *This;
                }

                /// <summary>
                /// Ефективно комбінує рядки в один
                /// </summary>
                /// <param name="strings">Масив рядків</param>
                /// <returns>Рядоку який є сумою масиву вхідних рядків</returns>
                ForceInline static TBaseString Combine(Containers::Array<TBaseString>&& strings) {
                    auto data = TBaseString(
                        strings.Aggregate<Memory::DataSizeT>(ArrowFunct(x, y, x + y.Size))
                    );

                    strings.ForEach([&](const auto& string) mutable InlineLambda{
                        data += string;
                    });

                    return data;
                }

                //Заміна по таблиці символів
                //Може згенерувати помилку IndexOutOfRange, якщо
                //- StartPos більша ніж кількість символів в рядку
                //- StartPos більша ніж EndPos
                TBaseString & Replace(
                    const Containers::BaseMap<THeap, Char, Char> & ReplaceTable,
                    SizeT StartPos = 0,
                    SizeT EndPos = Seed::Math::Max<SizeT>()
                ) {
                    TBaseString* This = static_cast<TBaseString*>(this);

                    if (StartPos > This->CurrentSize)
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, StartPos, This->CurrentSize);

                    if (StartPos > EndPos)
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, StartPos, EndPos);

                    if (EndPos > This->CurrentSize) 
                        EndPos = This->CurrentSize;

                    ReplaceTable.ForEach([&](const Containers::BasePair<THeap, Char, Char>& pair) {
                        for (SizeT i = StartPos; i < EndPos; i++) {
                            if (This->str[i] == pair.Key)
                                This->str[i] = pair.Value;
                        }
                        });
                    return *This;
                }

                //Заміна всіх входжень символа What на символ To
                //Може згенерувати помилку IndexOutOfRange, якщо
                //- StartPos більша ніж кількість символів в рядку
                //- StartPos більша ніж EndPos
                ///<summary>Заміна всіх входжень символа What на символ To</summary>
                ///<param name = "What">Символ який замінить</summary>
                ///<param name = "To">Символ на який замінить</summary>
                ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
                ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
                ///<returns>Силку на поточний рядок</returns>
                ///<exception cref="Seed::Exceptions::IndexOutOfRange">Якщо StartPos > CurrentSize або StartPos > EndPos</exception>
                TBaseString& Replace(
                    Char What,
                    Char To,
                    SizeT StartPos = 0,
                    SizeT EndPos = Seed::Math::Max<SizeT>()
                ) {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    if (StartPos > This->CurrentSize)
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, StartPos, This->CurrentSize);

                    if (StartPos > EndPos)
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, StartPos, EndPos);

                    if (EndPos > This->CurrentSize) 
                        EndPos = This->CurrentSize;

                    for (SizeT i = StartPos; i < EndPos; i++) {
                        if (This->str[i] == What)
                            This->str[i] = To;
                    }
                    return *This;
                }

                ///<summary>Приведення всіх букв в заглавні</summary>
                ///<returns>Силку на поточний рядок</retutns>
                ForceInline TBaseString& toUpcase() {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    for (SizeT i = 0; i < This->CurrentSize; i++)
                        This->str[i] = CStr::ToUpcase(This->str[i]);
                    return *This;
                }

                ///<summary>Приведення всіх букв в нижній регістр</summary>
                ///<returns>Силку на поточний рядок</retutns>
                ForceInline TBaseString& toDowncase() {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    for (SizeT i = 0; i < This->CurrentSize; i++)
                        This->str[i] = CStr::ToDowncase(This->str[i]);
                    return *This;
                }

                ///<summary>Розширює рядок до розміру NewSize
                ///<para>заповнюючи елементи після поточного розміру символами Filler</para></summary>
                ///<params name = "Filler">Символ заповнювач</params>
                ///<params name = "NewSize">Новий розмір рядка</params>
                ///<returns>Силку на поточний рядок після розширення</returns>
                ///<exception cref="Seed::Exceptions::IndexOutOfRange">NewSize менший ніж кількість символів в рядку</exception>
                TBaseString& Extend(Char Filler, SizeT NewSize) {
                    TBaseString* This = static_cast<TBaseString*>(this);

                    if (This->CurrentSize > NewSize)
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, NewSize, This->CurrentSize);

                    if (This->CurrentSize == NewSize)
                        return *This;

                    if (NewSize >= This->ReservedSize) {
                        SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                        CharStr buf = (CharStr)TBaseString::operator new[](NewSize * sizeof(Char));
                        Memory::Copy(This->str, buf, This->CurrentSize * sizeof(Char));
                        TBaseString::operator delete[](This->str);
                        This->str = buf;
                        This->ReservedSize = NewFullSize;
                    }

                    for (SizeT i = This->CurrentSize - 1; i < NewSize; i++)
                    {
                        This->str[i] = Filler;
                    }
                    This->str[NewSize] = L'\0';
                    This->CurrentSize = NewSize;

                    return *This;
                }

                ///<summary>Розділення рядка по символу Delim з позиції StartPos до EndPos</summary>
                ///<param name = "Delim">Символ розділювач, він не включається в вихідні фрагменти</param>
                ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
                ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
                ///<returns>Список підрядків</returns>
                Containers::BaseList<THeap, TBaseString> Split(const Char Delim, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if (This->str == nullptr)
                        return Containers::BaseList<THeap, TBaseString>();

                    if (This->CurrentSize == 0)
                        return Containers::BaseList<THeap, TBaseString>();

                    if (EndPos > This->Lenght) 
                        EndPos = This->Lenght;

                    Containers::BaseList<THeap, TBaseString> list;
                    SizeT StartPartPos = 0;
                    Boolean flag = false;

                    for (SizeT i = StartPos; i < EndPos; i++) {
                        if (flag == false && This->str[i] == Delim) {
                            list += TBaseString((const CharStr)(This->str + StartPartPos), i - StartPartPos);
                            flag = true;
                        }
                        if (flag == true && This->str[i] != Delim) {
                            StartPartPos = i;
                            flag = false;
                        }
                    }

                    if (flag == false)
                        list += TBaseString((const CharStr)(This->str + StartPartPos), EndPos - StartPartPos);

                    return list;
                }

                ///<summary>Розділення рядка по символу Delim з позиції StartPos до EndPos</summary>
                ///<param name = "Delim">Символ розділювач, він не включається в вихідні фрагменти</param>
                ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
                ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
                ///<returns>Список підрядків</returns>
                template<SizeT TCount>
                Containers::BaseList<THeap, TBaseString> Split(StaticTextRef<TCount> Delim, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if(TCount == 1)
                        return  Containers::BaseList<THeap, TBaseString>({ *This });

                    if (This->str == nullptr)
                        return Containers::BaseList<THeap, TBaseString>();

                    if (This->CurrentSize == 0)
                        return Containers::BaseList<THeap, TBaseString>();

                    if(This->CurrentSize <= TCount)
                        return Containers::BaseList<THeap, TBaseString>();

                    if (EndPos > This->Lenght) 
                        EndPos = This->Lenght;

                    Containers::BaseList<THeap, TBaseString> list;
                    SizeT CurrentPos = 0;
                    SizeT PreviosPos = StartPos;

                    do
                    {
                        CurrentPos = Find(Delim, PreviosPos);

                        if (CurrentPos > EndPos)
                            CurrentPos = EndPos;

                        if(CurrentPos != PreviosPos)
                            list += TBaseString((const CharStr)(This->str + PreviosPos), CurrentPos - PreviosPos);

                        PreviosPos = CurrentPos + (TCount - 1);
                    } while (!(PreviosPos >= EndPos));

                    return list;
                }

                ///<summary>Розділення рядка по символу Delim з позиції StartPos до EndPos</summary>
                ///<param name = "Delim">Символ розділювач, він не включається в вихідні фрагменти</param>
                ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
                ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
                ///<returns>Список підрядків</returns>
                Containers::BaseList<THeap, TBaseString> Split(const TBaseString Delim, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if(Delim.Lenght == 0)
                        return  Containers::BaseList<THeap, TBaseString>({ *This });

                    if (This->str == nullptr)
                        return Containers::BaseList<THeap, TBaseString>();

                    if (This->CurrentSize == 0)
                        return Containers::BaseList<THeap, TBaseString>();

                    if(This->CurrentSize <= Delim.Size)
                        return Containers::BaseList<THeap, TBaseString>();

                    if (EndPos > This->Lenght) 
                        EndPos = This->Lenght;

                    Containers::BaseList<THeap, TBaseString> list;
                    SizeT CurrentPos = 0;
                    SizeT PreviosPos = StartPos;

                    do
                    {
                        CurrentPos = Find(Delim, PreviosPos);

                        if (CurrentPos > EndPos)
                            CurrentPos = EndPos;

                        if(CurrentPos != PreviosPos)
                            list += TBaseString((const CharStr)(This->str + PreviosPos), CurrentPos - PreviosPos);

                        PreviosPos = CurrentPos + Delim.Lenght;
                    } while (!(PreviosPos >= EndPos));

                    return list;
                }

                ///<summary>Пошук символа з позиції StartPos до позиції EndPos</summary>
                ///<param name = "sym">Символ який шукатиме в рядку</param>
                ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
                ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
                ///<returns>Індекс першого входження або -1 якщо не знайдено</retutns>
                SizeT Find(Char sym, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if (This->CurrentSize == Math::Max<SizeT>()) 
                        return Math::Max<SizeT>();

                    if (StartPos == Math::Max<SizeT>()) 
                        StartPos = 0; 
                    else if (StartPos > This->Lenght) 
                        return Math::Max<SizeT>();

                    if (EndPos == Math::Max<SizeT>()) 
                        EndPos = This->CurrentSize;  
                    else if (EndPos < StartPos) 
                        return Math::Max<SizeT>();

                    for (SizeT i = StartPos; i < EndPos; i++)
                        if (This->str[i] == sym) return i;

                    return Math::Max<SizeT>();
                }

                ///<summary>Пошук символьного масиву з позиції StartPos до позиції EndPos</summary>
                ///<param name = "Text">Рядковий літерал який шукатиме</param>
                ///<param name = "Size">Кількість символів в рядковому літералі з нуль-термінатором</param>
                ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
                ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
                ///<returns>Індекс першого входження або -1 якщо не знайдено</returns>
                template<SizeT Size>
                SizeT Find(const Char(&Text)[Size], SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if (This->CurrentSize == Math::Max<SizeT>()) 
                        return Math::Max<SizeT>();

                    if (StartPos == Math::Max<SizeT>()) 
                        StartPos = 0; 
                    else if (StartPos > This->CurrentSize) 
                        return Math::Max<SizeT>();

                    if (EndPos == Math::Max<SizeT>()) 
                        EndPos = This->CurrentSize;  
                    else if (EndPos < StartPos) 
                        return Math::Max<SizeT>();

                    for (SizeT i = StartPos; i < EndPos - (Size - 1); i++) {
                        if (This->str[i] == Text[0]) {
                            Boolean Checked = true;
                            for (SizeT j = 1; j < Size - 1; j++)
                            {
                                if (This->str[i + j] != Text[j])
                                {
                                    Checked = false; break;
                                }
                            }
                            if (Checked) return i;
                        }
                    }
                    return Math::Max<SizeT>();
                }

                ///<summary>Пощук підрядка в даному рядку з позиції StartPos до позиції EndPos</summary>
                ///<param name = "Str">Рядкок який шукатиме</param>
                ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
                ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
                ///<returns>Індекс першого входження або -1 якщо не знайдено</returns>
                SizeT Find(const TBaseString& Str, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);

                    if (This->CurrentSize == Math::Max<SizeT>()) 
                        return Math::Max<SizeT>();

                    if (StartPos == Math::Max<SizeT>()) 
                        StartPos = 0; 
                    else if (StartPos > This->CurrentSize) 
                        return Math::Max<SizeT>();

                    if (EndPos == Math::Max<SizeT>()) 
                        EndPos = This->CurrentSize;  
                    else if (EndPos < StartPos) 
                        return Math::Max<SizeT>();

                    for (SizeT i = StartPos; i < EndPos - Str.Lenght; i++) {
                        if (This->str[i] == Str.str[0]) {
                            Boolean Checked = true;
                            for (SizeT j = 1; j < Str.Lenght; j++)
                            {
                                if (This->str[i + j] != Str.str[j])
                                {
                                    Checked = false; break;
                                }
                            }
                            if (Checked) return i;
                        }
                    }
                    return Math::Max<SizeT>();
                }
                
                /// <summary>
                /// Розширення рядка якщо він менший ніж вказаний розмір
                /// і заповнення вільного простору певним символом
                /// </summary>
                /// <param name="requiredSize">Розмір до якого треба розширити рядок</param>
                /// <param name="fillerCharacter">Символ заповнювач</param>
                /// <returns>Посилання на </returns>
                TBaseString& ResizeToFixed(SizeT requiredSize, Char fillerCharacter = L' ') {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    if (This->Lenght >= requiredSize)
                        return *This;

                    This->Reserve(requiredSize + CurrentSettings().MemoryManager.StringReserveSize);

                    for (SizeT i = This->Lenght; i < requiredSize; i++)
                    {
                        This->str[i] = fillerCharacter;
                    }
                    This->str[requiredSize] = L'\0';
                    This->CurrentSize = requiredSize + 1;

                    return *This;
                }

                /// <summary>
                /// Створює рядок заповнений вказаним символом вказану кількість раз
                /// </summary>
                /// <param name="requiredSize">Розмір який потрібно заповнити</param>
                /// <param name="fillerCharacter">Символ заповнювач</param>
                /// <returns>Заповнений рядок</returns>
                static TBaseString CreateFilledString(SizeT requiredSize, Char fillerCharacter = L' ') {
                    TBaseString str(Memory::DataSizeT(requiredSize * sizeof(Char)));
                    str.ResizeToFixed(requiredSize, fillerCharacter);
                    return str;
                }
			protected:
                ForceInline virtual ~TManipulationStringPart() = 0 {}
			};
		}
	}
}

#pragma warning(pop)