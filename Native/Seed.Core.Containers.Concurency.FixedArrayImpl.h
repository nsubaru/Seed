#pragma once
#include "Seed.Core.Containers.Concurency.FixedArray.h"

#pragma warning(push)
#pragma warning(disable: 4172)
#pragma warning(disable: 6386)
#pragma warning(disable: 4996)

namespace Seed::Core::Containers
{
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedArray<THeap, TSize, T>::~BaseConcurencyFixedArray()
	{
		if constexpr (TypeTraits::IsSameV<THeap, Memory::SensitiveDataHeap> &&
			TypeTraits::IsStandardLayout<T>)
		{
			Memory::SecureZeroMemory(array);
		}
	};
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline static void BaseConcurencyFixedArray<THeap, TSize, T>::QuickSort(T arr[], Int64 low, Int64 high, TLambda& lambda)
	{
		if (low < high)
		{
			Int64 pIndex = QuickSortPartition(arr, low, high, lambda);

			QuickSort(arr, low, pIndex - 1, lambda);
			QuickSort(arr, pIndex + 1, high, lambda);
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline static SizeT BaseConcurencyFixedArray<THeap, TSize, T>::QuickSortPartition(T arr[], Int64 low, Int64 high, TLambda& lambda)
	{
		Int64 pIndex = low;
		T pivot = arr[high];

		for (Int64 i = low; i < high; i++)
		{
			if (!lambda(arr[i], pivot))
			{
				Seed::Core::Swap((T&) arr[pIndex], (T&) arr[i]);
				pIndex++;
			}
		}

		Seed::Core::Swap((T&) arr[pIndex], (T&) arr[high]);

		return pIndex;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline SizeT BaseConcurencyFixedArray<THeap, TSize, T>::GetCount() const
	{
		return TSize;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->lock.SharedLock([&]()->BaseConcurencyFixedArray & InlineLambda{
				for (SizeT i = 0; i < TSize; i++)
				{
					lambda(this->array[i]);
				}
				return *this;
			});
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			return this->lock.SharedLock([&]()->BaseConcurencyFixedArray & InlineLambda{
				for (SizeT i = 0; i < TSize; i++)
				{
					lambda(this->array[i], i);
				}
				return *this;
			});
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
			__assume(0);
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->lock.SharedLock([&]() -> const BaseConcurencyFixedArray & InlineLambda{
				for (SizeT i = 0; i < TSize; i++)
				{
					lambda(this->array[i]);
				}
				return *this;
			});
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			return this->lock.SharedLock([&]() -> const BaseConcurencyFixedArray & InlineLambda{
				for (SizeT i = 0; i < TSize; i++)
				{
					lambda(this->array[i], i);
				}
				return *this;
			});
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
			__assume(0);
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename U, typename TLambda>
	U BaseConcurencyFixedArray<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		return MoveRValue(this->lock.SharedLock([&]() mutable InlineLambda{
			U result = value;
			U & data = result;
			for (SizeT i = 0; i < TSize; i++)
			{
				if constexpr (
					Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
					Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
				{
					data = lambda(data, this->array[i]);
				}
				else
				{
					lambda(data, this->array[i]);
				}
			}
			return MoveRValue(result);
		}));
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename U, typename TLambda>
	U BaseConcurencyFixedArray<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		return MoveRValue(this->lock.SharedLock([&, value = MoveRValue(value)]() mutable InlineLambda{
			for (SizeT i = 0; i < TSize; i++)
			{
				if constexpr (
					Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
					Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
				{
					value = lambda(value, this->array[i]);
				}
				else
				{
					lambda(value, this->array[i]);
				}
			}
			return MoveRValue(value);
		}));
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline bool BaseConcurencyFixedArray<THeap, TSize, T>::AnyImpl() const
	{
		return TSize > 0;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	bool BaseConcurencyFixedArray<THeap, TSize, T>::AnyImpl(TLambda&& lambda) const {
		return MoveRValue(this->lock.SharedLock([&]() InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
			{
				if (lambda(this->array[i]))
					return true;
			}
			return false;
		}));
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	bool BaseConcurencyFixedArray<THeap, TSize, T>::AllImpl(TLambda&& lambda) const {
		return MoveRValue(this->lock.SharedLock([&]() InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
			{
				if (!lambda(this->array[i]))
					return false;
			}
			return true;
		}));
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::SortByImpl(bool IsDescending) {
		return this->lock.ExclusiveLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
			if (!IsDescending) {
				auto lambda = [] (const T& v1, const T& v2) { return v1 < v2; };
				QuickSort(this->array, 0, TSize - 1, lambda);
			}
			else 
			{
				auto lambda = [] (const T& v1, const T& v2) { return v1 > v2; };
				QuickSort(this->array, 0, TSize - 1, lambda);
			}

			return *this;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::SortByLambdaImpl(TLambda&& lambda) {
		if constexpr (TSize == 0)
		{
			return *this;
		}
		else
		{
			return this->lock.ExclusiveLock([&]()->BaseConcurencyFixedArray & InlineLambda
			{
				auto l = [lambda = MoveRValue(lambda)] (const T& v1, const T& v2) { return !lambda(v1, v2); };
				QuickSort(this->array, 0, TSize - 1, l);

				return *this;
			});
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T& BaseConcurencyFixedArray<THeap, TSize, T>::FirstImpl(TLambda&& lambda) {
		return this->lock.SharedLock([&]() -> T& InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
				if (lambda(this->array[i]))
					return static_cast<T&>(this->array[i]);

			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T& BaseConcurencyFixedArray<THeap, TSize, T>::LastImpl(TLambda&& lambda) {
		return this->lock.SharedLock([&]() -> T& InlineLambda {
			SizeT Index = -1;
			for (SizeT i = 0; i < TSize; i++)
				if (lambda(this->array[i]))
					Index = i;

			if (Index != -1)
				return static_cast<T&>(this->array[Index]);

			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const T& BaseConcurencyFixedArray<THeap, TSize, T>::FirstImpl(TLambda&& lambda) const {
		return this->lock.SharedLock([&]() -> const T& InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
				if (lambda(this->array[i]))
					return static_cast<const T&>(this->array[i]);

			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const T& BaseConcurencyFixedArray<THeap, TSize, T>::LastImpl(TLambda&& lambda) const {
		return this->lock.SharedLock([&]() -> const T& InlineLambda {
			SizeT Index = -1;
			for (SizeT i = 0; i < TSize; i++)
				if (lambda(this->array[i]))
					Index = i;

			if (Index != -1)
				return static_cast<const T&>(this->array[Index]);

			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T BaseConcurencyFixedArray<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, T&& item) const {
		return this->lock.SharedLock([&, item = MoveRValue(item)]() InlineLambda {
			if (TSize == 0)
				return MoveRValue(item);

			for (SizeT i = 0; i < TSize; i++)
				if (lambda(this->array[i]))
					return this->array[i];

			return MoveRValue(item);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T BaseConcurencyFixedArray<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, T&& item) const {
		return this->lock.SharedLock([&, item = MoveRValue(item)]() InlineLambda {
			if (TSize == 0)
				return MoveRValue(item);

			SizeT Index = -1;
			for (SizeT i = 0; i < TSize; i++)
				if (lambda(this->array[i]))
					Index = i;

			if (Index != -1)
				return this->array[Index];

			return MoveRValue(item);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T BaseConcurencyFixedArray<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, const T& item) const {
		return this->lock.SharedLock([&]() InlineLambda {
			if (TSize == 0)
				return item;

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(this->array[i]))
				return this->array[i];

			return item;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T BaseConcurencyFixedArray<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, const T& item) const {
		return this->lock.SharedLock([&]() InlineLambda {
			if (TSize == 0)
				return item;

		SizeT Index = -1;
		for (SizeT i = 0; i < TSize; i++)
			if (lambda(this->array[i]))
				Index = i;

		if (Index != -1)
			return this->array[Index];

			return item;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	T BaseConcurencyFixedArray<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const {
		return this->lock.SharedLock([&]() InlineLambda {
			if (TSize == 0)
				return otherLambda();

			for (SizeT i = 0; i < TSize; i++)
				if (lambda(this->array[i]))
					return this->array[i];

			return otherLambda();
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	T BaseConcurencyFixedArray<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, TLambda&& otherLambda) const {
		return this->lock.SharedLock([&]() InlineLambda {
			if (TSize == 0)
				return otherLambda();

		SizeT Index = -1;
		for (SizeT i = 0; i < TSize; i++)
			if (lambda(this->array[i]))
				Index = i;

		if (Index != -1)
			return this->array[Index];

			return otherLambda();
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::ReverseImpl() {
		return this->lock.ExclusiveLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
			for (SizeT i = 0; i < TSize / 2; i++) {
				Seed::Core::Swap(static_cast<T&>(this->array[i]), static_cast<T&>(this->array[(TSize - 1) - i]));
			}
			return *this;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseConcurencyFixedArray<THeap, TSize, T>::UnsafeSet(const T& Value, SizeT Index)
	{
		this->array[Index] = Value;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseConcurencyFixedArray<THeap, TSize, T>::UnsafeSet(T&& Value, SizeT Index)
	{
		this->array[Index] = MoveRValue(Value);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::RemoveAllImpl() {
		return this->lock.ExclusiveLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
			for (SizeT i = 0; i < TSize; i++) {
				this->array[i].~T();
			}
			this->count = 0;
			return *this;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::RemoveAllImpl(TLambda&& lambda) {
		return this->lock.ExclusiveLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
			for (SizeT i = 0; i < TSize; i++) {
				lambda(MoveRValue(this->array[i]));
			}

			return *this;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda> requires (Concepts::IsInvocable<TLambda, const T&>)
	SizeT BaseConcurencyFixedArray<THeap, TSize, T>::IndexOfImpl(TLambda&& lambda) const {
		return lock.SharedLock([&]() InlineLambda{
			for (SizeT i = 0; i < TSize; i++)
			{
				if (lambda(this->array[i]))
					return i;
			}

			return Math::Max<SizeT>();
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseConcurencyFixedArray<THeap, TSize, T>::BaseConcurencyFixedArray(InitilizerList<T>&& list): lock()
	{
		if (list.size() != 0)
		{
			const SizeT actuallyCopied = (TSize < list.size()) ? TSize : list.size();
			auto it = list.begin();
			for (SizeT i = 0; i < actuallyCopied; i++)
			{
				this->array[i] = MoveRValue(*it); ++it;
			}
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseConcurencyFixedArray<THeap, TSize, T>::BaseConcurencyFixedArray(const T(&arr)[TSize]): lock()
	{
		if (TypeTraits::IsCompileTimeEvaluating())
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				array[i] = arr[i];
			}
		}
		else if constexpr (!TypeTraits::IsStandardLayout<T>)
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				array[i] = arr[i];
			}
		}
		else
		{
			Memory::Copy(arr, this->array);
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<SizeT USize>
	constexpr BaseConcurencyFixedArray<THeap, TSize, T>::BaseConcurencyFixedArray(const T(&arr)[USize], SizeT startPosition): lock()
	{
		for (SizeT i = 0; i < ((TSize < USize) ? TSize : USize); i++)
		{
			this->array[i] = arr[i + startPosition];
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseConcurencyFixedArray<THeap, TSize, T>::BaseConcurencyFixedArray(const BaseConcurencyFixedArray& original): lock()
	{
		original.lock.SharedLock([&] () {
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				for (SizeT i = 0; i < TSize; i++)
				{
					array[i] = original.array[i];
				}
			}
			else if constexpr (!TypeTraits::IsStandardLayout<T>)
			{
				for (SizeT i = 0; i < TSize; i++)
				{
					array[i] = original.array[i];
				}
			}
			else
			{
				Memory::Copy(original.array, this->array);
			}
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseConcurencyFixedArray<THeap, TSize, T>::BaseConcurencyFixedArray(BaseConcurencyFixedArray&& original): lock()
	{
		original.lock.ExclusiveLock([&] () {
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				for (SizeT i = 0; i < TSize; i++)
				{
					array[i] = MoveRValue(original.array[i]);
				}
			}
			else if constexpr (!TypeTraits::IsStandardLayout<T>)
			{
				for (SizeT i = 0; i < TSize; i++)
				{
					array[i] = MoveRValue(original.array[i]);
				}
			}
			else
			{
				Memory::Copy(original.array, this->array);
			}
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::operator=(const BaseConcurencyFixedArray& original)
	{
		this->~BaseConcurencyFixedArray();
		this->BaseConcurencyFixedArray::BaseConcurencyFixedArray(original);
		return *this;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseConcurencyFixedArray<THeap, TSize, T>& BaseConcurencyFixedArray<THeap, TSize, T>::operator=(BaseConcurencyFixedArray&& original)
	{
		this->~BaseConcurencyFixedArray();
		this->BaseConcurencyFixedArray::BaseConcurencyFixedArray(MoveRValue(original));
		return *this;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline constexpr T& BaseConcurencyFixedArray<THeap, TSize, T>::operator[](SizeT Index)
	{
		return this->lock.SharedLock([&]()->T & InlineLambda{
			if (Index < TSize)
			{
				return this->array[Index];
			}
			else
			{
				Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, Index, TSize);
			}
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline constexpr const T& BaseConcurencyFixedArray<THeap, TSize, T>::operator[](SizeT Index) const
	{
		return this->lock.SharedLock([&]() -> const T & InlineLambda{
			if (Index < TSize)
			{
				return this->array[Index];
			}
			else
			{
				Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, Index, TSize);
			}
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr bool BaseConcurencyFixedArray<THeap, TSize, T>::operator==(const BaseConcurencyFixedArray& other) const {
		return this->lock.SharedLock([&]() -> const T & InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
			{
				if (other.array[i] != this->array[i])
					return false;
			}
			return true;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr bool BaseConcurencyFixedArray<THeap, TSize, T>::operator!=(const BaseConcurencyFixedArray& other) const {
		return this->lock.SharedLock([&]() -> const T & InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
			{
				if (other.array[i] != this->array[i])
					return true;
			}
			return false;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr bool BaseConcurencyFixedArray<THeap, TSize, T>::operator==(const RawArray& other) const {
		return this->lock.SharedLock([&]() -> const T & InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
			{
				if (other[i] != this->array[i])
					return false;
			}
			return true;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr bool operator==(const typename BaseConcurencyFixedArray<THeap, TSize, T>::RawArray& left, const BaseConcurencyFixedArray<THeap, TSize, T>& right) {
		return this->lock.SharedLock([&]() -> const T & InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
			{
				if (left[i] != right.array[i])
					return false;
			}
			return true;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr bool BaseConcurencyFixedArray<THeap, TSize, T>::operator!=(const RawArray& other) const {
		return this->lock.SharedLock([&]() -> const T & InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
			{
				if (other[i] != this->array[i])
					return true;
			}
			return false;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr bool operator!=(const typename BaseConcurencyFixedArray<THeap, TSize, T>::RawArray& left, const BaseConcurencyFixedArray<THeap, TSize, T>& right) {
		return this->lock.SharedLock([&]() -> const T & InlineLambda {
			for (SizeT i = 0; i < TSize; i++)
			{
				if (left[i] != right.array[i])
					return true;
			}
			return false;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename> requires (Concepts::IsClonable<T>)
	constexpr BaseConcurencyFixedArray<THeap, TSize, T> BaseConcurencyFixedArray<THeap, TSize, T>::Clone() const
	{
		return this->lock.SharedLock([&]() InlineLambda {
			BaseConcurencyFixedArray result;
						
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				for (SizeT i = 0; i < TSize; i++)
				{
					result.array[i] = this->array[i];
				}
			}
			else if constexpr (TypeTraits::IsStandardLayout<T>) {
				Memory::Copy(this->array, result.array);
			}
			else {
				for (SizeT i = 0; i < TSize; i++)
				{
					result.array[i] = Core::Clone(this->array[i]);
				}
			}
			return result;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	constexpr BaseConcurencyFixedArray<THeap, TSize, T> BaseConcurencyFixedArray<THeap, TSize, T>::Clone(TLambda&& lambda) const
	{
		return this->lock.SharedLock([&]() InlineLambda {
			BaseConcurencyFixedArray result;

			for (SizeT i = 0; i < TSize; i++)
			{
				result.array[i] = lambda(this->array[i]);
			}
						
			return result;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedArray<THeap, TSize, T>::operator const RawArray() const
	{
		return const_cast<const RawArray>(this->array);
	}
}

#pragma warning(pop)