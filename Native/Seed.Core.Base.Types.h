#pragma once

// ReSharper disable once CppInconsistentNaming
namespace std {
	// ReSharper disable once CppInconsistentNaming
	enum class align_val_t : unsigned __int64
	{};
}

/// <summary>
/// Простір імен VCRT. По можливості уникати використання
/// </summary>
namespace CRT = std;

///<summary>Головний простір імен</summary>
namespace Seed {
	///<summary>Простір імен функцій ядра Рем</summary>
	inline namespace Core {
		///<summary>8-ми бітна беззнакова числова змінна</summary>
		namespace { using UInt8 = unsigned __int8; }
		///<summary>16-ти бітна беззнакова числова змінна</summary>
		namespace { using UInt16 = unsigned __int16; }
		///<summary>32-ти бітна беззнакова числова змінна</summary>
		namespace { using UInt32 = unsigned __int32; }
		///<summary>64-ти бітна беззнакова числова змінна</summary>
		namespace { using UInt64 = unsigned __int64; }

		///<summary>8-ми бітна знакова числова змінна</summary>
		namespace { using Int8 = signed __int8; }
		///<summary>16-ти бітна знакова числова змінна</summary>
		namespace { using Int16 = signed __int16; }
		///<summary>32-ти бітна знакова числова змінна</summary>
		namespace { using Int32 = signed __int32; }
		///<summary>64-ти бітна знакова числова змінна</summary>
		namespace { using Int64 = signed __int64; }

		///<summary>32-ох бітне дійсне число</summary>
		namespace { using Float = float; }
		///<summary>64-ох бітне дійсне число</summary>
		namespace { using Double = double; }

		///<summary>Один байт</summary>
		namespace { using Byte = unsigned char; }
		///<summary>Масив байтів</summary>
		namespace { using ByteArray = Byte*; }

		///<summary>Розмірний тип</summary>
		namespace { using SizeT = size_t; }

		///<summary>Кількість байт</summary>
		namespace { using Bytes = size_t; }

		static_assert(sizeof(char16_t) == sizeof(wchar_t), "Invalid wchar_t size. wchar_t must have same size as char16_t");
		///<summary>Символ в кодуванні UTF-16LE</summary>
		namespace { using Char = wchar_t; using UChar = wchar_t; }
		///<summary>Символ в кодуванні ASCII</summary>
		namespace { using AChar = char; }
		/// <summary> Символ в кодуванні UTF-8 </summary>
		namespace { using U8Char = char8_t; }
		/// <summary> Символ в кодуванні UTF-16 </summary>
		namespace { using U16Char = char16_t; }
		/// <summary> Символ в кодуванні UTF-32 </summary>
		namespace { using U32Char = char32_t; }

		///<summary>Покажчик на масив символів в кодуванні UTF-16LE</summary>
		namespace { using CharStr = Char*; using UCharStr = UChar*; }
		///<summary>Покажчик на масив символів в кодуванні ASCII</summary>
		namespace { using ACharStr = AChar*; }

		///<summary>Покажчик на статичний масив символів в кодуванні ASCII</summary>
		namespace { using ConstACharStr = const AChar* const; }
		///<summary>Покажчик на статичний масив символів в кодуванні UTF-16LE</summary>
		namespace { using ConstCharStr = const Char* const; using UConstCharStr = UChar*; }

		/// <summary>
		/// Силка на статичний рядок UTF-16 символів
		/// </summary>
		namespace
		{
			template<SizeT StrSize>
			using StaticTextRef = const wchar_t(&)[StrSize];
		}

		/// <summary>
		/// Силка на статичний рядок ASCII символів
		/// </summary>
		namespace
		{
			template<SizeT StrSize>
			using StaticATextRef = const char(&)[StrSize];
		}

		///<summary>Логічний тип</summary>
		namespace { using Boolean = bool; }

		///<summary>Тип для ідентифікацій</summary>
		namespace { using ID = UInt64; }

		///<summary>Нуль тип для покажчиків</summary>
		namespace { using NullptrT = decltype(nullptr); }

		///<summary>Нульовий покажчик</summary>
		constexpr NullptrT NullPtr = nullptr;
		///<summary>Без типовий блок пам'яті</summary>
		namespace { using RawMemBlk = void*; }

		///<summary>Дескриптор(невідомий тип)</summary>
		namespace { using Handle = void*; }

		///<summary>Тип мітка, який вказує що треба структуру\класс вирівнювати по певній границі</summary>
		namespace { using AllignRequirements = CRT::align_val_t; }

		///<summary>Деталь реалізації</summary>
		namespace { using ImplSpecificT = void*; }
	}
}

#pragma warning(push)
#pragma warning(disable: 4455)
#pragma warning(disable: 4114)

/// <summary>
/// Рядковий літерал для кодування UTF-16LE
/// </summary>
/// <param name="text">Покажчик на рядкову константу</param>
/// <param name="size">Кількість елементів рядкового масиву</param>
/// <returns>Константу CharStr</returns>
__forceinline constexpr Seed::Core::CharStr operator "" U(const wchar_t* const text, const Seed::SizeT size)
{ //-V2537
	using namespace Seed;
	return const_cast<const CharStr>(text); //-V2533
}

__forceinline constexpr Seed::SizeT operator "" Sz(const unsigned long long size) {
	using namespace Seed;
	return static_cast<Seed::SizeT>(size);
}

#define interface __interface
