#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		template<typename T>
		class SmartPtr : IDisposable {
			struct Data {
				T* ptr;
				RefCountT RefCount;
				Data(T*&& ptr) : ptr(MoveRValue(ptr)), RefCount(1) {}
			};
		private:
			Data* This;
		protected:
			virtual void Dispose() override final { delete This->ptr; }
		public:
			SmartPtr(T*&& ptr) : This(new Data(MoveRValue(ptr))) {}
			RefCountClass(SmartPtr, This);

			forceinline T& operator*()
			{
				return *This->ptr;
			}
			forceinline const T& operator*() const
			{
				return *This->ptr;
			}
			forceinline T* operator->()
			{
				return This->ptr;
			}
			forceinline const T* operator->() const
			{
				return This->ptr;
			}

			forceinline SizeT GetRefCount() const {
				return This->RefCount;
			}

			forceinline decltype(auto) GetTypeInfo() const {
				return typeid(This->ptr);
			}

			SizeT Property(GetRefCount) RefCount;
		};
	}
}