#pragma once
#include "Seed.Core.String.Manipulation.h"
#include "Seed.Core.String.LinqApi.h"
#include "Seed.Core.String.Convertors.h"
#include "Seed.Core.String.HexPart.h"

namespace Seed {
    inline namespace Core {
        inline namespace Exceptions {
            class IntegerOverflowException;
        }
#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)
        template<class THeap>
        class BaseString final :
            public Memory::Allocator<THeap>, 
            public IClonable<BaseString<THeap>>,
            public Details::TManipulationStringPart<THeap, BaseString>,
            public Details::TLinqApi<THeap, BaseString>,
            public Details::TConvertorsStringPart<THeap, BaseString>,
            public Details::THexStringPart<THeap, BaseString>
            {
        private:
            ///<summary>Покажчик на рядковий масив</summary>
            CharStr str;
            ///<summary>Поточна кількість символів</summary>
            SizeT CurrentSize;
            ///<summary>Розмір буфера</summary>
            SizeT ReservedSize;
            
            ///<summary>Конструктор переміщення для покажчика з розміром тексту і фактичним розміром масиву символів</summary>
            ///<params name = "rawPtr">Покажчик який буде використано як масив символів в нового рядка</params>
            ///<params name = "CurrentSize">Кількість символів з нуль-термінатором</params>
            ///<params name = "ReservedSize">Фактичний розмір масиву символів</params>
            ForceInline BaseString(CharStr&& rawPtr, SizeT CurrentSize, SizeT ReservedSize)
                :str(rawPtr), CurrentSize(CurrentSize), ReservedSize(ReservedSize) {
                rawPtr = nullptr;
            }
        public:
            ///<summary>Пустий конструктор</summary>
            constexpr ForceInline BaseString() : str(nullptr), CurrentSize(0), ReservedSize(0) {};

            ///<summary>Конструктор з готової літерала рядка</summary>
            ///<param name = "Size">Розмір рядка включно з нуль-термінатором</param>
            ///<param name = "Text">Літерал рядка</param>
            template<SizeT Size>
            ForceInline BaseString(StaticTextRef<Size> Text) : BaseString() {
                if constexpr (Size < 2)
                {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                }
                else {
                    CurrentSize = Size;
                    ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    Memory::Copy(Text, this->str, Size * sizeof(Char));
                }
            }

            ///<summary>Конструктор з готової літерала ASCII рядка</summary>
            ///<param name = "Size">Розмір рядка включно з нуль-термінатором</param>
            ///<param name = "Text">Літерал рядка</param>
            template<SizeT Size>
            inline BaseString(StaticATextRef<Size> Text) : CurrentSize(0), ReservedSize(0), str(nullptr) {
                if constexpr (!(Size < 2))
                {
                    CurrentSize = Size;
                    ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    CharStr temp_str = this->str;
                    ACharStr text = (ACharStr)Text;
                    {
                        while (*temp_str++ = *text++);
                    }
                    this->str[CurrentSize] = L'\0';
                }
            }

            ///<summary>Конструктор з готової константного рядка</summary>
            ///<param name = "Text">Літерал рядка</param>
            ForceInline BaseString(const ConstString& Text) {
                if (Text.Lenght < 2)
                {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                }
                else {
                    CurrentSize = Text.Lenght;
                    ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    Memory::Copy(Text.Str, this->str, Text.Lenght * sizeof(Char));
                }
            }

            ///<summary>Конструктор з готової константного ASCII рядка</summary>
            ///<param name = "str">Константний рядок</param>
            BaseString(const ConstAString& cstr) {
                if (cstr.Lenght < 2)
                {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                }
                else {
                    CurrentSize = cstr.Lenght;
                    ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    CharStr temp_str = this->str;
                    ACharStr text = (ACharStr)cstr.Str;
                    {
                        while (*temp_str++ = *text++);
                    }
                    this->str[CurrentSize] = L'\0';
                }
            }

            ///<summary>Конструктор з готового масиву букв який закінчується нуль-термінатором
            ///<para>Суто для внутрішнього використання</para></summary>
            ///<param name = "Text">Масив букв</param>
            ///<exception cref = "Seed::Exceptions::StringToLong">Генерує при завеликій кількості символів в масиві</exception>
            InternalApiCall BaseString(Char* text) {
                SizeT size = CStr::Size(text);
                if (size == 0) {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                    return;
                }
                CurrentSize = size + 1;
                if (CurrentSize > Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, CurrentSize);
                ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                for (SizeT i = 0; i < size; i++) {
                    this->str[i] = text[i];
                }
                this->str[size] = L'\0';
            }

            ///<summary>Конструктор з готового масиву букв який закінчується нуль-термінатором
            ///<para>Суто для внутрішнього використання</para></summary>
            ///<param name = "Text">Масив букв</param>
            ///<exception cref = "Seed::Exceptions::StringToLong">Генерує при завеликій кількості символів в масиві</exception>
            InternalApiCall BaseString(AChar* text) {
                SizeT size = CStr::Size(text);
                if (size == 0) {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                    return;
                }
                CurrentSize = size + 1;
                if (CurrentSize > Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, CurrentSize);
                ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                for (SizeT i = 0; i < size; i++) {
                    this->str[i] = text[i];
                }
                this->str[size] = L'\0';
            }

            ///<summary>Конструктор з готового масиву букв який закінчується нуль-термінатором
            ///<para>Суто для внутрішнього використання</para></summary>
            ///<param name = "Text">Масив букв</param>
            ///<param name = "Size">Кількість символів разом з нуль-термінатором</summary>
            ///<exception cref = "Seed::Exceptions::StringToLong">Генерує при завеликій кількості символів в масиві</exception>
            ForceInline InternalApiCall BaseString(const AChar* text, SizeT textSize) {
                if (textSize == 0) {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                    return;
                }
                CurrentSize = textSize + 1;
                if (CurrentSize > Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, CurrentSize);
                ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                for (SizeT i = 0; i < textSize; i++) {
                    this->str[i] = text[i];
                }
                this->str[textSize] = L'\0';
            }

            ///<summary>Конструктор з масиву символів і його розміру. Копіює вміст і дописую нуль-термінатор</summary>
            ///<params name = "rawText">Масив символів</params>
            ///<params name = "Size">Кількість символів</params>
            ForceInline InternalApiCall BaseString(const CharStr rawText, SizeT Size) :
                CurrentSize(Size + 1),
                ReservedSize(Size + 1 + CurrentSettings().MemoryManager.StringReserveSize),
                str((CharStr)BaseString::operator new[]((Size + 1 + CurrentSettings().MemoryManager.StringReserveSize) * sizeof(Char)))
            {
                Memory::Copy(rawText, this->str, Size * sizeof(Char));
                this->str[Size] = L'\0';
            }

            ///<summary>Конструктор з стартовим резервованим розміром</summary>
            ///<param name = "StartSize">Стартовий розмір в кількості символів без нуль-термінатора</param>
            ///<exception cref = "Seed::Exceptions::StringToLong">Генерує при завеликій кількості символів в масиві</exception>
            ForceInline explicit BaseString(Memory::DataSizeT StartSize) {
                if (StartSize.Bytes == Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, StartSize.Bytes);
                if (StartSize.Bytes == 0 || StartSize.Bytes < CurrentSettings().MemoryManager.StringReserveSize)
                    StartSize = Memory::DataSizeT(CurrentSettings().MemoryManager.StringReserveSize);
                this->ReservedSize = (StartSize.Bytes / 2) + 1;
                this->CurrentSize = 1;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * 2);
                this->str[0] = L'\0';
            }

            /// <summary>
            /// Конструктор з compile time рядка
            /// </summary>
            /// <param name="item">Compile time рядок</param>
            /// <typeparam name="N">Кільість символів</typeparam>
            template<SizeT N>
            ForceInline BaseString(const CTTI::Details::ConstexprArray<Char, N>& item) : CurrentSize(0), ReservedSize(0), str(nullptr) {
                if constexpr (N == 0)
                {
                    return;
                }
                else
                {
                    this->CurrentSize = N + 1;
                    for (SizeT i = 0; i < N; i++)
                    {
                        if (item[i] == '\0')
                        {
                            this->CurrentSize = i + 1; break;
                        }
                    }
                    
                    this->ReservedSize = this->CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr) BaseString::operator new[](ReservedSize * sizeof(Char));
                    Memory::Copy(item.Raw(), this->str, this->CurrentSize * sizeof(Char));
                    //for (SizeT i = 0; i < this->CurrentSize; i++)
                    //{
                    //    str[i] = item[i];
                    //}

                    this->str[this->CurrentSize - 1] = L'\0';
                }
            }

