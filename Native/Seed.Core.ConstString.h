#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		class ConstAString;
		class ConstString;

		namespace Internals {
			namespace Exceptions {
				__declspec(noreturn) void ThrowNullPointException(
					const ConstAString& File,
					const ConstAString& FunctName,
					SizeT Line,
					SizeT Addr,
					UInt8 Type);

				__declspec(noreturn) void ThrowStringToLong(
					const Seed::ConstAString& File,
					const Seed::ConstAString& Function,
					const Seed::SizeT Line,
					const Seed::SizeT RequiredSize);

				__declspec(noreturn) void ThrowNullPointException(
					const Seed::ConstAString& File,
					const Seed::ConstAString& Function,
					const Seed::SizeT Line);

				__declspec(noreturn) void ThrowIntegerOverflowException(
					const Seed::ConstAString& File,
					const Seed::ConstAString& Function,
					const Seed::SizeT Line);

				__declspec(noreturn) void ThrowInvalidFormatException(
					const Seed::ConstAString& File,
					const Seed::ConstAString& Function,
					const Seed::SizeT Line);

				__declspec(noreturn) void ThrowIndexOutOfRange(
					const Seed::ConstAString& File,
					const Seed::ConstAString& Function,
					const Seed::SizeT Line,
					const Seed::SizeT Required,
					const Seed::SizeT MaxAllowed);

				__declspec(noreturn) void ThrowEndNodeNotDeferencable(
					const Seed::ConstAString& File,
					const Seed::ConstAString& Function,
					const Seed::SizeT Line);

				__declspec(noreturn) void ThrowNotImplemented(
					const Seed::ConstAString& File,
					const Seed::ConstAString& Function,
					const Seed::SizeT Line);

				__declspec(noreturn) void ThrowKeyAlredyExist(
					const Seed::ConstAString& File,
					const Seed::ConstAString& Function,
					const Seed::SizeT Line);
			}
		}

		///<summary>Константний ASCII рядок який пам'ятає свій розмір</summary>
		class ConstAString final {
		private:
			///<summary>Покажчик на текст</summary>
			const AChar* str;
			///<summary>Його розмір</summary>
			SizeT size;
		public:
			///<summary>Конструктор константного ASCII рядка</summary>
			///<param name = "Text">Рядковий літерал ASCII символів</param>
			///<param name = "Size">Його розмір</param>
			template<SizeT Size>
			constexpr ConstAString(const char(&Text)[Size]) : str(Text), size(Size) {}

			///<summary>Копіювальний конструктор</summary>
			///<param name = "orig">Оригинал константного рядка</param>
			constexpr ConstAString(const ConstAString& orig) : str(orig.str), size(orig.size) {} //-V730

			constexpr ConstAString& operator=(const ConstAString& orig) = default;

			///<summary>Оператор доступу до символів константного ASCII рядка</summary>
			///<param name = "Index">Індекс символа в рядку</param>
			const AChar& operator[](SizeT Index) const {
				if (Index >= size)
					Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, size);
				return str[Index];
			}

			///<summary>Вертає розмір константного ASCII рядка</summary>
			constexpr SizeT GetSize() const { return this->size; }

			///<summary>Розмір константного ASCII рядка</summary>
			SizeT Property(GetSize) Size;

			///<summary>Вертає константний масив символів цього константного ASCII рядка</summary>
			constexpr ConstACharStr GetString() const { return this->str; }

			///<summary>Константний масив символів цього константного ASCII рядка</summary>
			ConstACharStr Property(GetString) Str;
		};

		///<summary>Константний рядок який пам'ятає свій розмір</summary>
		class ConstString final {
		private:
			///<summary>Покажчик на текст</summary>
			const Char* str;
			///<summary>Його розмір</summary>
			SizeT size;
		public:
			///<summary>Конструктор константного рядка</summary>
			///<param name = "Text">Рядковий літерал</param>
			///<param name = "Size">Його розмір</param>
			template<SizeT Size>
			constexpr ConstString(const wchar_t(&Text)[Size]) : str(Text), size(Size) {}

			///<summary>Копіювальний конструктор</summary>
			///<param name = "orig">Оригинал константного рядка</param>
			constexpr ConstString(const ConstString& orig) : str(orig.str), size(orig.size) {} //-V730

			//<summary>Оператор копіюваювання</summary>
			///<param name = "orig">Оригинал константного рядка</param>
			ConstString& operator=(const ConstString& orig) = default;

			///<summary>Оператор доступу до символів константного рядка</summary>
			///<param name = "Index">Індекс символа в рядку</param>
			const Char& operator[](SizeT Index) const {
				if (Index >= size)
					Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, size);
				return str[Index];
			}

			///<summary>Вертає розмір константного рядка</summary>
			constexpr SizeT GetSize() const { return this->size; }

			///<summary>Розмір константного рядка</summary>
			SizeT Property(GetSize) Size;

			///<summary>Вертає константний масив символів цього константного рядка</summary>
			constexpr ConstCharStr GetString() const { return this->str; }

			///<summary>Константний масив символів цього константного рядка</summary>
			ConstCharStr Property(GetString) Str;
		};
	}
}