#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Math.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.DateTime.h"
#include "Seed.Core.Containers.Array.h"
#include "Seed.Core.Containers.List.h"

namespace Seed {
	inline namespace Core {
		class SeedKernelCall StringToLong :public Exception {
		private:
			SizeT requiredSize;
		public:
			StringToLong(ConstACharStr File, ConstACharStr FunctName, SizeT Line, SizeT RequiredSize) :
				Exception(File, FunctName, Line, L"String to long"), requiredSize(RequiredSize) {}

			SizeT GetRequiredSize() { return requiredSize; }

			SizeT Property(GetRequiredSize) RequiredSize;
		};

		class SeedKernelCall InvalidNumberFormatException : public Exception {
		public:
			InvalidNumberFormatException(const AChar * File, const AChar * FunctName, SizeT Line) :
				Exception(File, FunctName, Line, L"Invalid integer exception") {}
		};

#pragma warning(push)
#pragma warning(disable: 4996)
		template<class Base>
		class SeedKernelCall BaseString : public Memory::Allocator<Base> {
		private:
			//Покажчик на рядковий масив
			CharStr str;
			//Поточна кількість символів
			SizeT CurrentSize;
			//Розмір буфера
			SizeT ReservedSize;
			//Таблиця ASCI значеннь
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

			BaseString(CharStr && rawPtr, SizeT CurrentSize, SizeT ReservedSize)
				:str(rawPtr), CurrentSize(CurrentSize), ReservedSize(ReservedSize) {}

		public:
			//Масив символів шістнадцятирядкового числа
			static constexpr const auto HexNumbers = L"0123456789abcdef";
		public:
			//Конструктор
			__forceinline BaseString() noexcept : str(nullptr), CurrentSize(0), ReservedSize(0) {};
			//Конструктор з готової рядка
			BaseString(const Char * text) {
				CurrentSize = CStr::Size((CharStr)text); //-V2005
				if (CurrentSize == Math::Max<SizeT>())
					Throw(Seed::StringToLong(ExceptionPosition, CurrentSize));
				ReservedSize = CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
				CStr::Copy((CharStr)text, this->str); //-V2005
			}

			//Конструктор з готової рядка
			BaseString(const AChar * text) {
				CurrentSize = CStr::Size((ACharStr)text); //-V2005
				if (CurrentSize == Math::Max<SizeT>())
					Throw(Seed::StringToLong(ExceptionPosition, CurrentSize));
				ReservedSize = CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
				CharStr temp_str = this->str;
				{
					while (*temp_str++ = *text++);
				}
			}

			//Конструктор з готової рядка і його розміру
			BaseString(const Char * text, SizeT size) {
				if (size == 0) {
					CurrentSize = 0;
					ReservedSize = CurrentSettings::Memory::GetStringReserveSpace();
					this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
					this->str[0] = L'\0';
					return;
				}
				CurrentSize = size;
				if (CurrentSize > Math::Max<SizeT>())
					Throw(Seed::StringToLong(ExceptionPosition, CurrentSize));
				ReservedSize = CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
				for (SizeT i = 0; i < size; i++) {
					this->str[i] = text[i];
				}
				this->str[size] = L'\0';
			}
			//Конструктор з стартовим резервованим розміром
			BaseString(SizeT StartSize) {
				if (StartSize > Math::Max<SizeT>())
					Throw(Seed::StringToLong(ExceptionPosition, CurrentSize));
				this->ReservedSize = StartSize;
				this->CurrentSize = 0;
				this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
			}
			//Конструктор з готової рядка і його розміру
			BaseString(CharStr& text, SizeT size) {
				this->str = text;
				text = nullptr;
				this->CurrentSize = this->ReservedSize = size;
			}

			//Копіювальний конструктор
			BaseString(const BaseString & from) {
				if (from.str != nullptr) {
					this->CurrentSize = from.CurrentSize;
					this->ReservedSize = from.ReservedSize;
					this->str = (CharStr)BaseString::operator new[](ReservedSize * sizeof(Char));
					CStr::Copy(from.str, this->str);
				}
				else {
					this->CurrentSize = this->ReservedSize = 0;
					this->str = nullptr;
				}
			}
			//Оптимізований конструктор
			__forceinline BaseString(BaseString && from) noexcept {
				this->str = from.str; from.str = nullptr;
				this->CurrentSize = from.CurrentSize; from.CurrentSize = 0;
				this->ReservedSize = from.ReservedSize; from.ReservedSize = 0;
			}

			//Оператор рівності
			__forceinline Boolean operator==(const BaseString & second) const noexcept {
				if (this->CurrentSize != second.CurrentSize) return false;
				if (this->str == second.str) return true;
				for (SizeT i = 0; i < CurrentSize; i++) {
					if (this->str[i] != second.str[i]) return false;
				}
				return true;
			}

			//Оператор нерівності
			__forceinline Boolean operator!=(const BaseString & second) const noexcept {
				return !this->operator==(second);
			}

			//Оператор рівності
			__forceinline Boolean operator==(const Char second[]) const noexcept {
				if (this->str == second) return true;
				SizeT SecondSize = Seed::CStr::Size((CharStr&)second);
				if (this->CurrentSize != SecondSize) return false;
				for (SizeT i = 0; i < CurrentSize; i++) {
					if (this->str[i] != second[i]) return false;
				}
				return true;
			}

			//Оператор нерівності
			__forceinline Boolean operator!=(const Char second[]) const noexcept {
				return !this->operator==(second);
			}

			//Оператор рівності
			__forceinline friend Boolean operator==(const Char second[], BaseString first) noexcept {
				return first.operator==(second);
			}

			//Оператор нерівності
			__forceinline friend Boolean operator!=(const Char second[], BaseString first) noexcept {
				return first.operator!=(second);
			}

			//Оператор рівності
			template<typename OtherBase>
			__forceinline Boolean operator==(const BaseString<OtherBase> & OtherString) const noexcept {
				if (this->CurrentSize != OtherString.Size()) return false;
				for (SizeT i = 0; i < CurrentSize; i++) {
					if (this->str[i] != OtherString[i]) return false;
				}
				return true;
			}

			//Оператор присвоєння
			BaseString& operator=(const BaseString& orig) {
				if (*this == orig) return *this;
				if (this->ReservedSize >= orig.CurrentSize) {
					CStr::Copy(orig.str, this->str);
					this->CurrentSize = orig.CurrentSize;
				}
				else {
					BaseString::operator delete[](this->str);
					this->str = nullptr;
					this->str = (CharStr)BaseString::operator new[](sizeof(Char) * orig.ReservedSize);
					CStr::Copy(orig.str, this->str);
					this->CurrentSize = orig.CurrentSize;
					this->ReservedSize = orig.ReservedSize;
				}
				return *this;
			}

			//Оператор оптимізованого присвоєння
			__forceinline BaseString& operator=(BaseString&& orig) noexcept {
				if (orig.str == this->str) return *this;
				this->~BaseString();
				this->str = orig.str; orig.str = nullptr;
				this->CurrentSize = orig.CurrentSize; orig.CurrentSize = 0;
				this->ReservedSize = orig.ReservedSize; orig.ReservedSize = 0;
				return *this;
			}

			//Оператор додавання масиву букв до рядка
			BaseString operator+(const Char * second) const {
				BaseString ret_data;
				SizeT Size = CStr::Size((CharStr)second); //-V2005
				SizeT NewSize = CurrentSize + Size;
				if (NewSize > Math::Max<SizeT>())
					Throw(Seed::StringToLong(ExceptionPosition, NewSize));
				SizeT NewFullSize = NewSize + CurrentSettings::Memory::GetStringReserveSpace();
				if (NewFullSize > Math::Max<SizeT>())
					NewFullSize = Math::Max<SizeT>();
				ret_data.str = (CharStr)BaseString::operator new[](sizeof(Char) * NewFullSize); //-V2005
				CStr::Copy(this->str, ret_data.str);
				CStr::Copy(second, ret_data.str + this->CurrentSize);
				ret_data.CurrentSize = NewSize;
				ret_data.ReservedSize = NewFullSize;
				return ret_data;
			}

			////Оператор додавання масиву букв до рядка
			//friend BaseString operator+(const Char* first, const BaseString string);

			//Оператор додавання масиву букв до рядка
			BaseString operator+(const AChar * second) const {
				BaseString ret_data;
				SizeT Size = CStr::Size((ACharStr)second); //-V2005
				SizeT NewSize = CurrentSize + Size;
				if (NewSize > Math::Max<SizeT>())
					Throw(StringToLong(ExceptionPosition, NewSize));
				SizeT NewFullSize = NewSize + Math::Max<SizeT>();
				if (NewFullSize > Math::Max<SizeT>())
					NewFullSize = Math::Max<SizeT>();
				ret_data.str = (CharStr)BaseString::operator new[](sizeof(Char) * NewFullSize); //-V2005
				if (this->str != nullptr)
					CStr::Copy(this->str, ret_data.str);
				CharStr temp_str = ret_data.str + this->CurrentSize;
				{
					while (*temp_str++ = *second++);
				}
				ret_data.CurrentSize = NewSize;
				ret_data.ReservedSize = NewFullSize;
				return ret_data;
			}

			//Оператор додавання рядка
			BaseString<Base> operator+(const BaseString & second) const {
				SizeT NewSize = CurrentSize + second.CurrentSize;
				if (NewSize > Math::Max<SizeT>())
					Throw(StringToLong(ExceptionPosition, NewSize));
				SizeT NewFullSize = NewSize + Math::Max<SizeT>();
				if (NewFullSize > Math::Max<SizeT>())
					NewFullSize = Math::Max<SizeT>();
				BaseString retData;
				CharStr buf = (CharStr)BaseString::operator new[](sizeof(Char)*NewFullSize); //-V2005
				if (this->str != nullptr)
					CStr::Copy(this->str, buf);
				CStr::Copy(second.str, buf + CurrentSize);
				retData.str = buf;
				retData.CurrentSize = NewSize;
				retData.ReservedSize = NewFullSize;
				return retData;
			}

			//Оператор додавання рядка
			template<typename OtherBase>
			BaseString<Base> operator+(const BaseString<OtherBase> & second) const {
				SizeT NewSize = CurrentSize + second.Size();
				if (NewSize > Math::Max::SizeT)
					Throw(Seed::StringToLong(ExceptionPosition, NewSize));
				SizeT NewFullSize = NewSize + Math::Max::SizeT;
				if (NewFullSize > Math::Max::SizeT)
					NewFullSize = Math::Max::SizeT;
				BaseString retData;
				CharStr buf = (CharStr)BaseString<Base>::operator new[](sizeof(Char)*NewFullSize); //-V2005
				if (this->str != nullptr)
					CStr::Copy(this->str, buf);
				CStr::Copy(second.operator const Seed::CharStr(), buf + CurrentSize);
				retData.str = buf;
				retData.CurrentSize = NewSize;
				retData.ReservedSize = NewFullSize;
				return retData;
			}

			//Оператор контактенації масиву букв до поточного рядка
			BaseString<Base> & operator+=(const CharStr temp_str) {
				SizeT StrSize = CStr::Size(temp_str);
				if (StrSize > Math::Max<SizeT>())
					Throw(Seed::StringToLong(ExceptionPosition, StrSize));
				SizeT NewSize = CurrentSize + StrSize;
				if (NewSize > ReservedSize) {
					NewSize = NewSize + Math::Max<SizeT>();
					CharStr buf = (CharStr)BaseString<Base>::operator new[](sizeof(Char)*NewSize); //-V2005
					if (this->str != nullptr)
						CStr::Copy(this->str, buf);
					CStr::Copy(temp_str, buf + CurrentSize);
					BaseString::operator delete[](this->str);
					this->str = buf;
					CurrentSize += StrSize;
					ReservedSize = NewSize;
				}
				else {
					CStr::Copy(str, this->str + CurrentSize);
					CurrentSize = NewSize;
				}
				return *this;
			}

			//Оператор контактенації рядка до поточного рядка
			BaseString & operator+=(const BaseString & orig) {
				SizeT NewSize = this->CurrentSize + orig.CurrentSize;
				if (NewSize < this->ReservedSize) {
					CStr::Copy(orig.str, this->str + this->CurrentSize);
					this->CurrentSize = NewSize;
					return *this;
				}
				else {
					ReservedSize = NewSize + Math::Max<SizeT>();
					CharStr buf = (CharStr)BaseString::operator new[](sizeof(Char)*ReservedSize);
					if (this->str != nullptr)
						CStr::Copy(this->str, buf);
					CStr::Copy(orig.str, buf + this->CurrentSize);
					BaseString::operator delete[](this->str);
					this->str = buf;
					this->str[NewSize] = L'\0';
					this->CurrentSize = NewSize;
				}
				return *this;
			}
			//Оператор контактенації рядка до поточного рядка
			template<typename OtherBase>
			BaseString & operator+=(const BaseString<OtherBase> & orig) {
				SizeT NewSize = this->CurrentSize + orig.Size();
				if (NewSize < this->ReservedSize) {
					CStr::Copy(orig.operator const Seed::CharStr(), this->str + this->CurrentSize);
					this->CurrentSize = NewSize;
					return *this;
				}
				else {
					ReservedSize = NewSize + CurrentSettings::Memory::GetStringReserveSpace();;
					CharStr buf = (CharStr)BaseString::operator new[](sizeof(Char)*ReservedSize); //-V2005
					if (this->str != nullptr)
						CStr::Copy(this->str, buf);
					CStr::Copy(orig.operator const Seed::CharStr(), buf + this->CurrentSize);
					BaseString::operator delete[](this->str); //-V2005
					this->str = buf;
					this->CurrentSize = NewSize;
				}
				return *this;
			}

			//Оператор доступу до символів рядка
			__forceinline Char & operator[](SizeT index) {
				if (index < CurrentSize) return str[index];
				else Throw(IndexOutOfRange(ExceptionPosition, index, CurrentSize));
			}
			//Констатний оператор доступу до символів рядка
			__forceinline const Char & operator[](SizeT index) const {
				if (index < CurrentSize) return str[index];
				else Throw(IndexOutOfRange(ExceptionPosition, index, CurrentSize));
			}

			//Вертає чи строка починається з строки ExpectedString
			__forceinline bool IsStartWith(const BaseString & ExpectedString) const {
				if (this->CurrentSize < ExpectedString.CurrentSize) return false;
				for (SizeT i = 0; i < ExpectedString.CurrentSize; i++)
					if (this->str[i] != ExpectedString.str[i]) return false;
				return true;
			}

			//Вертає чи строка починається з строки ExpectedString
			__forceinline bool IsStartWith(const Char* ExpectedString) const {
				SizeT ExpectedStringSize = CStr::Size(ExpectedString);
				if (this->CurrentSize < ExpectedStringSize) return false;
				for (SizeT i = 0; i < ExpectedStringSize; i++)
					if (this->str[i] != ExpectedString[i]) return false;
				return true;
			}

			//Оператор конвертації до символьного масиву
			__forceinline explicit operator const CharStr() const noexcept { return str; }
#pragma region Converting to string
#pragma region Converting int types to string
#pragma region Convertinh signed int types to string
			//Конвертація цілого числа до рядка
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
				BaseString str;
				str.CurrentSize = i;
				str.ReservedSize = i + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				Reverse(str.str, str.CurrentSize);
				return str;
			}

			//Конвертація цілого числа до рядка
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
				BaseString str;
				str.CurrentSize = i;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				Reverse(str.str, str.CurrentSize);
				return str;
			}

