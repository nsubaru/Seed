#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"

namespace Seed
{
    inline namespace Core
    {
		namespace Containers
		{
			namespace LINQ
			{
				class ILazyGenericBase {
				protected:
					ForceInline virtual ~ILazyGenericBase() = 0 {}
				};
			}
		}

		/// <summary>
		/// Лінивий об'єкт
		/// </summary>
        template<typename THeap, typename T>
        class BaseLazy final : public Memory::Allocator<THeap>, IDisposable, public Containers::LINQ::ILazyGenericBase { //-V1052
        public:
            using LazyGetter = T(*)();
        private:
            struct Data : public Memory::Allocator<THeap> {
                LazyGetter getter;
                T* value;
                RefCountT RefCount;

                Data(LazyGetter getter) :
                    getter(getter), value(nullptr), RefCount(1) {}

				Data(T*&& value) :
                    getter(nullptr), value(value), RefCount(1) {}
            };
        private:
            Data* This;
		private:
			void CheckIsExistOrGetNew() const {
				if (This->value == nullptr)
				{
					This->value = new T(This->getter());
				}
			}
		protected:
			virtual void Dispose() override final {
				delete This->value;
			}
        public:
			/// <summary>
			/// Конструктор лінивого об'єкта
			/// </summary>
			/// <param name="getter">Геттер який викличеться при спробі отримати запис</param>
            BaseLazy(LazyGetter getter) : This(new Data(getter)) {
                if (getter == nullptr)
                {
                    Throw(NullPointException(ExceptionPosition));
                }
            }

			/// <summary>
			/// Конструктор лінивого об'єкта
			/// </summary>
			/// <param name="val">Значення об'єкта</param>
            BaseLazy(const T& val) : This(new Data(new T(val))) {}

			/// <summary>
			/// Конструктор лінивого об'єкта
			/// </summary>
			/// <param name="val">Значення об'єкта</param>
            BaseLazy(T&& val) : This(new Data(new T(MoveRValue(val)))) {}

			/// <summary>
			/// Пустий конструктор лінивого об'єкта, якщо об'єкт можна створити конструктором без аргументів
			/// </summary>
            BaseLazy() requires Concepts::IsDefaultConstructible<T>
				: BaseLazy(ArrowFunct(T())) {}
        public:
            /// <summary>
			/// Оператор розіменування
			/// </summary>
			/// <returns>Силка на об'єкт</returns>
			ForceInline T& operator*() &
			{
				CheckIsExistOrGetNew();
                return *This->value;
			}

			/// <summary>
			/// Оператор розіменування
			/// </summary>
			/// <returns>Силка на об'єкт</returns>
			ForceInline const T& operator*() const &
			{
				CheckIsExistOrGetNew();
				return *This->value;
			}

			/// <summary>
			/// Оператор доступу до полів\методів
			/// </summary>
			/// <returns>Покажчик на об'єкт</returns>
			ForceInline T* operator->() &
			{
				CheckIsExistOrGetNew();
				return This->value;
			}

			/// <summary>
			/// Оператор доступу до полів\методів
			/// </summary>
			/// <returns>Покажчик на об'єкт</returns>
			ForceInline const T* operator->() const &
			{
				CheckIsExistOrGetNew();
				return This->value;
			}

            /// <summary>
			/// Оператор розіменування
			/// </summary>
			/// <returns>Силка на об'єкт</returns>
			ForceInline T&& operator*() &&
			{
				CheckIsExistOrGetNew();
				T value = MoveRValue(*This->value);
				delete This->value;
				This->value = nullptr;
                return MoveRValue(value);
			}

			/// <summary>
			/// Оператор доступу до полів\методів
			/// </summary>
			/// <returns>Покажчик на об'єкт</returns>
			ForceInline T*&& operator->() && //-V659
			{
				CheckIsExistOrGetNew();
				T* value = MoveRValue(This->value);
				This->value = nullptr;
                return MoveRValue(value);
			}
		public:
			template<typename T>
            friend void Seed::Core::Swap(T& a, T& b);
		public:
			RefCountClass(BaseLazy, This);
        };

		/// <summary>
        /// Лінивий об'єкт в простій кучі
        /// </summary>
        namespace {
            template<typename T>
            using Lazy = BaseLazy<Memory::Heap, T>;
        }

        /// <summary>
        /// Лінивий об'єкт в захищеній кучі
        /// </summary>
        namespace {
            template<typename T>
            using SensitiveLazy = BaseLazy<Memory::SensitiveDataHeap, T>;
        }

		template<typename THeap, typename T>
        void Swap(BaseLazy<THeap, T>& a, BaseLazy<THeap, T>& b) {
            Swap(a.This, b.This);
        }
    }
}