            ///<summary>Копіювальний конструктор</summary>
            ///<remarks>Виділить пам'ять лише при потребі</remarks>
            ///<param name = "from">Оригинал рядка</param>
            ForceInline BaseString(const BaseString& from) {
                if (from.str != nullptr) {
                    this->CurrentSize = from.CurrentSize;
                    this->ReservedSize = from.ReservedSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    Memory::Copy(from.str, this->str, from.CurrentSize * sizeof(Char));
                }
                else {
                    this->CurrentSize = this->ReservedSize = 0;
                    this->str = nullptr;
                }
            }
            ///<summary>Оптимізований конструктор</summary>
            ///<param name = "from">RValue силка на оригинал</param>
            ForceInline BaseString(BaseString&& from) {
                this->str = from.str; from.str = nullptr;
                this->CurrentSize = from.CurrentSize; from.CurrentSize = 0;
                this->ReservedSize = from.ReservedSize; from.ReservedSize = 0;
            }
#pragma region Comparing
#pragma region With Strings
            ///<summary>Оператор рівності</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            ForceInline Boolean operator==(const BaseString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            ForceInline Boolean operator!=(const BaseString& second) const {
                return !this->operator==(second);
            }
#pragma endregion
#pragma region With Char
            ///<summary>Оператор рівності з рядковим літералом</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            ForceInline Boolean operator==(StaticTextRef<Size> second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності з рядковим літералом</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<SizeT Size>
            ForceInline Boolean operator!=(StaticTextRef<Size> second) const {
                return !this->operator==<Size>(second);
            }

            ///<summary>Оператор рівності з рядковим літералом</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Рядкови літерад</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            ForceInline friend Boolean operator==(StaticTextRef<Size> second, const BaseString& first) {
                return first.operator==<Size>(second);
            }

            ///<summary>Оператор нерівності з рядковим літералом</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Рядкови літерад</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<SizeT Size>
            ForceInline friend Boolean operator!=(StaticTextRef<Size> second, const BaseString& first) {
                return first.operator!=<Size>(second);
            }
