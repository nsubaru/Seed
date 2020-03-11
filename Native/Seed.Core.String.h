#pragma once
#include "typeinfo"
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.IString.h"
#include "Seed.Core.Math.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.DateTime.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.NonConcurency.List.h"
#include "Seed.Core.Containers.NonConcurency.Map.h"
#include "Seed.Core.NewCTTI.TypeName.Details.h"

namespace Seed {
    inline namespace Core {
        inline namespace Exceptions {
            class IntegerOverflowException;
        }
#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)
        template<class THeap>
        class BaseString final :  //-V553
            public Memory::Allocator<THeap>, 
            public Containers::LINQ::IString<BaseString,THeap,UChar,false>
            {
        private:
            ///<summary>Покажчик на рядковий масив</summary>
            CharStr str;
            ///<summary>Поточна кількість символів</summary>
            SizeT CurrentSize;
            ///<summary>Розмір буфера</summary>
            SizeT ReservedSize;
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

            ///<summary>Конструктор переміщення для покажчика з розміром тексту і фактичним розміром масиву символів</summary>
            ///<params name = "rawPtr">Покажчик який буде використано як масив символів в нового рядка</params>
            ///<params name = "CurrentSize">Кількість символів з нуль-термінатором</params>
            ///<params name = "ReservedSize">Фактичний розмір масиву символів</params>
            BaseString(CharStr&& rawPtr, SizeT CurrentSize, SizeT ReservedSize)
                :str(rawPtr), CurrentSize(CurrentSize), ReservedSize(ReservedSize) {
                rawPtr = nullptr;
            }

        public:
            ///<summary>Масив символів шістнадцятирядкового числа</summary>
            static constexpr const auto HexNumbers = L"0123456789abcdef";
        public:
            ///<summary>Пустий конструктор</summary>
            __forceinline BaseString() noexcept : str(nullptr), CurrentSize(0), ReservedSize(0) {};

            ///<summary>Конструктор з готової літерала рядка</summary>
            ///<param name = "Size">Розмір рядка включно з нуль-термінатором</param>
            ///<param name = "Text">Літерал рядка</param>
            template<SizeT Size>
            __forceinline BaseString(StaticTextRef<Size> Text) : BaseString() {
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
            __forceinline BaseString(const ConstString& Text) {
                if (Text.Size < 2)
                {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                }
                else {
                    CurrentSize = Text.Size;
                    ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                    Memory::Copy(Text.Str, this->str, Text.Size * sizeof(Char));
                }
            }

            ///<summary>Конструктор з готової константного ASCII рядка</summary>
            ///<param name = "str">Константний рядок</param>
            BaseString(const ConstAString& cstr) {
                if (cstr.Size < 2)
                {
                    this->str = nullptr;
                    this->CurrentSize = this->ReservedSize = 0;
                }
                else {
                    CurrentSize = cstr.Size;
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
            InternalApiCall BaseString(const AChar* text, SizeT textSize) {
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
            InternalApiCall BaseString(const CharStr rawText, SizeT Size) :
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
            BaseString(SizeT StartSize) {
                if (StartSize > Math::Max<SizeT>())
                    Internals::Exceptions::ThrowStringToLong(__FILE__, __FUNCSIG__, __LINE__, StartSize);
                if (StartSize == 0 || StartSize < CurrentSettings().MemoryManager.StringReserveSize)
                    StartSize = CurrentSettings().MemoryManager.StringReserveSize;
                this->ReservedSize = StartSize + 1;
                this->CurrentSize = 1;
                this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                this->str[0] = L'\0';
            }

            /// <summary>
            /// Конструктор з compile time рядка
            /// </summary>
            /// <param name="item">Compile time рядок</param>
            /// <typeparam name="N">Кільість символів</typeparam>
            template<SizeT N>
            BaseString(const NewCTTI::Details::ConstexprArray<char, N>& item) : CurrentSize(0), ReservedSize(0), str(nullptr) {
                if constexpr (N == 0)
                {
	                static_assert(false, "ConstexprArray must have '\\0' in end");
                }
                else
                {
                    this->CurrentSize = N;
                    this->ReservedSize = this->CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                    this->str = (CharStr) BaseString::operator new[](ReservedSize * sizeof(Char));
                    for (SizeT i = 0; i < N; i++)
                    {
                        str[i] = item[i];
                    }
                }
            }

            ///<summary>Копіювальний конструктор</summary>
            ///<remarks>Виділить пам'ять лише при потребі</remarks>
            ///<param name = "from">Оригинал рядка</param>
            BaseString(const BaseString& from) {
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
            __forceinline BaseString(BaseString&& from) noexcept {
                this->str = from.str; from.str = nullptr;
                this->CurrentSize = from.CurrentSize; from.CurrentSize = 0;
                this->ReservedSize = from.ReservedSize; from.ReservedSize = 0;
            }
#pragma region Comparing
#pragma region With Strings
            ///<summary>Оператор рівності</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            __forceinline Boolean operator==(const BaseString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            __forceinline Boolean operator!=(const BaseString& second) const {
                return !this->operator==(second);
            }
#pragma endregion
#pragma region With Char
            ///<summary>Оператор рівності з рядковим літералом</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            __forceinline Boolean operator==(StaticTextRef<Size> second) const noexcept {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності з рядковим літералом</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<SizeT Size>
            __forceinline Boolean operator!=(StaticTextRef<Size> second) const noexcept {
                return !this->operator==<Size>(second);
            }

            ///<summary>Оператор рівності з рядковим літералом</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Рядкови літерад</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            __forceinline friend Boolean operator==(StaticTextRef<Size> second, const BaseString& first) {
                return first.operator==<Size>(second);
            }

            ///<summary>Оператор нерівності з рядковим літералом</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Рядкови літерад</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<SizeT Size>
            __forceinline friend Boolean operator!=(StaticTextRef<Size> second, const BaseString& first) {
                return first.operator!=<Size>(second);
            }
#pragma endregion
#pragma region With ASCII char
            ///<summary>Оператор рівності з рядковим літералом ASCII символів</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            __forceinline Boolean operator==(StaticATextRef<Size> second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності з рядковим літералом ASCII символів</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<SizeT Size>
            __forceinline Boolean operator!=(StaticATextRef<Size> second) const {
                return !this->operator==<Size>(second);
            }

            ///<summary>Оператор рівності з рядковим літералом ASCII символів</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Рядкови літерад</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            __forceinline friend Boolean operator==(StaticATextRef<Size> second, const BaseString& first) {
                return first.operator==<Size>(second);
            }

            ///<summary>Оператор нерівності з рядковим літералом ASCII символів</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Рядкови літерад</param>
            ///<param name = "Size">Кількість символів в літералі разом з нуль-термінатором</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<SizeT Size>
            __forceinline friend Boolean operator!=(StaticATextRef<Size> second, const BaseString& first) {
                return first.operator!=<Size>(second);
            }
#pragma endregion
#pragma region With ConstString
            ///<summary>Оператор рівності з константним рядком</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            __forceinline Boolean operator==(const ConstString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності з константним рядком</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            __forceinline Boolean operator!=(const ConstString& second) const {
                return !this->operator==(second);
            }

            ///<summary>Оператор рівності з константним рядком</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Константий рядок</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            __forceinline friend Boolean operator==(const ConstString& second, const BaseString& first) {
                return first.operator==(second);
            }

            ///<summary>Оператор нерівності з константним рядком</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Константий рядок</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            __forceinline friend Boolean operator!=(const ConstString& second, const BaseString& first) {
                return first.operator!=(second);
            }
#pragma endregion
#pragma region With ASCII ConstString
            ///<summary>Оператор рівності з константним рядком ASCII символів</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            template<SizeT Size>
            __forceinline Boolean operator==(const ConstAString& second) const {
                return (*this <=> second) == 0;
            }

            ///<summary>Оператор нерівності з константним рядком ASCII символів</summary>
            ///<param name = "second">Правий аргумент оператора</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            __forceinline Boolean operator!=(const ConstAString& second) const {
                return !this->operator==(second);
            }