			//Конвертація цілого числа до рядка
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
				BaseString str;
				str.CurrentSize = i;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				Reverse(str.str, str.CurrentSize);
				return str;
			}
			//Конвертація цілого числа до рядка
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
				BaseString str;
				str.CurrentSize = i;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				Reverse(str.str, str.CurrentSize);
				return str;
			}
#pragma endregion
#pragma region Convert unsigned int types to string
			//Конвертація цілого числа до рядка
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
				BaseString str;
				str.CurrentSize = i;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				Reverse(str.str, str.CurrentSize);
				return str;
			}
			//Конвертація цілого числа до рядка
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
				BaseString str;
				str.CurrentSize = i;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				Reverse(str.str, str.CurrentSize);
				return str;
			}
			//Конвертація цілого числа до рядка
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
				BaseString str;
				str.CurrentSize = i;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				Reverse(str.str, str.CurrentSize);
				return str;
			}
			//Конвертація цілого числа до рядка
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
				BaseString str;
				str.CurrentSize = i;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)* str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				Reverse(str.str, str.CurrentSize);
				return str;
			}
#pragma endregion
#pragma endregion
			//Конвертація логічного значення до рядка вертає 'True' або 'False'
			__forceinline static BaseString toString(Boolean val) {
				if (val) return BaseString(L"True"); else return BaseString(L"False");
			}
			//Конвертація логічного значення до рядка вертає вміст рядка IfTrue або IfFalse
			__forceinline static BaseString toString(Boolean val, const BaseString & IfTrue, const BaseString & IfFalse) {
				if (val) return IfTrue; else return IfFalse;
			}