#pragma endregion
#pragma region With ASCII char
            ///<summary>Оператор рівності з рядковим літералом ASCII символів</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            ForceInline Boolean operator==(StaticATextRef<Size> second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності з рядковим літералом ASCII символів</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<SizeT Size>
            ForceInline Boolean operator!=(StaticATextRef<Size> second) const {
                return !this->operator==<Size>(second);
            }

            ///<summary>Оператор рівності з рядковим літералом ASCII символів</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Рядкови літерад</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            ForceInline friend Boolean operator==(StaticATextRef<Size> second, const BaseString& first) {
                return first.operator==<Size>(second);
            }

            ///<summary>Оператор нерівності з рядковим літералом ASCII символів</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Рядкови літерад</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<SizeT Size>
            ForceInline friend Boolean operator!=(StaticATextRef<Size> second, const BaseString& first) {
                return first.operator!=<Size>(second);
            }
#pragma endregion
#pragma region With ConstString
            ///<summary>Оператор рівності з константним рядком</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            ForceInline Boolean operator==(const ConstString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності з константним рядком</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            ForceInline Boolean operator!=(const ConstString& second) const {
                return !this->operator==(second);
            }

            ///<summary>Оператор рівності з константним рядком</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Константий рядок</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            ForceInline friend Boolean operator==(const ConstString& second, const BaseString& first) {
                return first.operator==(second);
            }

            ///<summary>Оператор нерівності з константним рядком</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Константий рядок</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            ForceInline friend Boolean operator!=(const ConstString& second, const BaseString& first) {
                return first.operator!=(second);
            }
#pragma endregion
#pragma region With ASCII ConstString
            ///<summary>Оператор рівності з константним рядком ASCII символів</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            ForceInline Boolean operator==(const ConstAString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності з константним рядком ASCII символів</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            ForceInline Boolean operator!=(const ConstAString& second) const {
                return !this->operator==(second);
            }

            ///<summary>Оператор рівності з константним рядком ASCII символів</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Константний рядок</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            ForceInline friend Boolean operator==(const ConstAString& second, const BaseString& first) {
                return first.operator==(second);
            }

            ///<summary>Оператор нерівності з константним рядком ASCII символів</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Константний рядок</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            ForceInline friend Boolean operator!=(const ConstAString& second, const BaseString& first) {
                return first.operator!=(second);
            }
#pragma endregion
#pragma region With String which have other allocator
            ///<summary>Оператор рівності з рядком алокованим в іншій кучі</summary>
            ///<param name = "OtherString">Константне посилання на рядкову зміну</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<typename OtherBase>
            ForceInline Boolean operator==(const BaseString<OtherBase>& OtherString) const {
                return (*this <=> OtherString) == 0;
            }

            template<typename OtherBase>
            ForceInline Boolean operator!=(const BaseString<OtherBase>& OtherString) const {
                return (*this <=> OtherString) != 0;
            }
#pragma endregion
#pragma endregion
#pragma region Copy operators
            ///<summary>Оператор присвоєння. Копіює текст в поточну рядкову зміну</summary>
            ///<param name = "orig">Оригинал тексту</param>
            ///<returns>Посилання на поточну зміну</returns>
            ///<remarks>Якщо текст вміщається в масив в поточній змінній то виділення пам'яті не відбувається.
            ///Текст просто копіюється по байтово. Якщо не вміщається то відбувається виділення пам'яті.</remarks>
            BaseString& operator=(const BaseString& orig) {
                if (this->str == orig.str) return *this;
                if (this->ReservedSize >= orig.CurrentSize) {
                    Memory::Copy(orig.str, this->str, orig.CurrentSize * sizeof(Char));
                    this->CurrentSize = orig.CurrentSize;
                }
                else {
                    BaseString::operator delete[](this->str);
                    this->str = nullptr;
                    this->str = (CharStr)BaseString::operator new[](sizeof(Char)* orig.ReservedSize);
                    Memory::Copy(orig.str, this->str, orig.CurrentSize * sizeof(Char));
                    this->CurrentSize = orig.CurrentSize;
                    this->ReservedSize = orig.ReservedSize;
                }
                return *this;
            }

            ///<summary>Оператор оптимізованого присвоєння</summary>
            ///<param name = "orig">RValue силка на оригинал</param>
            ///<returns>Посилання на поточну зміну</returns>
            ///<remarks>Викликається деструктор для поточного рядка.
            ///Копіються поля з оригинала. І накінець поля в оригинала обнулюються</remarks>
            ForceInline BaseString& operator=(BaseString&& orig) {
                if (orig.str == this->str) return *this;
                this->~BaseString();
                this->str = orig.str; orig.str = nullptr;
                this->CurrentSize = orig.CurrentSize; orig.CurrentSize = 0;
                this->ReservedSize = orig.ReservedSize; orig.ReservedSize = 0;
                return *this;
            }