            ///<summary>Оператор рівності з константним рядком ASCII символів</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Константний рядок</param>
            ///<returns><c>True</c> якщо рядки рівні</returns>
            __forceinline friend Boolean operator==(const ConstAString& second, const BaseString& first) {
                return first.operator==(second);
            }

            ///<summary>Оператор нерівності з константним рядком ASCII символів</summary>
            ///<param name = "first">Правий аргумент оператора</param>
            ///<param name = "second">Лівий аргумент оператора. Константний рядок</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            __forceinline friend Boolean operator!=(const ConstAString& second, const BaseString& first) {
                return first.operator!=(second);
            }
#pragma endregion
#pragma region With String which have other allocator
            ///<summary>Оператор рівності з рядком алокованим в іншій кучі</summary>
            ///<param name = "OtherString">Константне посилання на рядкову зміну</param>
            ///<returns><c>True</c> якщо рядки не рівні</returns>
            template<typename OtherBase>
            __forceinline Boolean operator==(const BaseString<OtherBase>& OtherString) const {
                return (*this <=> OtherString) == 0;
            }

            template<typename OtherBase>
            __forceinline Boolean operator!=(const BaseString<OtherBase>& OtherString) const {
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
            __forceinline BaseString& operator=(BaseString&& orig) noexcept {
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
            StrongOrdering operator<=>(const BaseString<TOtherHeap>& right) const {
                extern Seed::Int32 CompareString(Seed::CharStr left, Seed::CharStr right, Seed::SizeT leftSize, Seed::SizeT rightSize);
                auto res = CompareString(this->str, right.str, this->Lenght, right.Lenght);
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
            StrongOrdering operator<=>(const BaseString<THeap>& right) const {
                extern Seed::Int32 CompareString(Seed::CharStr left, Seed::CharStr right, Seed::SizeT leftSize, Seed::SizeT rightSize);
                auto res = CompareString(this->str, right.str, this->Lenght, right.Lenght);
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
            StrongOrdering operator<=>(StaticTextRef<TextLenght> right) const {
                extern Seed::Int32 CompareString(Seed::CharStr left, Seed::CharStr right, Seed::SizeT leftSize, Seed::SizeT rightSize);
                auto res = CompareString(this->str, (Char*)right, this->Lenght, TextLenght - 1);
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
            StrongOrdering operator<=>(const ConstString& right) const {
                extern Seed::Int32 CompareString(Seed::CharStr left, Seed::CharStr right, Seed::SizeT leftSize, Seed::SizeT rightSize);
                auto res = CompareString(this->str, (Char*)right.Str, this->Lenght, right.GetSize() - 1);
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
            StrongOrdering operator<=>(const ConstAString& right) const {
                auto countedSize = Min(this->Size, right.Size);

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

                return (this->Size <=> right.Size);
            }

            /// <summary>
            /// Універсальний оператор порівняння
            /// </summary>
            /// <param name="str1">Другий рядок</param>
            /// <returns>ПОрядок першого рядка відносно другого</returns>
            template<SizeT TextLenght>
            StrongOrdering operator<=>(StaticATextRef<TextLenght> right) const {
                auto countedSize = Min(this->Size, TextLenght);

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

                return this->Size <=> TextLenght;
            }
#pragma endregion
#pragma region Add operators
#pragma region Literals
            ///<summary>Оператор додавання рядкового літералу</summary>
            ///<param name = "Text">Рядковий літерал</param>
            ///<param name = "Size">Кількість символів включно з нуль-термінатором</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            template<SizeT Size>
            __forceinline BaseString operator+(StaticTextRef<Size> Text) const {
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
            __forceinline friend BaseString operator+(StaticTextRef<Size> Text, const BaseString& Right) {
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
            __forceinline friend BaseString operator+(StaticATextRef<Size> Text, const BaseString& Right) {
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
            __forceinline BaseString operator+(const ConstString& Text) const {
                if (Text.Size < 2)
                    return *this;

                BaseString retData;
                SizeT NewSize = this->Lenght + Text.Size;
                SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                retData.str = (CharStr)BaseString::operator new[](sizeof(Char)* NewFullSize);

                if (this->str != nullptr) {
                    Memory::Copy(this->str, retData.str, this->Lenght * sizeof(Char));
                    Memory::Copy(Text.Str, retData.str + this->Lenght, Text.Size * sizeof(Char));
                }
                else {
                    Memory::Copy(Text.Str, retData.str, Text.Size * sizeof(Char));
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
            __forceinline friend BaseString operator+(const ConstString& Text, const BaseString& Right) {
                if (Text.Size < 2)
                    return Right;
                BaseString retData(Text);
                retData += Right;
                return retData;
            }

            ///<summary>Оператор додавання рядкового літерала ASCII символів</summary>
            ///<param name = "Text">Рядковий літерал</param>
            ///<returns>Новий рядок який є сумою поточного і рядкового літерала</returns>
            BaseString operator+(const ConstAString& Text) const {
                if (Text.Size < 2)
                    return *this;
                BaseString retData;
                SizeT NewSize = this->Lenght + Text.Size;
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
            __forceinline friend BaseString operator+(const ConstAString& Text, const BaseString& Right) {
                if (Text.Size < 2)
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
                    Memory::Copy(second.operator const Seed::ConstCharStr(), buf + this->Lenght, (second.Size) * sizeof(Char));
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
            __forceinline Char& operator[](SizeT index) {
                if (index < CurrentSize)
                    return str[index];
                else
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, index, CurrentSize);
            }
            ///<summary>Констатний оператор доступу до символів рядка
            ///<param name = "Index">Індекс елемента з нуля до нуль-термінатора не включно</param>
            ///<returns>Константна силка на елемент рядка</returns>
            __forceinline const Char& operator[](SizeT index) const {
                if (index < CurrentSize)
                    return str[index];
                else
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, index, CurrentSize);
            }

            ///<summary>Вертає чи строка починається з строки ExpectedString</summary>
            ///<param name = "ExpectedString">Рядок з якого має починатись поточний рядок</param>
            ///<results><c>True</c> якщо рядок поточний рядок починається ExpectedString</results>
            __forceinline bool IsStartWith(const BaseString& ExpectedString) const {
                if (this->CurrentSize < ExpectedString.CurrentSize) return false;
                for (SizeT i = 0; i < ExpectedString.CurrentSize; i++)
                    if (this->str[i] != ExpectedString.str[i]) return false;
                return true;
            }

            ///<summary>Вертає чи строка починається з строки ExpectedString</summary>
            ///<param name = "ExpectedString">Рядок з якого має починатись поточний рядок</param>
            ///<results><c>True</c> якщо рядок поточний рядок починається ExpectedString</results>
            template<SizeT ExpectedStringSize>
            __forceinline bool IsStartWith(StaticTextRef<ExpectedStringSize> ExpectedString) const {
                if (this->CurrentSize < ExpectedStringSize) return false;
                for (SizeT i = 0; i < ExpectedStringSize - 1; i++) {
                    if (this->str[i] != ExpectedString[i]) return false;
                }
                return true;
            }

            ///<summary>Оператор конвертації до символьного масиву</summary>
            ///<returns>Константний рядок</returns>
            __forceinline explicit operator const ConstCharStr() const noexcept { return str; }
#pragma region Converting to string
#pragma region Converting int types to string
#pragma region Convertinh signed int types to string
            ///<summary>Конвертація цілого числа типу Int8 до рядка</summary>
            ///<returns>Вертає рядкове представлення числа</returns>
            static BaseString toString(Int8 val) {
                if (val == 0) return BaseString(L"0");
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
                BaseString str;
                str.CurrentSize = i;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                return str;
            }

            ///<summary>Конвертація цілого числа типу Int16 до рядка</summary>
            ///<returns>Вертає рядкове представлення числа</returns>
            static BaseString toString(Int16 val) {
                if (val == 0) return BaseString(L"0");
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
                BaseString str;
                str.CurrentSize = i;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                return str;
            }

            ///<summary>Конвертація цілого числа типу Int32 до рядка</summary>
            ///<returns>Вертає рядкове представлення числа</returns>
            static BaseString toString(Int32 val) {
                if (val == 0) return BaseString(L"0");
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
                BaseString str;
                str.CurrentSize = i;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                return str;
            }
            ///<summary>Конвертація цілого числа типу Int64 до рядка</summary>
            ///<returns>Вертає рядкове представлення числа</returns>
            static BaseString toString(Int64 val) {
                if (val == 0) return BaseString(L"0");
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
                BaseString str;
                str.CurrentSize = i;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                return str;
            }
#pragma endregion
#pragma region Convert unsigned int types to string
            ///<summary>Конвертація цілого числа типу UInt8 до рядка</summary>
            ///<returns>Вертає рядкове представлення числа</returns>
            static BaseString toString(UInt8 val) {
                if (val == 0) return BaseString(L"0");
                Char buf[5];
                SizeT i = 0;
                do {
                    buf[i] = L'0' + (val % 10);
                    i++;
                    val /= 10;
                } while (val != 0);
                buf[i] = L'\0';
                i++;
                BaseString str;
                str.CurrentSize = i;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                return str;
            }

            ///<summary>Конвертація цілого числа типу UInt16 до рядка</summary>
            ///<returns>Вертає рядкове представлення числа</returns>
            static BaseString toString(UInt16 val) {
                if (val == 0) return BaseString(L"0");
                Char buf[10];
                SizeT i = 0;
                do {
                    buf[i] = L'0' + (val % 10);
                    i++;
                    val /= 10;
                } while (val != 0);
                buf[i] = L'\0';
                i++;
                BaseString str;
                str.CurrentSize = i;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                return str;
            }

            ///<summary>Конвертація цілого числа типу UInt32 до рядка</summary>
            ///<returns>Вертає рядкове представлення числа</returns>
            static BaseString toString(UInt32 val) {
                if (val == 0) return BaseString(L"0");
                Char buf[15];
                SizeT i = 0;
                do {
                    buf[i] = L'0' + (val % 10);
                    i++;
                    val /= 10;
                } while (val != 0);
                buf[i] = L'\0';
                i++;
                BaseString str;
                str.CurrentSize = i;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                return str;
            }

            ///<summary>Конвертація цілого числа до рядка</summary>
            ///<returns>Вертає рядкове представлення числа</returns>
            static BaseString toString(UInt64 val) {
                if (val == 0) return BaseString(L"0");
                Char buf[30];
                SizeT i = 0;
                do {
                    buf[i] = L'0' + (val % 10);
                    i++;
                    val /= 10;
                } while (val != 0);
                buf[i] = L'\0';
                i++;
                BaseString str;
                str.CurrentSize = i;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, str.CurrentSize * sizeof(Char));
                Seed::Core::Reverse(str.str, str.CurrentSize - 1);
                return str;
            }
#pragma endregion
#pragma endregion
            ///<summary>Конвертація логічного значення до рядка вертає <c>"True"</c> або <c>"False"</c></summary>
            ///<params name = "val">Логічне значення</params>
            ///<returns><c>"True"</c> якщо <c>val == true</c> інакше <c>"False"</c></returns>
            __forceinline static BaseString toString(Boolean val) {
                if (val) return BaseString(L"True"); else return BaseString(L"False");
            }
            ///<summary>Конвертація логічного значення до рядка вертає вміст рядка <c>IfTrue</c> або <c>IfFalse</c></summary>
            ///<params name = "val">Логічне значення</params>
            ///<params name = "IfTrue">Рядок який верне якщо <c>val == true</c></params>
            ///<params name = "IfFalse">Рядок який верне якщо <c>val == false</c></params>
            ///<returns><c>IfTrue</c> або <c>IfFalse</c> залежно від значення <c>val</c></returns>
            __forceinline static BaseString toString(Boolean val, const BaseString& IfTrue, const BaseString& IfFalse) {
                if (val) return IfTrue; else return IfFalse;
            }
#pragma endregion
#pragma region Converting string to types
#pragma region Converting float typed to string
            ///<summary>Конвертація дійсного числа з одинарною точністю до рядка з певною кількістю знаків після коми</summary>
            ///<param name = "val">Число яке конвертуватиме</param>
            ///<param name = "precision">Кількість знаків після коми</param>
            ///<returns>Рядкове представлення числа</returns>
            static BaseString toString(Float val, UInt8 precision = 3) {
                using namespace Math;
                if (val == 0.0) return L"0"; //-V550
                if (val != val) return L"NaN";
                if (val == Infinity) return L"Infinity"; //-V550
                if (val == -Infinity) return L"-Infinity"; //-V550
                if (precision > 10) precision = 10;
                Boolean sign = Math::Sign(val);
                if (sign) val *= -1.0;
                Float IntPart;
                Float DoublePart = Math::ModD(val, IntPart);
                DoublePart *= Math::Pow(10, precision);
                Char buf[50];
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
                BaseString str;
                str.CurrentSize = size;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
                Memory::Copy(buf, str.str, (str.CurrentSize) * sizeof(Char));
                return str;
            }

            ///<summary>Конвертація дійсного числа з подвійною точністю до рядка з певною кількістю знаків після коми</summary>
            ///<param name = "val">Число яке конвертуватиме</param>
            ///<param name = "precision">Кількість знаків після коми</param>
            ///<returns>Рядкове представлення числа</returns>
            static BaseString toString(Double val, UInt8 precision = 5) {
                using namespace Math;
                if (val == 0.0) return L"0"; //-V550
                if (val != val) return L"NaN";
                if (val == Infinity) return L"Infinity"; //-V550
                if (val == -Infinity) return L"-Infinity"; //-V550
                if (precision > 10) precision = 10;
                Boolean sign = Math::Sign(val);
                if (sign) val *= -1.0;
                Double IntPart;
                Double DoublePart = Math::ModD(val, IntPart);
                DoublePart *= Math::Pow(10, precision);
                Char buf[50];
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
                BaseString str;
                str.CurrentSize = size;
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
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
                if (this->CurrentSize == 0 || this->Lenght == 0)
                    Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(this), 0);
                if (this->CurrentSize > Math::MaxNumberCount<T>())
                    throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);

                if constexpr (TypeTraits::IsSameV<TypeTraits::Unsigned<T>, T>) {
                    Math::SafeInt<T> ret;
                    for (SizeT i = 0; i < Lenght; i++) {
                        if (!(L'0' <= str[i] && str[i] <= L'9'))
                            Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                        ret += static_cast<UInt8>(str[i] - L'0');
                        if (i < CurrentSize - 2)
                            ret *= 10;
                    }
                    return ret.Value;
                }
                else {
                    Boolean sign(this->str[0] == '-');
                    SizeT i = 0;
                    if (str[0] == L'-' || str[0] == L'+')
                        i = 1;
                    Math::SafeInt<T> ret = 0;
                    for (i; i < Lenght; i++) {
                        if (!(L'0' <= str[i] && str[i] <= L'9'))
                            Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                        ret += str[i] - L'0';
                        if (i < CurrentSize - 2)
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
            Float toFloat() {
                 if (this->CurrentSize == 0)
                    Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(this), 0);
                if (this->CurrentSize > 18)
                    Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
                Boolean sign(this->str[0] == '-');
                size_t i = 0;
                if (str[0] == L'-' || str[0] == L'+')
                    i = 1;
                Float int_part = 0.0, double_part = 0.0;
                for (i; i < Lenght && this->str[i] != L'.'; i++) {
                    if (!(L'0' <= str[i] && str[i] <= L'9'))
                        Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                    int_part += this->str[i] - L'0';
                    int_part *= 10;
                }
                int_part /= 10;
                if (i != CurrentSize) {
                    for (SizeT j = Lenght - 1; j > i; j--)
                    {
                        if (!(L'0' <= str[j] && str[j] <= L'9'))
                            Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                        double_part += this->str[j] - L'0';
                        double_part /= 10;
                    }
                    int_part += double_part;
                }
                return int_part * ((sign) ? -1.0 : 1.0);
            }

            ///<summary>Перетворення тексту в дійсне число Double. На початку може стояти символ '+' або '-'</summary>
            ///<returns>Числове представлення</returns>
            ///<exception cref="Seed::Exceptions::NullPointException">якщо рядок пустий</exception>
            ///<exception cref="Seed::Exceptions::IntegerOverflowException">якщо кількість цифр є більшою ніж можу вмістити число Double</exception>
            ///<exception cref="Seed::Exceptions::InvalidFormatException">якщо в строці є відміні від цифр і одної крапки символи</exception>
            Double toDouble() {
                if (this->CurrentSize == 0)
                    Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(this), 0);
                if (this->CurrentSize > 18)
                    Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
                Boolean sign(this->str[0] == '-');
                size_t i = 0;
                if (str[0] == L'-' || str[0] == L'+')
                    i = 1;
                Double int_part = 0.0, double_part = 0.0;
                for (i; i < Lenght && this->str[i] != L'.'; i++) {
                    if (!(L'0' <= str[i] && str[i] <= L'9'))
                        Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                    int_part += this->str[i] - L'0';
                    int_part *= 10;
                }
                int_part /= 10;
                if (i != CurrentSize) {
                    for (SizeT j = Lenght - 1; j > i; j--)
                    {
                        if (!(L'0' <= str[j] && str[j] <= L'9'))
                            Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                        double_part += this->str[j] - L'0';
                        double_part /= 10;
                    }
                    int_part += double_part;
                }
                return int_part * ((sign) ? -1.0 : 1.0);
            }
#pragma endregion
            ///<summary>Вертає рядкове представлення UUID</summary>
            ///<returns>Рядкове представлення UUID в вигляді {????????-????-????-????????????????}</returns>
            static BaseString<THeap> toString(const UUID& id) {
                auto ReservedSize = sizeof(UUID::UUIDStringWildCard) + CurrentSettings().MemoryManager.StringReserveSize;
                CharStr buffer = reinterpret_cast<CharStr>(BaseString<THeap>::operator new[](sizeof(UUID::UUIDStringWildCard)));
                extern void ConvertUUIDtoCharStr(const Seed::Core::UUID & id, Seed::CharStr buffer);
                ConvertUUIDtoCharStr(id, buffer);
                return BaseString<THeap>(MoveRValue(buffer), sizeof(UUID::UUIDStringWildCard), ReservedSize);
            }

            ///<summary>Перетворення дати і часу в строкове представлення</summary>
            static BaseString<THeap> toString(Seed::Core::Date date) {
                Char buffer[22];
                extern void ConvertDateToStringImpl(Seed::Core::Date date, Seed::Core::CharStr buffer);
                ConvertDateToStringImpl(date, buffer);
                return buffer;
            }

            ///<summary>Перетворення часу і дати в строкове представлення</summary>
            static BaseString<THeap> toString(Seed::Core::DateTime time) {
                Char buffer[22];
                extern void ConvertDateTimeToStringImpl(const Seed::Core::DateTime & time, Seed::CharStr buffer);
                ConvertDateTimeToStringImpl(time, buffer);
                return buffer;
            }

            /// <summary>
            /// Перетворює в рядок опис CxxRtti опис типу
            /// </summary>
            /// <param name="typeInfo">CxxRtti опис типу</param>
            /// <returns>Кодована назва типу</returns>
            static BaseString<THeap> toString(const TypeInfo& typeInfo) {
                return BaseString<THeap>(const_cast<AChar*>(typeInfo.GetCxxName()));
            }

            ///<summary>Приведення бінарного тексту до рядкової зміної</summary>
            ///<params name = "array">Масив букв</params>
            ///<returns>Рядок з текстом з масиву</returns>
            InternalApiCall static BaseString<THeap> toString(const Containers::BaseArray<THeap, Seed::Byte>& array) {
                BaseString<THeap> str;
                str.CurrentSize = array.Count / sizeof(Char);
                str.ReservedSize = str.CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                str.str = (CharStr)BaseString<THeap>::operator new[](str.ReservedSize * sizeof(Char));//new Char[str.CurrentSize + 1];
                Memory::Copy(array.GetRaw(), str.str, array.Count);
                str.str[str.CurrentSize] = L'\0';
                return str;
            }
#pragma endregion

            ///<summary>Конвертація з рядкового в бінарне представлення UUID </summary>
            ///<returns>UUID</returns>
            UUID toUUID() {
                if (this->str == nullptr || this->CurrentSize == 0)
                    Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(this), 0);
                UUID res;
                extern bool ConvertCharStrToUUID(Seed::Core::CharStr str, Seed::Core::UUID & uuid);
                if (!ConvertCharStrToUUID(this->str, res)) {
                    Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                }
                return res;
            }

            ///<summary>Вертає поточну довжина рядку</summary>
            SizeT GetLenght() const { return (CurrentSize > 0) ? CurrentSize - 1 : 0; }
            ///<summary>Поточна довжина рядка</summary>
            SizeT Property(GetLenght) Lenght;

            /// <summary>
            /// Вертає поточну довжину рядка з нуль-термінатором
            /// </summary>
            SizeT GetSize() const {
                return CurrentSize;
            }

            /// <summary>
            /// Поточна довжина рядка з нуль-термінатором
            /// </summary>
            SizeT Property(GetSize) Size;

            /// <summary>
            /// Вертає розмір буфера рядка
            /// </summary>
            SizeT GetBufferSize() const {
                return ReservedSize;
            }

            /// <summary>
            /// Розмір буфера рядка
            /// </summary>
            SizeT Property(GetBufferSize) BufferSize;

            ///<summary>Вертає істину якщо рядок пустий</summary>
            Boolean GetIsEmpty() const { return GetSize() == 0; }
            ///<summary>Вертає істину якщо рядок пустий</summary>
            Boolean Property(GetIsEmpty) IsEmpty;

            ///<summary>Зміна розміру буфера рядкової зміної</summary>
            ///<param name = "newSize">Новий розмір буфера без нуль-термінатора,
            ///<para> якщо менший ніж поточний розмір рядкової зміної то решту символів відкине</para></param>
            ///<returns>Силку на поточний рядок</returns>
            BaseString& Resize(SizeT newSize) {
                if (this->str == nullptr)
                {
                    this->str = static_cast<CharStr>(this->operator new[](sizeof(Char)* newSize));
                    this->str[0] = L'\0';
                    this->CurrentSize = 1;
                    this->ReservedSize = newSize;
                    return *this;
                }
                if (CurrentSize < newSize)
                    str[newSize] = L'\0';
                newSize++;
                CharStr buf = static_cast<CharStr>(this->operator new[](sizeof(Char)* newSize));
                if (CurrentSize > newSize) {
                    Memory::Copy(str, buf, newSize * sizeof(Char));
                    ReservedSize = CurrentSize = newSize;
                }
                else {
                    Memory::Copy(str, buf, CurrentSize * sizeof(Char));
                    ReservedSize = newSize;
                }
                this->operator delete[](str);
                this->str = buf;
                return *this;
            }

            /// <summary>
            /// Резервування буферу рядка, якщо буфер вже достатній то нічого не відбудеться в іншому випадку відбудеться реалокація рядка
            /// </summary>
            /// <param name="newReservedSize">Новий розмір буфера рядка</param>
            /// <returns>Силка на поточнй об'єкт</returns>
            BaseString& Reserve(SizeT newReservedSize) {
                if (newReservedSize < ReservedSize)
                    return *this;

                return Resize(newReservedSize);
            }

            /// <summary>Стиснення рядка до нового розміру</summary>
            /// <param name = "newSize">Новий фактичний розмір, якшо він більший ніж поточний то генерується помилка ThrowIndexOutOfRange</param>
            /// <exception cref="Exceptions::ThrowIndexOutOfRange">Новий фактичний розмір більший ніж поточний</exception>
            /// <returns>Силка на поточний рядок</returns>
            BaseString& Shrink(SizeT newSize) {
                if (newSize > CurrentSize)
                {
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, newSize, this->CurrentSize);
                }
                CurrentSize = newSize;
                str[newSize] = L'\0';
                return *this;
            }

            ///<summary>Пошук символа з позиції StartPos до позиції EndPos</summary>
            ///<param name = "sym">Символ який шукатиме в рядку</param>
            ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
            ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
            ///<returns>Індекс першого входження або -1 якщо не знайдено</retutns>
            SizeT Find(Char sym, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                if (CurrentSize == Math::Max<SizeT>()) 
                    return Math::Max<SizeT>();

                if (StartPos == Math::Max<SizeT>()) 
                    StartPos = 0; 
                else if (StartPos > Lenght) 
                    return Math::Max<SizeT>();

                if (EndPos == Math::Max<SizeT>()) 
                    EndPos = CurrentSize;  
                else if (EndPos < StartPos) 
                    return Math::Max<SizeT>();

                for (SizeT i = StartPos; i < EndPos; i++)
                    if (str[i] == sym) return i;
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
                if (CurrentSize == Math::Max<SizeT>()) 
                    return Math::Max<SizeT>();

                if (StartPos == Math::Max<SizeT>()) 
                    StartPos = 0; 
                else if (StartPos > CurrentSize) 
                    return Math::Max<SizeT>();

                if (EndPos == Math::Max<SizeT>()) 
                    EndPos = CurrentSize;  
                else if (EndPos < StartPos) 
                    return Math::Max<SizeT>();

                for (SizeT i = StartPos; i < EndPos - Size; i++) {
                    if (str[i] == Text[0]) {
                        Boolean Checked = true;
                        for (SizeT j = 1; j < Size - 1; j++)
                            if (str[i + j] != Text[j]) {
                                Checked = false; break;
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
            SizeT Find(const BaseString& Str, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                if (CurrentSize == Math::Max<SizeT>()) 
                    return Math::Max<SizeT>();

                if (StartPos == Math::Max<SizeT>()) 
                    StartPos = 0; 
                else if (StartPos > CurrentSize) 
                    return Math::Max<SizeT>();

                if (EndPos == Math::Max<SizeT>()) 
                    EndPos = CurrentSize;  
                else if (EndPos < StartPos) 
                    return Math::Max<SizeT>();

                for (SizeT i = StartPos; i < EndPos - Str.CurrentSize; i++) {
                    if (str[i] == Str.str[0]) {
                        Boolean Checked = true;
                        for (SizeT j = 1; j < Str.CurrentSize; j++)
                            if (str[i + j] != Str.str[j]) {
                                Checked = false; break;
                            }
                        if (Checked) return i;
                    }
                }
                return Math::Max<SizeT>();
            }

            ///<summary>Розділення рядка по символу Delim з позиції StartPos до EndPos</summary>
            ///<param name = "Delim">Символ розділювач, він не включається в вихідні фрагменти</param>
            ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
            ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
            ///<returns>Список підрядків</returns>
            Containers::BaseList<THeap, BaseString> Split(const Char Delim, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                if (this->str == nullptr)
                    return Containers::BaseList<THeap, BaseString>();

                if (this->CurrentSize == 0)
                    return Containers::BaseList<THeap, BaseString>();

                if (EndPos == Math::Max<SizeT>()) 
                    EndPos = this->CurrentSize;

                Containers::BaseList<THeap, BaseString> list;
                SizeT StartPartPos = 0;
                Boolean flag = false;

                for (SizeT i = StartPos; i < EndPos; i++) {
                    if (this->str[i] == Delim && flag == false) {
                        list += BaseString((const CharStr)(this->str + StartPartPos), i - StartPartPos);
                        flag = true;
                    }
                    if (this->str[i] != Delim && flag == true) {
                        StartPartPos = i;
                        flag = false;
                    }
                }
                if (this->str[EndPos - 1] != Delim)
                    list += BaseString((const CharStr)(this->str + StartPartPos), (EndPos - StartPartPos) - 1);
                return list;
            }

            ///<summary>Розділення рядка по символу Delim з позиції StartPos до EndPos</summary>
            ///<param name = "Delim">Символ розділювач, він не включається в вихідні фрагменти</param>
            ///<param name = "StartPos">Індекс початкової позиції. Не обов'язковий параметр. Якщо не вказано то від початку</param>
            ///<param name = "EndPos">Індекс кінцевої позиції. Не обов'язковий параметрю. Якщо не вказано то до кінця рядка</param>
            ///<returns>Список підрядків</returns>
            template<SizeT TCount>
            Containers::BaseList<THeap, BaseString> Split(StaticTextRef<TCount> Delim, SizeT StartPos = 0, SizeT EndPos = Math::Max<SizeT>()) const {
                if (this->str == nullptr)
                    return Containers::BaseList<THeap, BaseString>();

                if (this->CurrentSize == 0)
                    return Containers::BaseList<THeap, BaseString>();

                if(this->CurrentSize <= TCount)
                    return Containers::BaseList<THeap, BaseString>();

                if (EndPos == Math::Max<SizeT>()) 
                    EndPos = this->CurrentSize;

                Containers::BaseList<THeap, BaseString> list;
                SizeT CurrentPos = 0;
                SizeT PreviosPos = StartPos;

                do
                {
                    CurrentPos = Find(Delim, PreviosPos);
                    if (CurrentPos == Math::Max<SizeT>())
                        break;
                    list += BaseString((const CharStr)(this->str + PreviosPos), CurrentPos - PreviosPos);
                    PreviosPos = CurrentPos + (TCount - 1);
                } while (CurrentPos != Math::Max<SizeT>());
                list += BaseString((const CharStr)(this->str + PreviosPos), this->Lenght - PreviosPos);
                return list;
            }

            ///<summary>Перетворення байтового масиву в строку з 16 бітних символів (0-9a-f)</summary>
            ///<param name = "binaryData">Масив байтів</param>
            ///<returns>Рядкове представлення бінарних даних</returns>
            static BaseString<THeap> binaryToHex(const Containers::BaseArray<THeap, Byte>& binaryData) {
                //Bite <-> 2 hex digits
                union {
                    //1 Byte (8 bits)
                    const Byte* binary;
                    //2 Hex digits (4x2 bits)
                    const struct {
                        //Secon hex (4 bits)
                        Byte bit1 : 4;
                        //First hex (4 bits)
                        Byte bit0 : 4;
                    } *Hex;
                } conv;
                if (binaryData.Count % 2 != 0)
                    Internals::Exceptions::ThrowInvalidFormatException(__FILE__, __FUNCSIG__, __LINE__);
                auto CurrentSize = binaryData.Count * 2 + 1;
                auto ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                CharStr ret = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                SizeT i1 = 0, i2 = 0;
                ret[(binaryData.Count * 2) - 1] = L'\0';
                ret[binaryData.Count * 2] = L'\0';
                for (i1; i1 < binaryData.Count; i1++) {
                    conv.binary = reinterpret_cast<const Byte*>(&binaryData[i1]);
                    ret[i2] = HexNumbers[conv.Hex->bit0];
                    i2++;
                    ret[i2] = HexNumbers[conv.Hex->bit1];
                    i2++;
                }
                BaseString str(MoveRValue(ret), CurrentSize, ReservedSize);
                return str;
            }

            ///<summary>Шістнадцятиричне представлення UInt64</summary>
            ///<param name = "Value">Вхідне значення</param>
            ///<returns>Рядкове представлення бінарних даних</returns>
            static BaseString<THeap> UInt64asHex(UInt64 Value) {
                union ConvertT {
                    SizeT Variable;
                    struct {
                        Byte LowPart : 4;
                        Byte HighPart : 4;
                    }
                    Code[sizeof(SizeT)];
                } Converter;
                Converter.Variable = Value;
                constexpr auto CurrentSize = (sizeof(SizeT) * 2) + 1;
                auto ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                CharStr ret = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                ret[CurrentSize] = L'\0';
                
                Int32 len = CurrentSize - 1;
                for (--len; len >= 0; Value >>= 4, --len) {
                    ret[len] = HexNumbers[Value & 0xf];
                }

                BaseString str(MoveRValue(ret), CurrentSize, ReservedSize);
                return str;
            }

            ///<summary>Шістнадцятиричне представлення UInt64</summary>
            ///<param name = "Value">Вхідне значення</param>
            ///<returns>Рядкове представлення бінарних даних</returns>
            static BaseString<THeap> Int32asHex(Int32 Value) {
                union ConvertT {
                    SizeT Variable;
                    struct {
                        Byte LowPart : 4;
                        Byte HighPart : 4;
                    }
                    Code[sizeof(Int32)];
                } Converter;
                Converter.Variable = Value;
                constexpr auto CurrentSize = (sizeof(Int32) * 2) + 1;
                auto ReservedSize = CurrentSize + CurrentSettings().MemoryManager.StringReserveSize;
                CharStr ret = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
                ret[CurrentSize] = L'\0';

                Int32 len = CurrentSize - 1;
                for (--len; len >= 0; Value >>= 4, --len) {
                    ret[len] = HexNumbers[Value & 0xf];
                }

                BaseString str(MoveRValue(ret), CurrentSize, ReservedSize);
                return str;
            }

            ///<summary>Конвертація в масив байт. Якщо рядок пустий верне пустий масив</summary>
            ///<returns>Масив байт який є копією вмісту рядка</returns>
            operator Containers::BaseArray<THeap, Byte>() const {
                if (this->str == nullptr || this->CurrentSize == 0)
                    return Containers::BaseArray<THeap, Byte>();
                CharStr data = (CharStr)BaseString::operator new[](this->CurrentSize * sizeof(Char));
                Memory::Copy(this->str, data, CurrentSize * sizeof(Char));
                return Containers::BaseArray<THeap, Byte>(MoveRValue(reinterpret_cast<Byte*>(data)), this->CurrentSize * sizeof(Char));
            }

            ///<summary>Конвертаця строки шістнадцятиричних символів в байтовий масив</summary>
            ///<param name = "stringData">Текстове представлення шістнадцятиричних символів</param>
            ///<returns>Масив байт</returns>
            ///<exception cref="Seed::Exceptions::NullPointException">Якщо рядок пустий</exception>
            ///<exception cref="Seed::Exceptions::InvalidFormatException">Якщо рядок містить не коректні символи або його розмір не кратний 2</exception>
            static Containers::BaseArray<THeap, Byte> hexToBinary(const BaseString& stringData) {
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
                }
                return bytes;
            }

            ///<summary>Перетворення числа, структури, класу чи покажчика в hex рядок</summary>
            ///<param name = "Data">Дані які перетворить в шістнадцятиричне представлення</param>
            template<typename T>
            static BaseString toHexString(const T& Data) {
                return binaryToHex(Containers::Array<Byte>(reinterpret_cast<const Byte*>(&Data), sizeof(T)));
            }

            //Заміна по таблиці символів
            //Може згенерувати помилку IndexOutOfRange, якщо
            //- StartPos більша ніж кількість символів в рядку
            //- StartPos більша ніж EndPos
            BaseString & Replace(
                const Containers::BaseMap<THeap, Char, Char> & ReplaceTable,
                SizeT StartPos = 0,
                SizeT EndPos = Seed::Math::Max<SizeT>()
            ) {
                if (StartPos > this->CurrentSize)
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, StartPos, this->CurrentSize);
                if (StartPos > EndPos)
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, StartPos, EndPos);
                if (EndPos > this->CurrentSize) EndPos = this->CurrentSize;
                ReplaceTable.ForEach([&](const Containers::BasePair<THeap, Char, Char>& pair) {
                    for (SizeT i = StartPos; i < EndPos; i++) {
                        if (this->str[i] == pair.Key)
                            this->str[i] = pair.Value;
                    }
                    });
                return *this;
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
            BaseString& Replace(
                Char What,
                Char To,
                SizeT StartPos = 0,
                SizeT EndPos = Seed::Math::Max<SizeT>()
            ) {
                if (StartPos > this->CurrentSize)
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, StartPos, this->CurrentSize);
                if (StartPos > EndPos)
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, StartPos, EndPos);
                if (EndPos > this->CurrentSize) EndPos = this->CurrentSize;
                for (SizeT i = StartPos; i < EndPos; i++) {
                    if (this->str[i] == What)
                        this->str[i] = To;
                }
                return *this;
            }

            ///<summary>Приведення всіх букв в заглавні</summary>
            ///<returns>Силку на поточний рядок</retutns>
            BaseString& toUpcase() noexcept {
                for (SizeT i = 0; i < this->CurrentSize; i++)
                    str[i] = CStr::ToUpcase(str[i]);
                return *this;
            }

            ///<summary>Приведення всіх букв в нижній регістр</summary>
            ///<returns>Силку на поточний рядок</retutns>
            BaseString& toDowncase() noexcept {
                for (SizeT i = 0; i < this->CurrentSize; i++)
                    str[i] = CStr::ToDowncase(str[i]);
                return *this;
            }

            ///<summary>Розширює рядок до розміру NewSize
            ///<para>заповнюючи елементи після поточного розміру символами Filler</para></summary>
            ///<params name = "Filler">Символ заповнювач</params>
            ///<params name = "NewSize">Новий розмір рядка</params>
            ///<returns>Силку на поточний рядок після розширення</returns>
            ///<exception cref="Seed::Exceptions::IndexOutOfRange">NewSize менший ніж кількість символів в рядку</exception>
            BaseString& Extend(Char Filler, SizeT NewSize) {
                if (this->CurrentSize > NewSize)
                    Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, NewSize, this->CurrentSize);
                if (this->CurrentSize == NewSize)
                    return *this;

                if (NewSize >= ReservedSize) {
                    SizeT NewFullSize = NewSize + CurrentSettings().MemoryManager.StringReserveSize;
                    CharStr buf = (CharStr)BaseString::operator new[](NewSize * sizeof(Char));
                    Memory::Copy(this->str, buf, this->CurrentSize * sizeof(Char));
                    BaseString::operator delete[](this->str);
                    this->str = buf;
                    this->ReservedSize = NewFullSize;
                }
                for (SizeT i = CurrentSize - 1; i < NewSize; i++)
                {
                    this->str[i] = Filler;
                }
                this->str[NewSize] = L'\0';
                this->CurrentSize = NewSize;

                return *this;
            }

            ///<summary>Конвертор для строк з різним алокатором</summary>
            ///<param name = "NewBase">Новий алокатор</summary>
            ///<returns>Копія рядка алокована алокатором NewBase</returns>
            template<typename NewBase>
            operator BaseString<NewBase>() const {
                return BaseString<NewBase>((const CharStr)(ConstCharStr)*this);
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
            __forceinline virtual ~BaseString() override {
                if (str == nullptr) return;
                BaseString::operator delete[](str);
                str = nullptr;
                CurrentSize = ReservedSize = 0;
            }
        protected:
            template<template<typename THeap> typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
            friend class Containers::LINQ::IStringCollectionBaseLinq;
            template<template<typename THeap> typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
            friend class Containers::LINQ::IString;
        protected:
            template<typename LambdaT>
            __forceinline BaseString& ForEachImpl(LambdaT&& lambda) {
                if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, UChar&>) {
                    for (SizeT i = 0; i < this->Lenght; i++)
                    {
                        lambda(this->str[i]);
                    }
                    return *this;
                }
                else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, UChar&, SizeT>) {
                    for (SizeT i = 0; i < this->Lenght; i++)
                    {
                        lambda(this->str[i], i);
                    }
                    return *this;
                }
                else {
	                static_assert(false);
                }
            }

            template<typename LambdaT>
            __forceinline const BaseString& ForEachImpl(LambdaT&& lambda) const {
                if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const UChar&>) {
                    for (SizeT i = 0; i < this->Lenght; i++)
                    {
                        lambda(this->str[i]);
                    }
                    return *this;
                }
                else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const UChar&, SizeT>) {
                    for (SizeT i = 0; i < this->Lenght; i++)
                    {
                        lambda(this->str[i], i);
                    }
                    return *this;
                }
                else {
	                static_assert(false);
                }
            }

            template<typename LambdaT>
            __forceinline BaseString& ForEachImpl(LambdaT&& lambda, SizeT count) {
                if (count > this->Count)
                    count = this->Count;
                if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, UChar&>) {
                    for (SizeT i = 0; i < count; i++)
                    {
                        lambda(this->str[i]);
                    }
                    return *this;
                }
                else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, UChar&, SizeT>) {
                    for (SizeT i = 0; i < count; i++)
                    {
                        lambda(this->str[i], i);
                    }
                    return *this;
                }
                else {
	                static_assert(false);
                }
            }

            template<typename LambdaT>
            __forceinline const BaseString& ForEachImpl(LambdaT&& lambda, SizeT count) const {
                if (count > this->Lenght)
                    count = this->Lenght;
                if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const UChar&>) {
                    for (SizeT i = 0; i < count; i++)
                    {
                        lambda(this->str[i]);
                    }
                    return *this;
                }
                else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const UChar&, SizeT>) {
                    for (SizeT i = 0; i < count; i++)
                    {
                        lambda(this->str[i], i);
                    }
                    return *this;
                }
                else {
	                static_assert(false);
                }
            }
        protected:
            bool AnyImpl() const { return this->Lenght > 0; }

            template<typename Lambda>
            __forceinline bool AnyImpl(Lambda&& lambda) const {
                for (SizeT i = 0; i < this->Lenght; i++)
                {
                    if (lambda(this->str[i]))
                        return true;
                }
                return false;
            }

            template<typename Lambda>
            __forceinline bool AllImpl(Lambda&& lambda) const {
                for (SizeT i = 0; i < this->Lenght; i++)
                {
                    if (!lambda(this->str[i]))
                        return false;
                }
                return true;
            }

            BaseString& ReverseImpl() {
                for (SizeT i = 0; i < this->Lenght / 2; i++) {
                    Swap(this->str[i], this->str[(this->Lenght - 1) - i]);
                }
                return *this;
            }

            __forceinline void UnsafeSet(const UChar& Value, SizeT Index) {
                this->str[Index] = Value;
            }

            __forceinline BaseString& RemoveAllImpl() {
                this->~BaseString();
                return *this;
            }

            template<typename Lambda>
            __forceinline BaseString& RemoveAllImpl(Lambda&& lambda) {
                for (SizeT i = 0; i < CurrentSize; i++) {
                    lambda(MoveRValue(this->str[i]));
                }
                this->~BaseString();
            	return *this;
            }

            template<typename TOtherHeap>
            friend class BaseString;
        };

        ///<summary>Тип кінця рядка</summary>
        namespace { typedef const Char(&EndLineT)[3]; }
        ///<summary>Позначка переходу на новий рядок, використовується CRLF режим</summary>
        constexpr __forceinline EndLineT NextLine() {
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

__forceinline Seed::Core::String operator "" S(const wchar_t* const text, const Seed::SizeT size)
{ //-V2537
	using namespace Seed;
	return String((Char*)text); //-V2533
}
    
#pragma warning(pop)