#pragma endregion
#pragma region Converting string to types
#pragma region Converting float typed to string
			//Конвертація дійсного числа з одинарною точністю до рядка
			static BaseString toString(Float val) {
				using namespace Math;
				if (val == 0.0F) return L"0"; //-V550
				if (val != val) return L"NaN";
				if (val == Seed::Math::Infinity) return L"Infinity"; //-V550
				if (val == -Seed::Math::Infinity) return L"-Infinity"; //-V550
				Boolean sign = Math::Sign(val);
				if (sign) Math::Abs(val);
				Float IntPart;
				Float DoublePart = Math::ModD(val, IntPart);
				DoublePart *= 10e5;
				Char buf[25];
				SizeT size = 0;
				while (DoublePart >= 1) {
					buf[size] = L'0' + (Int32)(DoublePart) % 10; //-V2005 //-V2003
					size++;
					DoublePart /= 10;
				}
				buf[size] = L'.';
				size++;
				if (IntPart == 0.0F) { //-V550
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
				Reverse(buf, size);
				BaseString str;
				str.CurrentSize = size;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)*str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				return str;
			}

			//Конвертація дійсного числа з одинарною точністю до рядка з певною кількістю знаків після коми
			static BaseString toString(Float val, UInt8 precision) {
				using namespace Math;
				if (val == 0.0F) return L"0"; //-V550
				if (val != val) return L"NaN";
				if (val == Infinity) return L"Infinity"; //-V550
				if (val == -Infinity) return L"-Infinity"; //-V550
				if (precision > 10) precision = 10;
				Boolean sign = Math::Sign(val);
				if (sign) val *= -1.0F;
				Float IntPart;
				Float DoublePart = Math::ModD(val, IntPart);
				DoublePart *= Math::Pow(10, precision);
				Char buf[25];
				SizeT size = 0;
				while (DoublePart >= 1) {
					buf[size] = L'0' + (Int32)(DoublePart) % 10; //-V2005 //-V2003
					size++;
					DoublePart /= 10;
				}
				buf[size] = L'.';
				size++;
				if (IntPart == 0.0F) { //-V550
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
				Reverse(buf, size);
				BaseString str;
				str.CurrentSize = size;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)*str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				return str;
			}

			//Конвертація дійсного числа з подвійною точністю до рядка
			static BaseString toString(Double val) {
				using namespace Math;
				if (val == 0.0) return L"0"; //-V550
				if (val != val) return L"NaN";
				if (val == Seed::Math::Infinity) return L"Infinity"; //-V550
				if (val == -Seed::Math::Infinity) return L"-Infinity"; //-V550
				Boolean sign = Math::Sign(val);
				if (sign) val *= -1.0;
				Double IntPart;
				Double DoublePart = Math::ModD(val, IntPart);
				DoublePart *= 10e5;
				Char buf[40];
				SizeT size = 0;
				if (DoublePart == 0.0) { //-V550
					buf[0] = L'0';
					size++;
				}
				else {
					while (DoublePart >= 1) {
						buf[size] = L'0' + (Int32)(DoublePart) % 10; //-V2005 //-V2003
						size++;
						DoublePart /= 10;
					}
				}
				buf[size] = L'.';
				size++;
				if (IntPart == 0.0) { //-V550
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
				Reverse(buf, size);
				BaseString str;
				str.CurrentSize = size;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)*str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				return str;
			}

			//Конвертація дійсного числа з подвійною точністю до рядка з певною кількістю знаків після коми
			static BaseString toString(Double val, UInt8 precision) {
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
				buf[size] = L'.';
				size++;
				if (IntPart == 0.0) { //-V550
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
				Reverse(buf, size);
				BaseString str;
				str.CurrentSize = size;
				str.ReservedSize = str.CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				str.str = (CharStr)BaseString::operator new[](sizeof(Char)*str.ReservedSize); //-V2005
				CStr::Copy(buf, str.str);
				return str;
			}
#pragma endregion
#pragma region Converting to int types
#pragma region Converting to unsigned int types
			//Спроба перетворення в ціле число
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			UInt8 toUInt8() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 3)
					Throw(IntegerOverflowException(ExceptionPosition));
				Math::SafeInt<UInt8> ret;
				for (SizeT i = 0; i < CurrentSize; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					ret += static_cast<UInt8>(str[i] - L'0');
					if (i != CurrentSize - 1)
						ret *= 10;
				}
				return ret.Value;
			}

			//Спроба перетворення в ціле число
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			UInt16 toUInt16() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 5)
					Throw(IntegerOverflowException(ExceptionPosition));
				Math::SafeInt<UInt16> ret = 0;
				for (SizeT i = 0; i < CurrentSize; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					ret += str[i] - L'0';
					if (i != CurrentSize - 1)
						ret *= 10;
				}
				return ret.Value;
			}

			//Спроба перетворення в ціле число
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			UInt32 toUInt32() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 10)
					Throw(IntegerOverflowException(ExceptionPosition));
				Math::SafeInt<UInt32> ret = 0;
				for (SizeT i = 0; i < CurrentSize; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					ret += str[i] - L'0';
					if (i != CurrentSize - 1)
						ret *= 10;
				}
				return ret.Value;
			}

			//Спроба перетворення в ціле число
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			UInt64 toUInt64() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 20)
					Throw(IntegerOverflowException(ExceptionPosition));
				Math::SafeInt<UInt64> ret = 0;
				for (SizeT i = 0; i < CurrentSize; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					ret += str[i] - L'0';
					if (i != CurrentSize - 1)
						ret *= 10;
				}
				return ret.Value;
			}

			//Спроба перетворення в ціле число
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			__forceinline SizeT toSizeT() {
				return toUInt64();
			}