#pragma endregion
#pragma region Comparing

            /// <summary>
            /// Універсальний оператор порівняння
            /// </summary>
            /// <param name="str1">Другий рядок</param>
            /// <returns>ПОрядок першого рядка відносно другого</returns>
            template<typename TOtherHeap>
            ForceInline StrongOrdering operator<=>(const BaseString<TOtherHeap>& right) const {
                auto res = Internals::CompareString(this->str, right.str, this->Lenght, right.Lenght);
                if (res == 0)
                    return StrongOrdering::equal;
                else if (res > 0)
                    return StrongOrdering::greater;
                else
                    return StrongOrdering::less;
            }

            /// <summary>
            /// Універсальний оператор порівняння
            /// </summary>
            /// <param name="str1">Другий рядок</param>
            /// <returns>ПОрядок першого рядка відносно другого</returns>
            template<> 
            ForceInline StrongOrdering operator<=>(const BaseString<THeap>& right) const {
                auto res = Internals::CompareString(this->str, right.str, this->Lenght, right.Lenght);
                if (res == 0)
                    return StrongOrdering::equal;
                else if (res > 0)
                    return StrongOrdering::greater;
                else
                    return StrongOrdering::less;
            }

            /// <summary>
            /// Універсальний оператор порівняння
            /// </summary>
            /// <param name="str1">Другий рядок</param>
            /// <returns>ПОрядок першого рядка відносно другого</returns>
            template<SizeT TextLenght>
            ForceInline StrongOrdering operator<=>(StaticTextRef<TextLenght> right) const {
                auto res = Internals::CompareString(this->str, (Char*)right, this->Lenght, TextLenght - 1);
                if (res == 0)
                    return StrongOrdering::equal;
                else if (res > 0)
                    return StrongOrdering::greater;
                else
                    return StrongOrdering::less;
            }

            /// <summary>
            /// Універсальний оператор порівняння
            /// </summary>
            /// <param name="str1">Другий рядок</param>
            /// <returns>ПОрядок першого рядка відносно другого</returns>
            ForceInline StrongOrdering operator<=>(const ConstString& right) const {
                auto res = Internals::CompareString(this->str, (Char*)right.Str, this->Lenght, right.GetLenght() - 1);
                if (res == 0)
                    return StrongOrdering::equal;
                else if (res > 0)
                    return StrongOrdering::greater;
                else
                    return StrongOrdering::less;
            }

            /// <summary>
            /// Універсальний оператор порівняння
            /// </summary>
            /// <param name="str1">Другий рядок</param>
            /// <returns>ПОрядок першого рядка відносно другого</returns>
            ForceInline StrongOrdering operator<=>(const ConstAString& right) const {
                auto countedSize = Min(this->Lenght, right.Lenght);

                for (SizeT i = 0; i < countedSize; i++)
                {
                    if (this->str[i] != right[i])
                    {
                        if (this->str[i] > right[i])
                            return StrongOrdering::greater;
                        else
                            return StrongOrdering::less;
                    }
                }

                return (this->Lenght <=> right.Lenght);
            }

            /// <summary>
            /// Універсальний оператор порівняння
            /// </summary>
            /// <param name="str1">Другий рядок</param>
            /// <returns>ПОрядок першого рядка відносно другого</returns>
            template<SizeT TextLenght>
            ForceInline StrongOrdering operator<=>(StaticATextRef<TextLenght> right) const {
                auto countedSize = Min(this->Lenght, TextLenght);

                for (SizeT i = 0; i < countedSize; i++)
                {
                    if (this->str[i] != right[i])
                    {
                        if (this->str[i] > right[i])
                            return StrongOrdering::greater;
                        else
                            return StrongOrdering::less;
                    }
                }

                return this->Lenght <=> TextLenght;
            }
#pragma endregion
#pragma region Add operators
#pragma region Literals
            ///<summary>Оператор додавання рядкового літералу</summary>
            ///<param name = "Text">Рядковий літерал</param>
            ///<param name = "Size">Кількість символів включно з нуль-термінатором</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            template<SizeT Size>
            ForceInline BaseString operator+(StaticTextRef<Size> Text) const {
                if constexpr (Size < 2)
                    return *this;
                BaseString retData;
                SizeT NewSize = this->Lenght + Size;
                SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* NewFullSize);
                if (this->str != nullptr) {
                    Memory::Copy(this->str, retData.str, this->Lenght * sizeof(Char));
                    Memory::Copy(Text, retData.str + this->Lenght, Size * sizeof(Char));
                }
                else {
                    Memory::Copy(Text, retData.str, Size * sizeof(Char));
                }
                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewFullSize;
                retData.str[retData.CurrentSize - 1] = L'\0';
                return retData;
            }

            ///<summary>Оператор додавання рядкового літералу</summary>
            ///<param name = "Text">Рядковий літерал</param>
            ///<param name = "Size">Кількість символів включно з нуль-термінатором</param>
            ///<param name = "Right">Правий елемент, він буде доданий до кінця лівого</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            template<SizeT Size>
            ForceInline friend BaseString operator+(StaticTextRef<Size> Text, const BaseString& Right) {
                if constexpr (Size < 2)
                    return Right;
                BaseString retData(Text);
                retData += Right;
                return retData;
            }

            ///<summary>Оператор додавання рядкового літерала ASCII символів</summary>
            ///<param name = "Text">Рядковий літерал</param>
            ///<param name = "Size">Кількість символів включно з нуль-термінатором</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            template<SizeT Size>
            BaseString operator+(StaticATextRef<Size> Text) const {
                if constexpr (Size < 2)
                    return *this;

                BaseString retData;
                SizeT NewSize = this->Lenght + Size;
                SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* NewFullSize); //-V2005

                if (this->str != nullptr)
                    Memory::Copy(this->str, retData.str, this->Lenght * sizeof(Char));
                CharStr tempStr = retData.str + this->CurrentSize;

                ACharStr second = (ACharStr)Text;
                {
                    while (*tempStr++ = *second++);
                }

                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewFullSize;
                return retData;
            }

            ///<summary>Оператор додавання рядкового літерала ASCII символів</summary>
            ///<param name = "Text">Рядковий літерал ASCII символів</param>
            ///<param name = "Size">Кількість символів включно з нуль-термінатором</param>
            ///<param name = "Right">Правий елемент, він буде доданий до кінця лівого</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            template<SizeT Size>
            ForceInline friend BaseString operator+(StaticATextRef<Size> Text, const BaseString& Right) {
                if constexpr (Size < 2)
                    return Right;
                BaseString retData(Text);
                retData += Right;
                return retData;
            }
