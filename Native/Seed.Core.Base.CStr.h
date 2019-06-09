#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
	inline namespace Core {
		///<summary>Простір імен функцій для роботи з C-подібними рядковими зміними</summary>
		namespace CStr {
#pragma inline_recursion(on)
#pragma inline_depth(256)
			///<summary>Compile-time функція визначення кількості сиволів в символьному масиві</summary>
			///<param name="CStr">Рядкова зміна</param>
			///<returns>Кількість елементів в рядковому масиві</returns>
			__forceinline constexpr SizeT Size(const CharStr CStr) noexcept {
				return *CStr == '\0' ? 0 : Size(CStr + 1) + 1;
			}

#pragma inline_recursion(on)
#pragma inline_depth(256)
			///<summary>Compile-time функція визначення кількості сиволів в символьному масиві</summary>
			///<param name="CStr">Рядкова зміна</param>
			///<returns>Кількість елементів в рядковому масиві</returns>
			__forceinline constexpr SizeT Size(const Char * CStr) noexcept {
				return *CStr == '\0' ? 0 : Size(CStr + 1) + 1;
			}

#pragma inline_recursion(on)
#pragma inline_depth(256)
			///<summary>Compile-time функція визначення кількості ASCII сиволів в символьному масиві</summary>
			///<param name="CStr">Рядкова зміна</param>
			///<returns>Кількість елементів в рядковому масиві</returns>
			__forceinline constexpr SizeT Size(const AChar * CStr) noexcept {
				return *CStr == '\0' ? 0 : Size(CStr + 1) + 1;
			}

			///<summary>Функція копіювання с-подібних рядків
			///<para>Позначена я не рекомендована</para>
			///</summary>
			///<param name="Source">Рядкова зміна з якої копіюється</param>
			///<param name="Dest">Рядкова зміна в яку копіюється</param>
			__declspec(deprecated("Без гарантій щодо виходу за межі"))
				__forceinline void Copy(CharStr Source, CharStr Dest) noexcept {
				while (*Dest++ = *Source++); //-V2507
			}

			///<summary>Функція копіювання с-подібних рядків
			///<para>Позначена я не рекомендована</para>
			///</summary>
			///<param name="Source">Рядкова зміна з якої копіюється</param>
			///<param name="Dest">Рядкова зміна в яку копіюється</param>
			__declspec(deprecated("Без гарантій щодо виходу за межі"))
				__forceinline void Copy(const Char * Source, CharStr Dest) noexcept {
				while (*Dest++ = *Source++); //-V2507
			}

			///<summary>Функція копіювання с-подібних рядків
			///<para>Позначена я не рекомендована</para>
			///</summary>
			///<param name="Source">Рядкова зміна з якої копіюється</param>
			///<param name="Dest">Рядкова зміна в яку копіюється</param>
			__declspec(deprecated("Без гарантій щодо виходу за межі"))
				__forceinline void Copy(const AChar * Source, ACharStr Dest) noexcept {
				while (*Dest++ = *Source++);
			}

			///<summary>Compile-time функція порівняння UTF16-LE с-подібних рядків</summary>
			///<param name="Str1">Перша рядкова змінна</param>
			///<param name="Str2">Друга рядкова змінна</param>
			///<returns>True якщо всі символи ідендисні</returns>
			__forceinline constexpr Boolean Compare(const CharStr Str1, const CharStr Str2) {
				SizeT Str1Size = Size(Str1);
				SizeT Str2Size = Size(Str2);
				if (Str1Size != Str2Size) return false;
				for (SizeT i = 0; i < Str1Size; i++)
					if (Str1[i] != Str2[i]) return false;
				return true;
			}

			///<summary>Compile-time функція порівняння UTF-16LE с-подібних рядків</summary>
			///<param name="Str1">Перша рядкова змінна</param>
			///<param name="Str2">Друга рядкова змінна</param>
			///<returns>True якщо всі символи ідендисні</returns>
			__forceinline constexpr Boolean Compare(const Char * Str1, const Char * Str2) {
				SizeT Str1Size = Size(Str1);
				SizeT Str2Size = Size(Str2);
				if (Str1Size != Str2Size) return false;
				for (SizeT i = 0; i < Str1Size; i++)
					if (Str1[i] != Str2[i]) return false;
				return true;
			}

			///<summary>Compile-time функція порівняння ASCII с-подібних рядків</summary>
			///<param name="Str1">Перша рядкова змінна</param>
			///<param name="Str2">Друга рядкова змінна</param>
			///<returns>True якщо всі символи ідендисні</returns>
			__forceinline constexpr Boolean Compare(const ACharStr Str1, const ACharStr Str2) {
				SizeT Str1Size = Size(Str1);
				SizeT Str2Size = Size(Str2);
				if (Str1Size != Str2Size) return false;
				for (SizeT i = 0; i < Str1Size; i++)
					if (Str1[i] != Str2[i]) return false;
				return true;
			}

			///<summary>Compile-time функція порівняння ASCII с-подібних рядків</summary>
			///<param name="Str1">Перша рядкова змінна</param>
			///<param name="Str2">Друга рядкова змінна</param>
			///<returns>True якщо всі символи ідендисні</returns>
			__forceinline constexpr Boolean Compare(const AChar * Str1, const AChar * Str2) {
				SizeT Str1Size = Size((ACharStr)Str1); //-V2533
				SizeT Str2Size = Size((ACharStr)Str2); //-V2533
				if (Str1Size != Str2Size) return false;
				for (SizeT i = 0; i < Str1Size; i++)
					if (Str1[i] != Str2[i]) return false;
				return true;
			}

			__forceinline Char ToUpcase(Char Symbol) {
				extern Seed::Core::Char UpcaseCharImp(Seed::Core::Char symb);
				return UpcaseCharImp(Symbol);
			}

			__forceinline Char ToDowncase(Char Symbol) {
				extern Seed::Core::Char DowncaseCharImp(Seed::Core::Char symb);
				return DowncaseCharImp(Symbol);
			}
		}
	}
}