#pragma endregion
#pragma region Convert to signed int types
			//Спроба перетворення в ціле число,
			//на початку може стояти символ '+' або '-'
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			Int8 toInt8() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 20)
					Throw(IntegerOverflowException(ExceptionPosition));
				Boolean sign(this->str[0] == '-');
				SizeT i = 0;
				if (str[0] == L'-' || str[0] == L'+')
					i = 1;
				Math::SafeInt<UInt8> ret = 0;
				for (i; i < CurrentSize; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					ret += str[i] - L'0';
					if (i != CurrentSize - 1)
						ret *= 10;
				}
				return Math::ConvertUnsignedToSigned<UInt8>(ret.Value);
			}

			//Спроба перетворення в ціле число,
			//на початку може стояти символ '+' або '-'
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			Int16 toInt16() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 20)
					Throw(IntegerOverflowException(ExceptionPosition));
				Boolean sign(this->str[0] == '-');
				SizeT i = 0;
				if (str[0] == L'-' || str[0] == L'+')
					i = 1;
				Math::SafeInt<UInt16> ret = 0;
				for (i; i < CurrentSize; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					ret += str[i] - L'0';
					if (i != CurrentSize - 1)
						ret *= 10;
				}
				return Math::ConvertUnsignedToSigned<UInt16>(ret.Value);
			}

			//Спроба перетворення в ціле число,
			//на початку може стояти символ '+' або '-'
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			Int32 toInt32() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 20)
					Throw(IntegerOverflowException(ExceptionPosition));
				Boolean sign(this->str[0] == '-');
				SizeT i = 0;
				if (str[0] == L'-' || str[0] == L'+')
					i = 1;
				Math::SafeInt<UInt32> ret = 0;
				for (i; i < CurrentSize; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					ret += str[i] - L'0';
					if (i != CurrentSize - 1)
						ret *= 10;
				}
				return Math::ConvertUnsignedToSigned<UInt32>(ret.Value);
			}

			//Спроба перетворення в ціле число,
			//на початку може стояти символ '+' або '-'
			//
			//Може генерувати помилки:
			//	NullPointException	- якщо рядок пустий
			//	IntegerOverflow	    - якщо число є завеликим
			//	InvalidFormat		- якщо в рядку є відміний від числа символ
			Int64 toInt64() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 20)
					Throw(IntegerOverflowException(ExceptionPosition));
				Boolean sign(this->str[0] == '-');
				SizeT i = 0;
				if (str[0] == L'-' || str[0] == L'+')
					i = 1;
				Math::SafeInt<UInt64> ret = 0;
				for (i; i < CurrentSize; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					ret += str[i] - L'0';
					if (i != CurrentSize - 1)
						ret *= 10;
				}
				return Math::ConvertUnsignedToSigned<UInt64>(ret.Value);
			}
