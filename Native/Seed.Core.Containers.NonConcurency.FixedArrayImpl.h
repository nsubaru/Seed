#pragma once
#include "Seed.Core.Containers.NonConcurency.FixedArray.h"

#pragma warning(push)
#pragma warning(disable: 4172)
#pragma warning(disable: 6386)
#pragma warning(disable: 4996)

namespace Seed::Core::Containers
{
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline static void BaseFixedArray<THeap, TSize, T>::QuickSort(T arr[], Int64 low, Int64 high, TLambda& lambda)
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
	inline static SizeT BaseFixedArray<THeap, TSize, T>::QuickSortPartition(T arr[], Int64 low, Int64 high, TLambda& lambda)
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
	template<typename TLambda>
	ForceInline BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				lambda(array[i]);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				lambda(array[i], i);
			}
			return *this;
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
	ForceInline const BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				lambda(array[i]);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				lambda(array[i], i);
			}
			return *this;
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
	ForceInline T BaseFixedArray<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		U result = value;
		U& data = result;

		for (SizeT i = 0; i < TSize; i++)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				data = lambda(data, array[i]);
			}
			else
			{
				lambda(data, array[i]);
			}
		}
		return MoveRValue(result);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename U, typename TLambda>
	ForceInline U BaseFixedArray<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				value = lambda(value, array[i]);
			}
			else
			{
				lambda(value, array[i]);
			}
		}
		return MoveRValue(value);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline bool BaseFixedArray<THeap, TSize, T>::AnyImpl(TLambda&& lambda) const
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			if (lambda(array[i]))
				return true;
		}
		return false;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline bool BaseFixedArray<THeap, TSize, T>::AllImpl(TLambda&& lambda) const
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			if (!lambda(array[i]))
				return false;
		}
		return true;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::SortByImpl(bool IsDescending)
	{
		if (!IsDescending)
		{
			auto l = [] (const T& V1, const T& V2) { return V1 < V2; };
			QuickSort(array, 0, TSize - 1, l);
		}
		else
		{
			auto l = [] (const T& V1, const T& V2) { return V1 > V2; };
			QuickSort(array, 0, TSize - 1, l);
		}
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::SortByLambdaImpl(TLambda&& lambda)
	{
		if constexpr (TSize == 0)
		{
			return *this;
		}
		else
		{
			auto l = [lambda = MoveRValue(lambda)] (const T& v1, const T& v2) { return !lambda(v1, v2); };
			QuickSort(array, 0, TSize - 1, l);

			return *this;
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T& BaseFixedArray<THeap, TSize, T>::FirstImpl(TLambda&& lambda)
	{
		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				return static_cast<T&>(array[i]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T& BaseFixedArray<THeap, TSize, T>::LastImpl(TLambda&& lambda)
	{
		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != Math::Max<SizeT>())
			return static_cast<T&>(array[Index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline const T& BaseFixedArray<THeap, TSize, T>::FirstImpl(TLambda&& lambda) const
	{
		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				return static_cast<const T&>(array[i]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline const T& BaseFixedArray<THeap, TSize, T>::LastImpl(TLambda&& lambda) const
	{
		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != Math::Max<SizeT>())
			return static_cast<const T&>(array[Index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedArray<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
	{
		if (TSize == 0)
			return MoveRValue(item);

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				return array[i];

		return MoveRValue(item);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedArray<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, T&& item) const
	{
		if (TSize == 0)
			return MoveRValue(item);

		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != Math::Max<SizeT>())
			return array[Index];

		return MoveRValue(item);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedArray<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
	{
		if (TSize == 0)
			return item;

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				return array[i];

		return item;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedArray<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, const T& item) const
	{
		if (TSize == 0)
			return item;

		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != Math::Max<SizeT>())
			return array[Index];

		return item;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseFixedArray<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (TSize == 0)
			return otherLambda();

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				return array[i];

		return otherLambda();
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseFixedArray<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (TSize == 0)
			return otherLambda();

		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < TSize; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != Math::Max<SizeT>())
			return array[Index];

		return otherLambda();
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::ReverseImpl()
	{
		for (SizeT i = 0; i < TSize / 2; i++)
		{
			Swap(array[i], array[(TSize - 1) - i]);
		}
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedArray<THeap, TSize, T>::UnsafeSet(const T& Value, SizeT Index)
	{
		array[Index] = Value;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedArray<THeap, TSize, T>::UnsafeSet(T&& Value, SizeT Index)
	{
		array[Index] = MoveRValue(Value);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::RemoveAllImpl()
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			array[i].~T();
		}
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::RemoveAllImpl(TLambda&& lambda)
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			lambda(MoveRValue(array[i]));
		}
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda> requires (Concepts::IsInvocable<TLambda, const T&>)
	SizeT BaseFixedArray<THeap, TSize, T>::IndexOfImpl(TLambda&& lambda) const
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			if (lambda(this->array[i]))
				return i;
		}

		return Math::Max<SizeT>();
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseFixedArray<THeap, TSize, T>::BaseFixedArray(const BaseFixedArray& original): array()
	{
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
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseFixedArray<THeap, TSize, T>::BaseFixedArray(BaseFixedArray&& original): array()
	{
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
				this->array[i] = MoveRValue(original.array[i]);
			}
		}
		else
		{
			Memory::Copy(original.array, this->array);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::operator=(const BaseFixedArray& original)
	{
		this->~BaseFixedArray();
		this->BaseFixedArray::BaseFixedArray(original);
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseFixedArray<THeap, TSize, T>& BaseFixedArray<THeap, TSize, T>::operator=(BaseFixedArray&& original)
	{
		this->~BaseFixedArray();
		this->BaseFixedArray::BaseFixedArray(MoveRValue(original));
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr BaseFixedArray<THeap, TSize, T>::BaseFixedArray(InitilizerList<T>&& list): array()
	{
		if (list.size() != 0)
		{
			const SizeT actuallyCopied = (TSize < list.size()) ? TSize : list.size();
			auto it = list.begin();
			for (SizeT i = 0; i < actuallyCopied; i++)
			{
				array[i] = MoveRValue(*it); ++it;
			}
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline constexpr BaseFixedArray<THeap, TSize, T>::BaseFixedArray(const T(&arr)[TSize]): array()
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			array[i] = arr[i];
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<SizeT USize>
	constexpr BaseFixedArray<THeap, TSize, T>::BaseFixedArray(const T(&arr)[USize], SizeT startPosition): array()
	{
		for (SizeT i = 0; i < ((TSize < USize) ? TSize : USize); i++)
		{
			array[i] = arr[i + startPosition];
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline constexpr T& BaseFixedArray<THeap, TSize, T>::operator[](SizeT Index)
	{
		if (Index < TSize)
		{
			return array[Index];
		}
		else
		{
			Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, Index, TSize);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline constexpr const T& BaseFixedArray<THeap, TSize, T>::operator[](SizeT index) const
	{
		if (index < TSize)
		{
			return array[index];
		}
		else
		{
			Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, index, TSize);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr bool BaseFixedArray<THeap, TSize, T>::operator==(const BaseFixedArray& other) const
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			if (other.array[i] != this->array[i])
				return false;
		}
		return true;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	constexpr bool BaseFixedArray<THeap, TSize, T>::operator==(const RawArray& other) const
	{
		for (SizeT i = 0; i < TSize; i++)
		{
			if (other[i] != this->array[i])
				return false;
		}
		return true;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename> requires (Concepts::IsClonable<T>)
	constexpr BaseFixedArray<THeap, TSize, T> BaseFixedArray<THeap, TSize, T>::Clone() const
	{
		BaseFixedArray result;

		if (TypeTraits::IsCompileTimeEvaluating())
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				result.array[i] = array[i];
			}
		}
		else if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			Memory::Copy(array, result.array);
		}
		else
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				result.array[i] = Core::Clone(array[i]);
			}
		}

		return result;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	constexpr BaseFixedArray<THeap, TSize, T> BaseFixedArray<THeap, TSize, T>::Clone(TLambda&& lambda) const
	{
		BaseFixedArray result;

		for (SizeT i = 0; i < TSize; i++)
		{
			result.array[i] = lambda(array[i]);
		}

		return result;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	inline Byte* BaseFixedArray<THeap, TSize, T>::GetRaw()
	{
		return array;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	inline const Byte* BaseFixedArray<THeap, TSize, T>::GetRaw() const
	{
		return array;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedArray<THeap, TSize, T>::operator RawArray () const
	{
		return array;
	}
}

#pragma warning(pop)