#pragma endregion
#pragma region Const String
            ///<summary>Оператор додавання рядкового літералу</summary>
            ///<param name = "Text">Рядковий літерал</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            BaseString operator+(const ConstString& Text) const {
                if (Text.Lenght < 2)
                    return *this;

                BaseString retData;
                SizeT NewSize = this->Lenght + Text.Lenght;
                SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* NewFullSize);

                if (this->str != nullptr) {
                    Memory::Copy(this->str, retData.str, this->Lenght * sizeof(Char));
                    Memory::Copy(Text.Str, retData.str + this->Lenght, Text.Lenght * sizeof(Char));
                }
                else {
                    Memory::Copy(Text.Str, retData.str, Text.Lenght * sizeof(Char));
                }

                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewFullSize;
                retData.str[retData.CurrentSize - 1] = L'\0';
                return retData;
            }

            ///<summary>Оператор додавання рядкового літералу</summary>
            ///<param name = "Text">Рядковий літерал</param>
            ///<param name = "Right">Правий елемент, він буде доданий до кінця лівого</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            ForceInline friend BaseString operator+(const ConstString& Text, const BaseString& Right) {
                if (Text.Lenght < 2)
                    return Right;
                BaseString retData(Text);
                retData += Right;
                return retData;
            }

            ///<summary>Оператор додавання рядкового літерала ASCII символів</summary>
            ///<param name = "Text">Рядковий літерал</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            BaseString operator+(const ConstAString& Text) const {
                if (Text.Lenght < 2)
                    return *this;
                BaseString retData;
                SizeT NewSize = this->Lenght + Text.Lenght;
                SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* NewFullSize); //-V2005
                if (this->str != nullptr)
                    Memory::Copy(this->str, retData.str, (this->Lenght) * sizeof(Char));
                CharStr tempStr = retData.str + this->CurrentSize;
                ACharStr second = (ACharStr)Text.Str;
                {
                    while (*tempStr++ = *second++);
                }
                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewFullSize;
                return retData;
            }

            ///<summary>Оператор додавання рядкового літерала ASCII символів</summary>
            ///<param name = "Text">Рядковий літерал ASCII символів</param>
            ///<param name = "Right">Правий елемент, він буде доданий до кінця лівого</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            ForceInline friend BaseString operator+(const ConstAString& Text, const BaseString& Right) {
                if (Text.Lenght < 2)
                    return Right;
                BaseString retData(Text);
                retData += Right;
                return retData;
            }