#pragma endregion
#pragma endregion
#pragma region Converting to float point types
			//Перетворення тексту в дійсне число Float
			//
			//Можу генерувати помилки:
			//	NullPointException	- якщо строка є нуль-строкою
			//	FloatOverflow	    - якщо кількість цифр є більшою ніж можу вмістити число Float
			//	InvalidFormat		- якщо в строці є відміні від цифр і одної крапки символи
			Float toFloat() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 9)
					Throw(IntegerOverflowException(ExceptionPosition));
				Boolean sign(this->str[0] == '-');
				size_t i = 0;
				if (str[0] == L'-' || str[0] == L'+')
					i = 1;
				Float int_part = 0.0, double_part = 0.0;
				for (i; i < CurrentSize && this->str[i] != L'.'; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					int_part += this->str[i] - L'0';
					int_part *= 10;
				}
				int_part /= 10;
				if (i != CurrentSize) {
					i++;
					for (size_t j = CurrentSize - 1; j >= i; j--) {
						if (str[i]<L'0' || str[i]>L'9')
							Throw(InvalidNumberFormatException(ExceptionPosition));
						double_part += this->str[j] - L'0';
						double_part /= 10;
					}
					int_part += double_part;
				}
				return int_part * ((sign) ? -1.0f : 1.0f);
			}

			//Перетворення тексту в дійсне число Double
			//
			//Можу генерувати помилки:
			//	NullPointException	- якщо строка є нуль-строкою
			//	FloatOverflow	    - якщо кількість цифр є більшою ніж можу вмістити число Double
			//	InvalidFormat		- якщо в строці є відміні від цифр і одної крапки символи
			Double toDouble() {
				if (this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				if (this->CurrentSize > 18)
					Throw(IntegerOverflowException(ExceptionPosition));
				Boolean sign(this->str[0] == '-');
				size_t i = 0;
				if (str[0] == L'-' || str[0] == L'+')
					i = 1;
				Double int_part = 0.0, double_part = 0.0;
				for (i; i < CurrentSize && this->str[i] != L'.'; i++) {
					if (str[i]<L'0' || str[i]>L'9')
						Throw(InvalidNumberFormatException(ExceptionPosition));
					int_part += this->str[i] - L'0';
					int_part *= 10;
				}
				int_part /= 10;
				if (i != CurrentSize) {
					i++;
					for (size_t j = CurrentSize - 1; j >= i; j--) {
						if (str[i]<L'0' || str[i]>L'9')
							Throw(InvalidNumberFormatException(ExceptionPosition));
						double_part += this->str[j] - L'0';
						double_part /= 10;
					}
					int_part += double_part;
				}
				return int_part * ((sign) ? -1.0 : 1.0);
			}
#pragma endregion
			//Вертає рядкове представлення UUID
			static BaseString<Base> toString(const UUID & id) {
				auto ReservedSize = sizeof(UUID::UUIDStringWildCard) + CurrentSettings::Memory::GetStringReserveSpace();
				CharStr buffer = reinterpret_cast<CharStr>(BaseString<Base>::operator new[](sizeof(UUID::UUIDStringWildCard)));
				extern void ConvertUUIDtoCharStr(const Seed::Core::UUID & id, Seed::CharStr buffer);
				ConvertUUIDtoCharStr(id, buffer);
				return BaseString<Base>(MoveRValue(buffer), sizeof(UUID::UUIDStringWildCard), ReservedSize);
			}
			//Перетворення дати і часу в строкове представлення
			static BaseString<Base> toString(Seed::Core::Date date) {
				Char buffer[22];
				extern void ConvertDateToStringImpl(Seed::Core::Date date, Seed::Core::CharStr buffer);
				ConvertDateToStringImpl(date, buffer);
				return buffer;
			}
			//Перетворення часу і дати в строкове представлення
			static BaseString<Base> toString(Seed::Core::DateTime time) {
				Char buffer[22];
				extern void ConvertDateTimeToStringImpl(Seed::Core::DateTime time, Seed::CharStr buffer);
				ConvertDateTimeToStringImpl(time, buffer);
				return buffer;
			}

			static BaseString<Base> toString(const Seed::Core::Array<Seed::Byte> & array) {
				BaseString<Base> str;
				str.ReservedSize = str.CurrentSize = array.Count / sizeof(Char);
				str.str = new Char[str.CurrentSize + 1];
				Memory::Copy(array.Raw, str.str, array.Count);
				str.str[str.CurrentSize] = L'\0';
				return str;
			}
#pragma endregion

			//Конвертація з рядкового в бінарне представлення UUID
			UUID toUUID() {
				if (this->str == nullptr || this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				UUID res;
				extern bool ConvertCharStrToUUID(Seed::Core::CharStr str, Seed::Core::UUID & uuid);
				if (!ConvertCharStrToUUID(this->str, res)) {
					Throw(InvalidFormat(ExceptionPosition));
				}
				return res;
			}

			//Поточна довжина строки
			SizeT GetSize() const { return CurrentSize; }

			SizeT Property(GetSize) Size;

			//Зміна розміру рядка
			BaseString & Reserve(SizeT NewReservedSize) {
				if (this->str == nullptr)
				{
					this->str = static_cast<CharStr>(this->operator new[](sizeof(Char) * NewReservedSize));
					this->str[0] = L'\0';
					this->CurrentSize = 0;
					this->ReservedSize = NewReservedSize;
					return *this;
				}
				if (CurrentSize > NewReservedSize)
					str[NewReservedSize] = L'\0';
				CharStr buf = static_cast<CharStr>(this->operator new[](sizeof(Char) * NewReservedSize));
				CStr::Copy(str, buf);
				this->operator delete[](buf);
				str = buf;
				str[NewReservedSize] = L'\0';
				CurrentSize = ReservedSize = NewReservedSize;
				return *this;
			}
			//заміна першого входження символа Filler на \0 тим самим зменшуючи розмір строки
			BaseString & Shrink(Char Filler) {
				SizeT Pos = Find(Filler);
				if (Pos == -1)
					return *this;
				CurrentSize = Pos;
				str[Pos] = L'\0';
				return *this;
			}

			//Пошук символа з позиції StartPos до позиції EndPos
			SizeT Find(Char sym, SizeT StartPos = 0, SizeT EndPos = -1) noexcept {
				if (CurrentSize == -1) return -1;
				if (StartPos == -1) StartPos = 0; else if (StartPos > CurrentSize) return -1;
				if (EndPos == -1) EndPos = CurrentSize;  else if (EndPos < StartPos) return -1;
				for (SizeT i = StartPos; i < EndPos; i++)
					if (str[i] == sym) return i;
				return -1;
			}
			//Пошук символьного масиву з позиції StartPos до позиції EndPos
			SizeT Find(const CharStr Cstr, SizeT StartPos = 0, SizeT EndPos = -1) noexcept {
				if (CurrentSize == -1) return -1;
				if (StartPos == -1) StartPos = 0; else if (StartPos > CurrentSize) return -1;
				if (EndPos == -1) EndPos = CurrentSize;  else if (EndPos < StartPos) return -1;
				SizeT Size = CStr::Size(Cstr);
				if (Size > EndPos) return -1;
				for (SizeT i = StartPos; i < EndPos - Size; i++) {
					if (str[i] == Cstr[0]) {
						Boolean Checked = True;
						for (SizeT j = 1; j < Size; j++)
							if (str[i + j] != Cstr[i]) {
								Checked = False; break;
							}
						if (Checked) return i;
					}
				}
				return -1;
			}
			//Пощук підрядка в даному рядку з позиції StartPos до позиції EndPos
			SizeT Find(const BaseString & Str, SizeT StartPos = 0, SizeT EndPos = -1) noexcept {
				if (CurrentSize == -1) return -1;
				if (StartPos == -1) StartPos = 0; else if (StartPos > CurrentSize) return -1;
				if (EndPos == -1) EndPos = CurrentSize;  else if (EndPos < StartPos) return -1;
				if (EndPos > Str.CurrentSize) return -1;
				for (SizeT i = StartPos; i < EndPos - Str.CurrentSize; i++) {
					if (str[i] == Str.str[0]) {
						Boolean Checked = True;
						for (SizeT j = 1; j < Str.CurrentSize; j++)
							if (str[i + j] != Str.str[i]) {
								Checked = False; break;
							}
						if (Checked) return i;
					}
				}
				return -1;
			}

			//Розділення рядка по символу Delim з позиції StartPos до EndPos
			BaseList<Base, BaseString> Split(const Char Delim, SizeT StartPos = 0, SizeT EndPos = -1) const {
				if (this->str == nullptr)
					return BaseList<Base, BaseString>();
				if (this->CurrentSize == 0)
					return BaseList<Base, BaseString>();
				if (EndPos == -1) EndPos = this->CurrentSize;
				BaseList<Base, BaseString> list;
				SizeT StartPartPos = 0;
				Boolean flag = false;
				for (SizeT i = StartPos; i < EndPos; i++) {
					if (this->str[i] == Delim && flag == false) {
						list.Add(BaseString(this->str + StartPartPos, i - StartPartPos));
						flag = true;
					}
					if (this->str[i] != Delim && flag == true) {
						StartPartPos = i;
						flag = false;
					}
				}
				if (this->str[EndPos - 1] != Delim)
					list.Add(BaseString(this->str + StartPartPos, EndPos - StartPartPos));
				return list;
			}

			//Перетворення байтового масиву в строку з 16 бітних символів (0-9a-f)
			static BaseString<Base> binaryToHex(const Array<Byte> & binaryData) {
				//Bite <-> 2 hex digits
				union {
					//1 Byte (8 bits)
					const Byte * binary;
					//2 Hex digits (4x2 bits)
					const struct {
						//Secon hex (4 bits)
						Byte bit1 : 4;
						//First hex (4 bits)
						Byte bit0 : 4;
					} *Hex;
				} conv;
				if (binaryData.Count % 2 != 0)
					Throw(InvalidFormat(ExceptionPosition));
				auto CurrentSize = binaryData.Count * 2 * sizeof(Char) + 1;
				auto ReservedSize = CurrentSize + CurrentSettings::Memory::GetStringReserveSpace();
				CharStr ret = (CharStr)BaseString::operator new[](ReservedSize);
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

			//Конвертація в масив байт
			operator Array<Byte>() const {
				if (this->str == nullptr || this->CurrentSize == 0)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(this), NullPointException::Types::Read));
				CharStr data = (CharStr)BaseString::operator new[](this->CurrentSize);
				Memory::Copy(this->str, data, CurrentSize * sizeof(Char));
				return Array<Byte>((Byte*&&)data, this->CurrentSize * sizeof(Char));
			}

			//Конвертаця строки 16 значних символів в байтовий масив
			static Array<Byte> hexToBinary(const BaseString & stringData) {
				if (stringData.Size() % 2)
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(&stringData), NullPointException::Types::Read));
				SizeT Size = stringData.Size() / 2;
				SizeT i1 = 0, i2 = 0;
				Array<Byte> bytes(Size);
				//Bite <-> 2 hex digits
				union {
					//1 Byte (8 bits)
					Byte * binary;
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
					default:
						Throw(InvalidFormat(ExceptionPosition));
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
						Throw(InvalidFormat(ExceptionPosition));
					}
					i2++;
				}
				return bytes;
			}

			//Перетворення числа чи покажчика в hex рядок
			template<typename T>
			static BaseString toHexString(const T & Data) {
				return binaryToHex(Array<Byte>((const Byte*)(&Data), sizeof(T)));
			}

			//Заміна по таблиці символів
			//Може згенерувати помилку IndexOutOfRange, якщо
			//- StartPos більша ніж кількість символів в рядку
			//- StartPos більша ніж EndPos
			//TODO: Write Dictionary<TKey, TValue>
			//BaseString & Replace(
			//	Array<Pair<Char, Char>> ReplaceTable,
			//	SizeT StartPos = 0,
			//	SizeT EndPos = Seed::Math::Max::SizeT
			//) {
			//	if (StartPos > this->CurrentSize)
			//		Throw(IndexOutOfRange(ExceptionPosition, StartPos, this->CurrentSize));
			//	if (StartPos > EndPos)
			//		Throw(IndexOutOfRange(ExceptionPosition, StartPos, EndPos));
			//	if (EndPos > this->CurrentSize) EndPos = this->CurrentSize;
			//	for (SizeT i = StartPos; i < EndPos; i++) {
			//		for (SizeT j = 0; j < ReplaceTable.Count; j++) {
			//			if (this->str[i] == ReplaceTable[j].key)
			//				this->str[i] = ReplaceTable[j].value;
			//		}
			//	}
			//	return *this;
			//}

			//Заміна всіх входжень символа What на символ To
			//Може згенерувати помилку IndexOutOfRange, якщо
			//- StartPos більша ніж кількість символів в рядку
			//- StartPos більша ніж EndPos
			BaseString & Replace(
				Char What,
				Char To,
				SizeT StartPos = 0,
				SizeT EndPos = Seed::Math::Max<SizeT>()
			) {
				if (StartPos > this->CurrentSize)
					Throw(IndexOutOfRange(ExceptionPosition, StartPos, this->CurrentSize));
				if (StartPos > EndPos)
					Throw(IndexOutOfRange(ExceptionPosition, StartPos, EndPos));
				if (EndPos > this->CurrentSize) EndPos = this->CurrentSize;
				for (SizeT i = StartPos; i < EndPos; i++) {
					if (this->str[i] == What)
						this->str[i] = To;
				}
				return *this;
			}

			//Приведення всіх букв в заглавні
			BaseString & toUpcase() noexcept {
				for (SizeT i = 0; i < this->CurrentSize; i++)
					str[i] = CStr::ToUpcase(str[i]);
				return *this;
			}

			//Приведення всіх букв в нижній регістр
			BaseString & toDowncase() noexcept {
				for (SizeT i = 0; i < this->CurrentSize; i++)
					str[i] = CStr::ToDowncase(str[i]);
				return *this;
			}

			//Розширює рядок до розміру NewSize
			//заповнюючи елементи після поточного розміру символами Filler
			//Може згенерувати помилку IndexOutOfRange, якщо
			//- NewSize менший ніж кількість символів в рядку
			BaseString Extend(Char Filler, SizeT NewSize) {
				if (this->CurrentSize > NewSize)
					Throw(IndexOutOfRange(ExceptionPosition, NewSize, this->CurrentSize));
				if (this->CurrentSize == NewSize)
					return *this;
				BaseString newStr(NewSize);
				Memory::Copy(this->str, newStr.str, sizeof(Char) * this->CurrentSize);
				Memory::MemBlkFill(newStr.str + this->CurrentSize, (NewSize - 1) - this->CurrentSize, L' ');
				return newStr;
			}

			//Конвертор для строк з різним алокатором
			template<typename NewBase>
			operator BaseString<NewBase>() const {
				return BaseString<NewBase>((CharStr)*this);
			}

			//Оптимізований обмін рядками a і b
			template<typename T, template<typename> typename U>
			friend __forceinline void Swap(U<T> & a, U<T> & b) noexcept;

			//Деструктор
			__forceinline ~BaseString() {
				if (str == nullptr) return;
				BaseString::operator delete[](str);
				str = nullptr;
				CurrentSize = ReservedSize = 0;
			}
		};

		//Функція обміну для строк
		template<typename T, template<typename> typename U>
		__forceinline void Swap(U<T> & a, U<T> & b) noexcept {
			static_assert(StaticAsserrts::IsSame<BaseString<T>, U<T>>(), "Invalid template passed");
			Seed::Swap(a.str, b.str);
			Seed::Swap(a.CurrentSize, b.CurrentSize);
			Seed::Swap(a.ReservedSize, b.ReservedSize);
		}

		//Позначка переходу на новий рядок, використовується CRLF режим
		__forceinline const Char * const NextLine() {
			return L"\r\n";
		}

		//Простий рядок
		typedef BaseString<Memory::Heap> String;
		//Рядок для збереження конфіденційної інформації
		typedef BaseString<Memory::SensitiveDataHeap> SecuredString;
#pragma warning(pop)
	}
}