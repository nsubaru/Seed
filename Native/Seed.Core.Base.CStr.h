#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Internals.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
    inline namespace Core {
        ///<summary>Простір імен функцій для роботи з C-подібними рядковими змінними</summary>
        namespace CStr {
        	/// <summary>
        	/// Compile-time функція визначення кількості символів в символьному масиві
        	/// </summary>
        	/// <param name="cStr">Рядкова зміна</param>
        	/// <returns>Кількість елементів в рядковому масиві</returns>
            __forceinline constexpr SizeT Size(const CharStr cStr) {
                SizeT i = 0;
                while (cStr[i] != '\0')
                {
                    i++;
                }
                return i;
            }

        	/// <summary>
        	/// Compile-time функція визначення кількості символів в символьному масиві
        	/// </summary>
        	/// <param name="cStr">Рядкова зміна</param>
        	/// <returns>Кількість елементів в рядковому масиві</returns>
            __forceinline constexpr SizeT Size(const Char* cStr) {
                SizeT i = 0;
                while (cStr[i] != '\0')
                {
                    i++;
                }
                return i;
            }


        	/// <summary>
        	/// Compile-time функція визначення кількості ASCII символів в символьному масиві
        	/// </summary>
        	/// <param name="cStr">Рядкова зміна</param>
        	/// <returns>Кількість елементів в рядковому масиві</returns>
            __forceinline constexpr SizeT Size(const AChar* cStr) {
                SizeT i = 0;
                while (cStr[i] != '\0')
                {
                    i++;
                }
                return i;
            }

        	/// <summary>
        	/// Функція копіювання с-подібних рядків. Не рекомендується так як без перевірок на межі масивів
        	/// </summary>
        	/// <param name="source">Рядкова зміна з якої копіюється</param>
        	/// <param name="dest">Рядкова зміна в яку копіюється</param>
            __declspec(deprecated("Без гарантій щодо виходу за межі"))
                __forceinline void Copy(CharStr source, CharStr dest) {
                while ((*dest++ = *source++)); //-V2507
            }

        	/// <summary>
        	/// Функція копіювання с-подібних рядків. Не рекомендується так як без перевірок на межі масивів
        	/// </summary>
        	/// <param name="source">Рядкова зміна з якої копіюється</param>
        	/// <param name="dest">Рядкова зміна в яку копіюється</param>
            __declspec(deprecated("Без гарантій щодо виходу за межі"))
                __forceinline void Copy(const Char* source, CharStr dest) {
                while ((*dest++ = *source++)); //-V2507
            }

        	/// <summary>
        	/// Функція копіювання с-подібних рядків. Не рекомендується так як без перевірок на межі масивів
        	/// </summary>
        	/// <param name="source">Рядкова зміна з якої копіюється</param>
        	/// <param name="dest">Рядкова зміна в яку копіюється</param>
            __declspec(deprecated("Без гарантій щодо виходу за межі"))
                __forceinline void Copy(const AChar* source, ACharStr dest) {
                while ((*dest++ = *source++));
            }

        	/// <summary>
        	/// Compile-time функція порівняння UTF16-LE с-подібних рядків
        	/// </summary>
        	/// <param name="str1">Перша рядкова змінна</param>
        	/// <param name="str2">Друга рядкова змінна</param>
        	/// <returns>True якщо всі символи ідентичні</returns>
            __forceinline constexpr Boolean Compare(const CharStr str1, const CharStr str2) {
                const auto str1Size = Size(str1);
                const auto str2Size = Size(str2);
                if (str1Size != str2Size) return false;
                for (SizeT i = 0; i < str1Size; i++)
                    if (str1[i] != str2[i]) return false;
                return true;
            }

        	/// <summary>
        	/// Compile-time функція порівняння UTF-16LE с-подібних рядків
        	/// </summary>
        	/// <param name="str1">Перша рядкова змінна</param>
        	/// <param name="str2">Друга рядкова змінна</param>
        	/// <returns>True якщо всі символи ідентичні</returns>
            __forceinline constexpr Boolean Compare(const Char* str1, const Char* str2) {
                const auto str1Size = Size(str1);
                const auto str2Size = Size(str2);
                if (str1Size != str2Size) return false;
                for (SizeT i = 0; i < str1Size; i++)
                    if (str1[i] != str2[i]) return false;
                return true;
            }

        	/// <summary>
        	/// Compile-time функція порівняння ASCII с-подібних рядків
        	/// </summary>
        	/// <param name="str1">Перша рядкова змінна</param>
        	/// <param name="str2">Друга рядкова змінна</param>
        	/// <returns>True якщо всі символи ідентичні</returns>
            __forceinline constexpr Boolean Compare(const ACharStr str1, const ACharStr str2) { //-V2558
                const auto str1Size = Size(str1);
                const auto str2Size = Size(str2);
                if (str1Size != str2Size) return false;
                for (SizeT i = 0; i < str1Size; i++)
                    if (str1[i] != str2[i]) return false;
                return true;
            }

        	/// <summary>
        	/// Compile-time функція порівняння ASCII с-подібних рядків
        	/// </summary>
        	/// <param name="str1">Перша рядкова змінна</param>
        	/// <param name="str2">Друга рядкова змінна</param>
        	/// <returns>True якщо всі символи ідентичні</returns>
            __forceinline constexpr Boolean Compare(const AChar* str1, const AChar* str2) {
                const auto str1Size = Size(const_cast<ACharStr>(str1)); //-V2533
                const auto str2Size = Size(const_cast<ACharStr>(str2)); //-V2533
                if (str1Size != str2Size) return false;
                for (SizeT i = 0; i < str1Size; i++)
                    if (str1[i] != str2[i]) return false;
                return true;
            }

            /// <summary>
            /// Перетворює символ у верхній регістр
            /// </summary>
            /// <param name="symbol">Символ який потрібно перетворити</param>
            /// <returns>Перетворений символ</returns>
            __forceinline Char ToUpcase(const Char symbol) {
                return Internals::UpcaseCharImp(symbol);
            }

            /// <summary>
            /// Перетворює символ у нижній регістр
            /// </summary>
            /// <param name="symbol">Символ який потрібно перетворити</param>
            /// <returns>Перетворений символ</returns>
            __forceinline Char ToDowncase(const Char symbol) {
               return Internals::DowncaseCharImp(symbol);
            }
        }
    }
}

#pragma warning(pop)