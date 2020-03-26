#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Абстракція динамічної черги
				/// </summary>
				/// <typeparam name="TDynamicQueue">Тип динамічної черги</typeparam>
				/// <typeparam name="THeap">Тип кучі колекції</typeparam>
				/// <typeparam name="T">Тип елементів</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<typename TCircularBuffer, typename THeap, typename T, bool TIsConcurencySuport>
				class ICircularBuffer {
				private:
					using ThisCollection = TCircularBuffer;
				public:
					forceinline void Add(const T& item) {
						return static_cast<ThisCollection&>(*this).AddImpl(item);
					}
					forceinline void Add(T&& item) {
						return static_cast<ThisCollection&>(*this).AddImpl(MoveRValue(item));
					}
				public:
					forceinline ThisCollection& operator+=(const T& item) {
						Add(item);
						return static_cast<ThisCollection&>(*this);
					}

					forceinline ThisCollection& operator+=(T&& item) {
						Add(MoveRValue(item));
						return static_cast<ThisCollection&>(*this);
					}
				public:
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					forceinline T& GetBy(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).GetByImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					forceinline const T& GetBy(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).GetByImpl(MoveRValue(lambda));
					}
				public:
					forceinline void Flush() {
						return static_cast<ThisCollection&>(*this).FlushImpl();
					}
				public:
					forceinline SizeT GetActualSize() const {
						return static_cast<const ThisCollection&>(*this).GetActualSizeImpl();
					}
					forceinline Boolean GetIsEmpty() const {
						return static_cast<const ThisCollection&>(*this).GetIsEmptyImpl();
					}
					forceinline SizeT GetSize() const {
						return static_cast<const ThisCollection&>(*this).GetSizeImpl(); 
					}
					forceinline Boolean GetIsFull() const {
						return GetActualSize() == GetSize();
					}
				public:
					SizeT Property(GetActualSize) ActualSize;
					Boolean Property(GetIsEmpty) IsEmpty;
					Boolean Property(GetIsFull) IsFull;
					SizeT Property(GetSize) BufferSize;
				public:
					forceinline virtual ~ICircularBuffer() = 0 {};
				};
			}
		}
	}
}