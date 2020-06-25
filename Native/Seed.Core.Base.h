#pragma once
//Allow complex unnamed object, unnamed object often appear as iterator
//or when function have side-effect for example starting thread or task
#pragma warning(disable: 26444)

#include "Seed.Core.Base.Definitions.h"
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Interlocked.h"
#include "Seed.Core.Base.Math.h"
#include "Seed.Core.Base.Comparing.h"
#include "Seed.Core.Base.BitOperations.h"
#include "Seed.Core.Base.CStr.h"
#include "Seed.Core.Base.Swap.h"
#include "Seed.Core.Base.TypeTraits.h"
#include "Seed.Core.Base.Concepts.h"
#include "Seed.Core.Base.UUID.h"
#include "Seed.Core.Base.Internals.h"

#pragma warning(disable: 26439)

/// <summary>
/// Головний простір імен
/// </summary>
namespace Seed {
	/// <summary>
	/// Простір імен функцій ядра Рем
	/// </summary>
    inline namespace Core
    {
        /// <summary>
        /// Атомарний лічильник. Використовує функції InterlockedXXX
        /// </summary>
        class RefCountT final {
        public:
            using CounterType = SizeT;
        private:
            /// <summary>
            /// Містить значення
            /// </summary>
            volatile CounterType count;
        public:
            /// <summary>
            /// Гетер
            /// </summary>
            /// <returns>Значення лічильника</returns>
            SizeT GetValue() const {
                return count;
            }

            /// <summary>
            /// Поточне значення лічильника
            /// </summary>
            SizeT Property(GetValue) Value;

            /// <summary>
            /// Конструктор з SizeT
            /// </summary>
            /// <param name="count">Початкове значення</param>
            constexpr __forceinline RefCountT(SizeT count = 0) :
                count(count) {};

            /// <summary>
            /// Префіксний інкремент. Використовує функцію InterlockedIncrement для інкременту
            /// </summary>
            /// <returns>Вертає силку на елемент після інкременту</returns>
            __forceinline RefCountT& operator++() {
                Interlocked::Increment(static_cast<volatile SizeT&>(this->count));
                return *this;
            }

            /// <summary>
            /// Постфіксний інкремент. Використовує функцію InterlockedIncrement для інкременту
            /// </summary>
            /// <returns>Вертає копію елемента до інкременту</returns>
            __forceinline RefCountT operator++(int) {
                auto cp = *this;
                Interlocked::Increment(static_cast<volatile SizeT&>(this->count));
                return cp;
            }

            /// <summary>
            /// Префіксний декремент. Використовує функцію InterlockedDecrement для декременту
            /// </summary>
            /// <returns>Вертає силку на поточне значення</returns>
            __forceinline RefCountT& operator--() {
                Interlocked::Decrement(static_cast<volatile SizeT&>(this->count));
                return *this;
            }

            /// <summary>
            /// Постфіксний декремент. Використовує функцію InterlockedDecrementSize для декременту
            /// </summary>
            /// <returns>Вертає копію об'єкта до декременту</returns>
            __forceinline RefCountT operator--(int) {
                auto cp = *this;
                Interlocked::Decrement(static_cast<volatile SizeT&>(this->count));
                return cp;
            }

            /// <summary>
            /// Вертає значення в вигляді SizeT змінної
            /// </summary>
            /// <returns>Поточне значення об'єкта</returns>
            constexpr __forceinline operator SizeT() const {
                return this->count;
            }
        };

    	/// <summary>
    	/// Змінює порядок елементів в масиву Array
    	/// </summary>
    	/// <param name="Array">Масив</param>
    	/// <param name="size">Кількість елементів</param>
        template<typename T>
        DeprecatedCall("Робота з с-подібнимим масивами") __forceinline void Reverse(T* Array, SizeT size) {
            for (size_t i = 0; i < size / 2; i++) {
                Swap(Array[i], Array[(size - 1) - i]);
            }
        }

    	/// <summary>
    	/// Інтерфейс клонування
    	/// </summary>
        template<typename T>
        class IClonable  // NOLINT
        {
        public:
	        /// <summary>
        	/// Метод клонування
        	/// </summary>
        	/// <returns>Вертає повну копію об'єкта</returns>
            virtual T Clone() const = 0;
        };

    	/// <summary>
    	/// Інтерфейс звільнення ресурсів з класу з підрахунком силок
    	/// </summary>
        class IDisposable // NOLINT
        {
        protected:
        	/// <summary>
        	/// Метод для звільнення ресурсів, викликається деструктором автоматично в класів з підрахунком силок
        	/// </summary>
            virtual void Dispose() = 0;
        };

    	/// <summary>
    	/// Compile-time функція пошуку максимального серед двох об'єктів
    	/// </summary>
    	/// <typeparam name="T">Тип аргументів</typeparam>
    	/// <param name="v1">Перший аргумент</param>
    	/// <param name="v2">Другий аргумент</param>
    	/// <returns>Аргумент який більший</returns>
        template<typename T>
        __forceinline constexpr T Max(const T & v1, const T & v2) { return (v1 > v2) ? v1 : v2; }

    	/// <summary>
    	/// Compile-time функція пошуку мінімального серед двох об'єктів
    	/// </summary>
    	/// <typeparam name="T">Тип аргументів</typeparam>
    	/// <param name="v1">Перший аргумент</param>
    	/// <param name="v2">Другий аргумент</param>
    	/// <returns>Аргумент який менший</returns>
        template<typename T>
        __forceinline constexpr T Min(T v1, T v2) { return (v1 < v2) ? v1 : v2; }
    }
}

/// <summary>
/// Якщо RefCountClass вставлено в тіло класу в .h файлі то даних клас автоматично отримує підрахунок силок
/// </summary>
/// <param name = "ClassName">назва класу</param>
/// <param name = "This">покажчик в який поміщено дані екземпляра, має містити зміну RefCount типу RefCountT</param>
/// <remarks>
/// Define додає конструктор та оператор копіювання і переміщення та деструктор
/// </remarks>
#define RefCountClass(ClassName, This) \
/**<summary>Копіювальний конструктор</summary><param name = "Orig">Оригинал</param>*/ \
__forceinline ClassName(const ClassName & Orig) :This(Orig.This) { if(This != nullptr) ++This->RefCount; }; \
/**<summary>Конструктор переміщення</summary><param name = "Orig">Оригинал</param>*/ \
__forceinline ClassName(ClassName && From) : This(From.This) { From.This = nullptr; }; \
/**<summary>Оператор копіювання</summary><param name = "Orig">Оригинал</param><returns>Силка на поточний елемент</returns>*/ \
__forceinline ClassName & operator=(const ClassName & Orig){this->~ClassName(); this->ClassName::ClassName(Orig); return *this; }; \
/**<summary>Оператор переміщення</summary><param name = "Orig">Оригинал</param><returns>Силка на поточний елемент</returns>*/ \
__forceinline ClassName & operator=(ClassName && From){if(this == &From) return *this; this->~ClassName(); this->ClassName::ClassName(Seed::MoveRValue(From)); return *this; }; \
/**<summary>Деструктор</summary>*/ \
__forceinline virtual ~ClassName(){if (This != nullptr) {if (--This->RefCount == 0) {this->Dispose();delete This;}This = nullptr;}}

namespace Seed {
    inline namespace Core {
        namespace TypeTraits {

        }
    }
}