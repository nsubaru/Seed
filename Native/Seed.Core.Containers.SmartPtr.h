#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers
		{
			namespace LINQ
			{
				class ISmartPtrGenericBase {
				protected:
					virtual ~ISmartPtrGenericBase() = 0 {}
				};
			}
		}
		/// <summary>
		/// Розумний покажчик, сам автоматично видаляє покажчик при зникненні посилань на об'єкт
		/// </summary>
		template<typename THeap, typename T>
		class BaseSmartPtr : protected IDisposable, public Containers::LINQ::ISmartPtrGenericBase, public Memory::Allocator<THeap> {
			struct Data : Memory::Allocator<THeap> {
				T* ptr;
				RefCountT RefCount;
				Data(T*&& ptr) : ptr(MoveRValue(ptr)), RefCount(1) {}
			};
		private:
			Data* This;
		protected:
			virtual void Dispose() override final { delete This->ptr; }
		public:
			/// <summary>
			/// Пустий конструктор ініціалізує покажчик невірним значення, 
			/// при використані такого об'єкт буде виникати NPE
			/// </summary>
			BaseSmartPtr() : This(nullptr) {}
			/// <summary>
			/// Конструктор з rvalue силки на покажчик
			/// </summary>
			/// <param name="ptr">Rvalue силки на покажчик</param>
			BaseSmartPtr(T*&& ptr) : This(new Data(MoveRValue(ptr))) {}
			
			/// <summary>
			/// Оператор розіменування
			/// </summary>
			/// <returns>Силка на об'єкт</returns>
			ForceInline T& operator*()
			{
				return *This->ptr;
			}

			/// <summary>
			/// Оператор розіменування
			/// </summary>
			/// <returns>Силка на об'єкт</returns>
			ForceInline const T& operator*() const
			{
				return *This->ptr;
			}

			/// <summary>
			/// Оператор доступу до полів\методів
			/// </summary>
			/// <returns>Покажчик на об'єкт</returns>
			ForceInline T* operator->()
			{
				return This->ptr;
			}

			/// <summary>
			/// Оператор доступу до полів\методів
			/// </summary>
			/// <returns>Покажчик на об'єкт</returns>
			ForceInline const T* operator->() const
			{
				return This->ptr;
			}

			/// <summary>
			/// Кількість силок на даний об'єкт
			/// </summary>
			/// <returns>Кількість силок</returns>
			ForceInline SizeT GetRefCount() const {
				return This->RefCount;
			}

			/// <summary>
			/// Тип значення
			/// </summary>
			/// <returns>Cxx RTTI об'єкт типу</returns>
			ForceInline decltype(auto) GetTypeInfo() const {
				return typeid(This->ptr);
			}

			/// <summary>
			/// Кількість силок
			/// </summary>
			SizeT Property(GetRefCount) RefCount;

		public:
			/// <summary>
			/// Порівняння з нульовим покажчиком
			/// </summary>
			/// <returns>Результат порівняння</returns>
			bool operator==(const std::nullptr_t&) const {
				return This->ptr == nullptr;
			}

			/// <summary>
			/// Порівняння з нульовим покажчиком
			/// </summary>
			/// <returns>Результат порівняння</returns>
			bool operator==(std::nullptr_t&&) const {
				return This->ptr == nullptr;
			}
		public:
			template<typename T>
            friend void Seed::Core::Swap(T& a, T& b);
		public:
			RefCountClass(BaseSmartPtr, This);
		};

		/// <summary>
        /// Розумний покажчик в простій кучі
        /// </summary>
        namespace {
            template<typename T>
            using SmartPtr = BaseSmartPtr<Memory::Heap, T>;
        }

        /// <summary>
        /// Розумний покажчик в захищеній кучі
        /// </summary>
        namespace {
            template<typename T>
            using SensitiveSmartPtr = BaseSmartPtr<Memory::SensitiveDataHeap, T>;
        }

		template<typename THeap, typename T>
        void Swap(BaseSmartPtr<THeap, T>& a, BaseSmartPtr<THeap, T>& b) {
            Swap(a.This, b.This);
        }
	}
}