#pragma endregion
            ///<summary>Оператор сумування двох рядків</summary>
            ///<param name = "second">Правий рядковий об'єкт</param>
            ///<returns>Суму рядкових зміних</returns>
            BaseString<THeap> operator+(const BaseString& second) const {
                if (this->CurrentSize == 0)
                    return second;
                SizeT NewSize = this->CurrentSize;
                if (second.CurrentSize != 0)
                    NewSize += second.CurrentSize - 1;
                else
                    return *this;

                SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                BaseString retData;
                CharStr buf = (CharStr)BaseString::operator new[](sizeof(Char)* NewFullSize); //-V2005
                if (this->str != nullptr) {
                    Memory::Copy(this->str, buf, this->Lenght * sizeof(Char));
                    Memory::Copy(second.str, buf + this->Lenght, second.CurrentSize * sizeof(Char));
                }
                else {
                    Memory::Copy(second.str, buf, second.CurrentSize * sizeof(Char));
                }

                retData.str = buf;
                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewFullSize;
                retData.str[retData.CurrentSize - 1] = L'\0';
                return retData;
            }

            ///<summary>Оператор сумування двох рядків з різними алокаторами</summary>
            ///<param name = "second">Правий рядковий об'єкт</param>
            ///<returns>Суму рядкових зміних з алокатором лівого елемента</returns>
            template<typename OtherBase>
            BaseString<THeap> operator+(const BaseString<OtherBase>& second) const {
                if (this->CurrentSize == 0)
                    return second;
                SizeT NewSize = this->CurrentSize;
                if (second.Lenght != 0)
                    NewSize += second.Lenght - 1;
                else
                    return *this;

                SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                BaseString retData;
                CharStr buf = (CharStr)BaseString<THeap>::operator new[](sizeof(Char)* NewFullSize); //-V2005
                if (this->str != nullptr) {
                    Memory::Copy(this->str, buf, (this->Lenght) * sizeof(Char));
                    Memory::Copy(second.operator const Seed::ConstCharStr(), buf + this->Lenght, second.Size.Bytes);
                }
                else {
                    Memory::Copy(second.operator const Seed::ConstCharStr(), buf, second.Lenght * sizeof(Char));
                }

                retData.str = buf;
                retData.CurrentSize = NewSize;
                retData.ReservedSize = NewFullSize;
                retData.str[retData.CurrentSize - 1] = L'\0';
                return retData;
            }

            ///<summary>Оператор контактенації масиву букв до поточного рядка</summary>
            ///<param name = "Text">Рядковий літерал який допишеться в кінець поточного рядка</param>
            ///<param name = "StrSize">Кількість символів в рядковому літералі</param>
            ///<returns>Силку на лівий аргумент який є сумою двох рядків</returns>
            ///<remarks>Якщо текст вміщається в масив в поточній змінній то виділення пам'яті не відбувається.
            ///Текст просто  дописуєтья по байтово. Якщо не вміщається то відбувається виділення пам'яті.</remarks>
            template<SizeT StrSize>
            BaseString<THeap>& operator+=(StaticTextRef<StrSize> Text) {
                SizeT NewSize = this->Lenght + StrSize;
                if (NewSize > ReservedSize) {
                    SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                    CharStr buf = (CharStr)BaseString<THeap>::operator new[](sizeof(Char)* NewFullSize); //-V2005
                    if (this->str != nullptr) {
                        Memory::Copy(this->str, buf, this->Lenght * sizeof(Char));
                        Memory::Copy(Text, buf + this->Lenght, StrSize * sizeof(Char));
                    }
                    else {
                        Memory::Copy(Text, buf, StrSize * sizeof(Char));
                    }
                    BaseString::operator delete[](this->str);
                    this->str = buf;
                    CurrentSize = NewSize;
                    this->str[CurrentSize - 1] = L'\0';
                    ReservedSize = NewFullSize;
                }
                else {
                    Memory::Copy(Text, this->str + this->Lenght, StrSize * sizeof(Char));
                    CurrentSize = NewSize;
                    this->str[CurrentSize - 1] = L'\0';
                }
                return *this;
            }

            ///<summary>Оператор контактенації символу до поточного рядка</summary>
            ///<param name = "Text">Символьний літерал який допишеться в кінець поточного рядка</param>
            ///<returns>Силку на лівий аргумент який є сумою двох рядків</returns>
            ///<remarks>Якщо текст вміщається в масив в поточній змінній то виділення пам'яті не відбувається.
            ///Символ просто  дописуєтья по байтово. Якщо не вміщається то відбувається виділення пам'яті.</remarks>
            BaseString<THeap>& operator+=(Char Text) {
                SizeT NewSize = this->Lenght + 2;
                if (NewSize > ReservedSize) {
                    SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                    CharStr buf = (CharStr)BaseString<THeap>::operator new[](sizeof(Char)* NewFullSize); //-V2005
                    if (this->str != nullptr) {
                        Memory::Copy(this->str, buf, this->Lenght * sizeof(Char));
                        buf[this->Lenght] = Text;
                    }
                    else {
                        buf[0] = Text;
                    }
                    BaseString::operator delete[](this->str);
                    this->str = buf;
                    CurrentSize = NewSize;
                    this->str[CurrentSize - 1] = L'\0';
                    ReservedSize = NewFullSize;
                }
                else {
                    this->str[this->Lenght] = Text;
                    CurrentSize = NewSize;
                    this->str[CurrentSize - 1] = L'\0';
                }
                return *this;
            }

            ///<summary>Оператор контактенації масиву букв до поточного рядка</summary>
            ///<param name = "Text">Рядковий літерал який допишеться в кінець поточного рядка</param>
            ///<param name = "StrSize">Кількість символів в рядковому літералі</param>
            ///<returns>Силку на лівий аргумент який є сумою двох рядків</returns>
            ///<remarks>Якщо текст вміщається в масив в поточній змінній то виділення пам'яті не відбувається.
            ///Текст просто  дописуєтья по байтово. Якщо не вміщається то відбувається виділення пам'яті.</remarks>
            template<SizeT StrSize>
            BaseString<THeap>& operator+=(StaticATextRef<StrSize> Text) {
                SizeT NewSize = this->Lenght + StrSize;
                if (NewSize > ReservedSize) {
                    SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                    CharStr buf = (CharStr)BaseString<THeap>::operator new[](sizeof(Char)* NewFullSize); //-V2005
                    if (this->str != nullptr) {
                        Memory::Copy(this->str, buf, this->Lenght * sizeof(Char));
                    }
                    CharStr tempStr = this->str + this->CurrentSize;
                    ACharStr second = (ACharStr)Text;
                    {
                        while (*tempStr++ = *second++);
                    }
                    BaseString::operator delete[](this->str);
                    this->str = buf;
                    CurrentSize += StrSize;
                    ReservedSize = NewFullSize;
                }
                else {
                    CharStr tempStr = this->str + this->Lenght;
                    ACharStr second = (ACharStr)Text;
                    {
                        while (*tempStr++ = *second++);
                    }
                    CurrentSize = NewSize;
                }
                return *this;
            }

            ///<summary>Оператор контактенації рядка до поточного рядка</summary>
            ///<param name = "second">Рядок який допише до поточного</param>
            ///<returns>Силку на лівий аргумент який є сумою двох рядків</returns>
            ///<remarks>Якщо текст вміщається в масив в поточній змінній то виділення пам'яті не відбувається.
            ///Текст просто  дописуєтья по байтово. Якщо не вміщається то відбувається виділення пам'яті.</remarks>
            BaseString& operator+=(const BaseString& second) {
                if (second.Lenght == 0)
                    return *this;

                SizeT NewSize = this->Lenght + second.Lenght + 1;
                if (NewSize < this->ReservedSize) {
                    Memory::Copy(second.str, this->str + this->Lenght, second.CurrentSize * sizeof(Char));
                    this->CurrentSize = NewSize;
                    this->str[this->CurrentSize - 1] = L'\0';
                    return *this;
                }
                else {
                    ReservedSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                    CharStr buf = (CharStr)BaseString::operator new[](sizeof(Char)* ReservedSize);
                    if (this->str != nullptr) {
                        Memory::Copy(this->str, buf, this->Lenght * sizeof(Char));
                        Memory::Copy(second.str, buf + this->Lenght, second.CurrentSize * sizeof(Char));
                    }
                    else {
                        Memory::Copy(second.str, buf, second.CurrentSize * sizeof(Char));
                    }
                    BaseString::operator delete[](this->str);
                    this->str = buf;
                    this->CurrentSize = NewSize;
                }
                return *this;
            }
            ///<summary>Оператор контактенації рядка до поточного рядка</summary>
            ///<param name = "orig">Рядок з іншим алокатором</summary>
            ///<param name = "OtherBase">Алокатор правого рядка</param>
            ///<returns>Силку на лівий аргумент який є сумою двох рядків</returns>
            ///<remarks>Якщо текст вміщається в масив в поточній змінній то виділення пам'яті не відбувається.
            ///Текст просто  дописуєтья по байтово. Якщо не вміщається то відбувається виділення пам'яті.</remarks>
            template<typename OtherBase>
            BaseString& operator+=(const BaseString<OtherBase>& orig) {
                if (orig.Lenght == 0)
                    return *this;

                SizeT NewSize = this->Lenght + orig.Lenght + 1;
                if (NewSize < this->ReservedSize) {
                    Memory::Copy(orig.operator const Seed::ConstCharStr(), this->str + this->Lenght, orig.Lenght * sizeof(Char));
                    this->CurrentSize = NewSize;
                    this->str[this->CurrentSize - 1] = L'\0';
                    return *this;
                }
                else {
                    ReservedSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;;
                    CharStr buf = (CharStr)BaseString::operator new[](sizeof(Char)* ReservedSize); //-V2005
                    if (this->str != nullptr) {
                        Memory::Copy(this->str, buf, (this->Lenght) * sizeof(Char));
                        Memory::Copy(orig.operator const Seed::ConstCharStr(), buf + this->Lenght, orig.Lenght * sizeof(Char));
                    }
                    else {
                        Memory::Copy(orig.operator const Seed::ConstCharStr(), buf, orig.Lenght * sizeof(Char));
                    }
                    BaseString::operator delete[](this->str); //-V2005
                    this->str = buf;
                    this->CurrentSize = NewSize;
                    this->str[this->CurrentSize - 1] = L'\0';
                }
                return *this;
            }

            ///<summary>Оператор доступу до символів рядка</summary>
            ///<param name = "Index">Індекс елемента з нуля до нуль-термінатора не включно</param>
            ///<returns>Силка на елемент рядка</returns>
            ForceInline Char& operator[](SizeT index) {
                if (index < CurrentSize)
                    return str[index];
                else
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, index, CurrentSize);
            }
            ///<summary>Констатний оператор доступу до символів рядка
            ///<param name = "Index">Індекс елемента з нуля до нуль-термінатора не включно</param>
            ///<returns>Константна силка на елемент рядка</returns>
            ForceInline const Char& operator[](SizeT index) const {
                if (index < CurrentSize)
                    return str[index];
                else
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, index, CurrentSize);
            }

            ///<summary>Вертає чи строка починається з строки ExpectedString</summary>
            ///<param name = "ExpectedString">Рядок з якого має починатись поточний рядок</param>
            ///<results><c>True</c> якщо рядок поточний рядок починається ExpectedString</results>
            ForceInline bool IsStartWith(const BaseString& ExpectedString) const {
                if (this->CurrentSize < ExpectedString.CurrentSize) return false;
                for (SizeT i = 0; i < ExpectedString.CurrentSize; i++)
                    if (this->str[i] != ExpectedString.str[i]) return false;
                return true;
            }

            ///<summary>Вертає чи строка починається з строки ExpectedString</summary>
            ///<param name = "ExpectedString">Рядок з якого має починатись поточний рядок</param>
            ///<results><c>True</c> якщо рядок поточний рядок починається ExpectedString</results>
            template<SizeT ExpectedStringSize>
            ForceInline bool IsStartWith(StaticTextRef<ExpectedStringSize> ExpectedString) const {
                if (this->CurrentSize < ExpectedStringSize) return false;
                for (SizeT i = 0; i < ExpectedStringSize - 1; i++) {
                    if (this->str[i] != ExpectedString[i]) return false;
                }
                return true;
            }

            ///<summary>Оператор конвертації до символьного масиву</summary>
            ///<returns>Константний рядок</returns>
            ForceInline explicit operator const ConstCharStr() const { return str; }

            ///<summary>Вертає поточну довжина рядку</summary>
            ForceInline SizeT GetLenght() const { return (CurrentSize > 0) ? CurrentSize - 1 : 0; }
            ///<summary>Поточна довжина рядка</summary>
            SizeT Property(GetLenght) Lenght;

            /// <summary>
            /// Вертає поточну довжину рядка з нуль-термінатором
            /// </summary>
            ForceInline SizeT GetFullLenght() const {
                return CurrentSize;
            }

            /// <summary>
            /// Поточна довжина рядка з нуль-термінатором
            /// </summary>
            SizeT Property(GetFullLenght) FullLenght;

            ///<summary>Вертає істину якщо рядок пустий</summary>
            ForceInline Boolean GetIsEmpty() const { return GetFullLenght() == 0; }
            ///<summary>Вертає істину якщо рядок пустий</summary>
            Boolean Property(GetIsEmpty) IsEmpty;

            /// <summary>
            /// Розмір даних
            /// </summary>
            /// <returns>Розмір даних</returns>
            Memory::DataSizeT GetSize() const { return Memory::DataSizeT(CurrentSize * sizeof(Char)); }
            /// <summary>
            /// Вертає розмір буфера рядка
            /// </summary>
            Memory::DataSizeT GetBufferSize() const { return Memory::DataSizeT(ReservedSize * sizeof(Char)); }

            /// <summary>
            /// Розмір буфера рядка
            /// </summary>
            Memory::DataSizeT Property(GetSize) Size;

            /// <summary>
            /// Розмір буфера
            /// </summary>
            Memory::DataSizeT Property(GetBufferSize) BufferSize;

            ///<summary>Конвертація в масив байт. Якщо рядок пустий верне пустий масив</summary>
            ///<returns>Масив байт який є копією вмісту рядка</returns>
            operator Containers::BaseArray<THeap, Byte>() const {
                if (this->str == nullptr || this->CurrentSize == 0)
                    return Containers::BaseArray<THeap, Byte>();
                CharStr data = (CharStr)BaseString::operator new[](this->CurrentSize * sizeof(Char));
                Memory::Copy(this->str, data, CurrentSize * sizeof(Char));
                return Containers::BaseArray<THeap, Byte>(MoveRValue(reinterpret_cast<Byte*>(data)), this->CurrentSize * sizeof(Char));
            }

            /// <summary>
            /// Провіря чи поточна рядкова змінна містить текст з заданої
            /// </summary>
            /// <param name="item">Задана змінна</param>
            /// <returns>true якщо тескт з item міститься в поточній змінній</returns>
            bool Containe(const BaseString& item) const {
                if (item.Lenght == this->Lenght && this->Lenght == 0)
                    return true;

                if (item.CurrentSize > this->CurrentSize)
                    return false;

                if (item.CurrentSize == this->CurrentSize)
                    return *this == item;

                auto searchSize = (this->CurrentSize - item.CurrentSize) + 1;
                for (SizeT i = 0; i < searchSize; i++)
                {
                    if (this->str[i] != item.str[0])
                    {
                        continue;
                    }

                    bool found = true;
                    for (SizeT j = 0; j < item.CurrentSize; j++)
                    {
                        if (this->str[i + j] != item.str[j])
                        {
                            found = false;
                            break;
                        }
                    }

                    if (found)
                        return true;
                }

                return false;
            }

            ///<summary>Конвертор для строк з різним алокатором</summary>
            ///<param name = "NewBase">Новий алокатор</summary>
            ///<returns>Копія рядка алокована алокатором NewBase</returns>
            template<typename NewBase>
            ForceInline operator BaseString<NewBase>() const {
                return BaseString<NewBase>((const CharStr)(ConstCharStr)*this);
            }

            ForceInline virtual BaseString Clone() const override {
                return BaseString(*this);
            }

            ///<summary>Оптимізований обмін рядками a і b</summary>
            ///<params name "a">Правий рядок</params>
            ///<params name "b">Лівий рядок</params>
            friend void Swap(BaseString& a, BaseString& b) {
                Seed::Swap(a.str, b.str);
                Seed::Swap(a.CurrentSize, b.CurrentSize);
                Seed::Swap(a.ReservedSize, b.ReservedSize);
            }

            ///<summary>Деструктор</summary>
            ForceInline virtual ~BaseString() override {
                if (str == nullptr) return;
                BaseString::operator delete[](str);
                str = nullptr;
                CurrentSize = ReservedSize = 0;
            }

            template<typename TOtherHeap>
            friend class BaseString;

            //String class parts

            template<typename THeap, template<typename THeap> typename TString>
            friend class Details::TManipulationStringPart;

            template<typename THeap, template<typename THeap> typename TString>
            friend class Details::TLinqApi;

            template<typename THeap, template<typename THeap> typename TString>
            friend class Details::TConvertorsStringPart;

            template<typename THeap, template<typename THeap> typename TString>
            friend class Details::THexStringPart;
        };

        ///<summary>Тип кінця рядка</summary>
        namespace { typedef const Char(&EndLineT)[3]; }
        ///<summary>Позначка переходу на новий рядок, використовується CRLF режим</summary>
        constexpr ForceInline EndLineT NextLine() {
            return L"\r\n";
        }

        ///<summary>Простий рядок</summary>
        namespace { using String = BaseString<Memory::Heap>; }
        ///<summary>Рядок для збереження конфіденційної інформації</summary>
        namespace { using SensitiveString = BaseString<Memory::SensitiveDataHeap>; }

        namespace TypeTraits {
            namespace Collection {
                template<typename THeap>
                struct IsStringT<BaseString<THeap>> : LogicTrue {};
            }
        }
    }
}

#pragma warning(disable: 4455)

ForceInline Seed::Core::String operator "" S(const wchar_t* const text, const Seed::SizeT size)
{ //-V2537
	using namespace Seed;
	return String((Char*)text); //-V2533
}
    
#pragma warning(pop)