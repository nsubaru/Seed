#pragma once

namespace std {
	enum class align_val_t : unsigned __int64
	{};
}

///<summary>Простір імен VCRT
///<para> По можливості уникати використання </para>
///</summary>
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
		namespace { using Int8 = __int8; }
		///<summary>16-ти бітна знакова числова змінна</summary>
		namespace { using Int16 = __int16; }
		///<summary>32-ти бітна знакова числова змінна</summary>
		namespace { using Int32 = __int32; }
		///<summary>64-ти бітна знакова числова змінна</summary>
		namespace { using Int64 = __int64; }

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

		static_assert(sizeof(char16_t) == sizeof(wchar_t), "Помилка розмір char16_t не є рівний wchar_t");
		///<summary>Символ в кодувані UTF-16LE</summary>
		namespace { using Char = wchar_t; using UChar = wchar_t; }
		///<summary>Символ в кодувані ASCII</summary>
		namespace { using AChar = char; }

		///<summary>Покажчик на масив символів в кодуванні UTF-16LE</summary>
		namespace { using CharStr = Char*; using UCharStr = UChar*; }
		///<summary>Покажчик на масив символів в кодуванні ASCII</summary>
		namespace { using ACharStr = AChar*; }

		///<summary>Покажчик на статичний масив символів в кодуванні ASCII</summary>
		namespace { using ConstACharStr = const AChar* const; }
		///<summary>Покажчик на статичний масив символів в кодуванні UTF-16LE</summary>
		namespace { using ConstCharStr = const Char* const; using UConstCharStr = UChar*; }

		///<summary>Логічний тип</summary>
		namespace { using Boolean = bool; }

		///<summary>Тип для ідентифікацій</summary>
		namespace { using ID = UInt64; }

		///<summary>Нуль тип для покажчиків</summary>
		namespace { using NullptrT = CRT::nullptr_t; }

		///<summary>Нульовий покажчик</summary>
		constexpr NullptrT NullPtr = nullptr;
		///<summary>Безтипний блок пам'яті</summary>
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
///<summary>Рядковий літерал для кодування UTF-16LE</summary>
///<param name='Text'>Покажчик на рядкову константу</param>
///<param name='Size'>Кількість елементів рядкового масиву</param>
///<returns>Константу CharStr</returns>
__forceinline constexpr const Seed::Core::CharStr const operator "" U(const wchar_t* const Text, const Seed::SizeT Size) { //-V2537
	using namespace Seed;
	return (const CharStr const)Text; //-V2533
}

__forceinline constexpr Seed::Core::Bytes operator "" Kb(unsigned long long Size) {
	using namespace Seed;
	return Size * 1024;
}

__forceinline constexpr Seed::Core::Bytes operator "" Mb(unsigned long long Size) {
	using namespace Seed;
	return Size * 1024Kb;
}

__forceinline constexpr Seed::Core::Bytes operator "" Gb(unsigned long long Size) {
	using namespace Seed;
	return Size * 1024Mb;
}
#pragma warning(pop)

#define interface __interface