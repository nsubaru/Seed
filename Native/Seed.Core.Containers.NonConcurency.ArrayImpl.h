#pragma once
#include "Seed.Core.Containers.NonConcurency.Array.h"

#pragma warning(push)
#pragma warning(disable: 4172)
#pragma warning(disable: 6386)
#pragma warning(disable: 4996)

namespace Seed::Core::Containers
{
	template<typename THeap, typename T>
	template<bool TSortType, typename TLambda>
	inline void BaseArray<THeap, T>::QuickSort(T arr[], Int64 low, Int64 high, TLambda& lambda)
	{
		if (low < high)
		{
			Int64 pIndex = QuickSortPartition<TSortType>(arr, low, high, lambda);

			QuickSort<TSortType>(arr, low, pIndex - 1, lambda);
			QuickSort<TSortType>(arr, pIndex + 1, high, lambda);
		}
	}

	template<typename THeap, typename T>
	template<bool TSortType, typename TLambda>
	inline SizeT BaseArray<THeap, T>::QuickSortPartition(T arr[], Int64 low, Int64 high, TLambda& lambda)
	{
		Int64 pIndex = low;
		T pivot = arr[high];

		for (Int64 i = low; i < high; i++)
		{
			if constexpr (TSortType)
			{
				if (!lambda(arr[i], pivot))
				{
					Seed::Core::Swap((T&) arr[pIndex], (T&) arr[i]);
					pIndex++;
				}
			}
			else
			{
				if (!(lambda(arr[i]) < lambda(pivot)))
				{
					Seed::Core::Swap((T&) arr[pIndex], (T&) arr[i]);
					pIndex++;
				}
			}
		}

		Seed::Core::Swap((T&) arr[pIndex], (T&) arr[high]);

		return pIndex;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
						TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(array[i]);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
							TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(array[i], i);
			}
			return *this;
		}
		else
		{
			static_assert(
				TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>
				);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline const BaseArray<THeap, T>& BaseArray<THeap, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(array[i]);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(array[i], i);
			}
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT reqCount)
	{
		if (reqCount > this->Count)
			reqCount = this->Count;

		if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
						TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < reqCount; i++)
			{
				lambda(array[i]);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
							TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < reqCount; i++)
			{
				lambda(array[i], i);
			}
			return *this;
		}
		else
		{
			static_assert(
				TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>
				);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline const BaseArray<THeap, T>& BaseArray<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT reqCount) const
	{
		if (reqCount > this->Count)
			reqCount = this->Count;

		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < reqCount; i++)
			{
				lambda(array[i]);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < reqCount; i++)
			{
				lambda(array[i], i);
			}
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT startIndex, SizeT reqCount)
	{
		if (startIndex > this->count)
			Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, startIndex, this->count);

		if (reqCount == Math::Max<SizeT>())
			reqCount = this->count - startIndex;

		if (startIndex + reqCount > this->count)
			reqCount = this->count - startIndex;

		if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
						TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < reqCount; i++)
			{
				lambda(array[startIndex + i]);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
							TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < reqCount; i++)
			{
				lambda(array[startIndex + i], startIndex + i);
			}
			return *this;
		}
		else
		{
			static_assert(
				TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>
				);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline const BaseArray<THeap, T>& BaseArray<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT startIndex, SizeT reqCount) const
	{
		if (startIndex > this->count)
			Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, startIndex, this->count);

		if (reqCount == Math::Max<SizeT>())
			reqCount = this->count - startIndex;

		if (startIndex + reqCount > this->count)
			reqCount = this->count - startIndex;

		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < reqCount; i++)
			{
				lambda(array[startIndex + i]);
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < reqCount; i++)
			{
				lambda(array[startIndex + i], startIndex + i);
			}
			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	ForceInline U BaseArray<THeap, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		U result = value;
		U& data = result;
		for (SizeT i = 0; i < count; i++)
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

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	ForceInline U BaseArray<THeap, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		for (SizeT i = 0; i < count; i++)
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

	template<typename THeap, typename T>
	ForceInline bool BaseArray<THeap, T>::AnyImpl() const
	{
		return count > 0;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline bool BaseArray<THeap, T>::AnyImpl(TLambda&& lambda) const
	{
		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(array[i]))
				return true;
		}
		return false;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline bool BaseArray<THeap, T>::AllImpl(TLambda&& lambda) const
	{
		for (SizeT i = 0; i < count; i++)
		{
			if (!lambda(array[i]))
				return false;
		}
		return true;
	}

	template<typename THeap, typename T>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::SortImpl(bool IsDescending)
	{
		if (!IsDescending)
		{
			auto l = [] (const T& V1, const T& V2) { return V1 < V2; };
			QuickSort<true>(array, 0, count - 1, l);
		}
		else
		{
			auto l = [] (const T& V1, const T& V2) { return V1 > V2; };
			QuickSort<true>(array, 0, count - 1, l);
		}

		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::SortImpl(TLambda&& lambda)
	{
		if (count == 0)
			return *this;

		auto l = [lambda = MoveRValue(lambda)] (const T& v1, const T& v2) { return !lambda(v1, v2); };
		QuickSort<true>(array, 0, count - 1, l);

		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::SortByLambdaImpl(TLambda&& lambda)
	{
		if (count == 0)
			return *this;

		auto l = [lambda = MoveRValue(lambda)] (const T& v1, const T& v2) { return !lambda(v1, v2); };
		QuickSort<false>(array, 0, count - 1, l);

		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T& BaseArray<THeap, T>::FirstImpl(TLambda&& lambda)
	{
		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				return static_cast<T&>(array[i]);

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T& BaseArray<THeap, T>::LastImpl(TLambda&& lambda)
	{
		SizeT Index = -1;
		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != -1)
			return static_cast<T&>(array[Index]);

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline const T& BaseArray<THeap, T>::FirstImpl(TLambda&& lambda) const
	{
		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				return static_cast<T&>(array[i]);

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline const T& BaseArray<THeap, T>::LastImpl(TLambda&& lambda) const
	{
		SizeT Index = -1;
		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != -1)
			return static_cast<T&>(array[Index]);

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseArray<THeap, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
	{
		if (count == 0)
			return MoveRValue(item);

		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				return array[i];

		return MoveRValue(item);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseArray<THeap, T>::LastOrImpl(TLambda&& lambda, T&& item) const
	{
		if (count == 0)
			return MoveRValue(item);

		SizeT Index = -1;
		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != -1)
			return array[Index];

		return MoveRValue(item);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseArray<THeap, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
	{
		if (count == 0)
			return item;

		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				return array[i];

		return item;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseArray<THeap, T>::LastOrImpl(TLambda&& lambda, const T& item) const
	{
		if (count == 0)
			return item;

		SizeT Index = -1;
		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != -1)
			return array[Index];

		return item;
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseArray<THeap, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				return array[i];

		return otherLambda();
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseArray<THeap, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		SizeT Index = -1;
		for (SizeT i = 0; i < count; i++)
			if (lambda(array[i]))
				Index = i;

		if (Index != -1)
			return array[Index];

		return otherLambda();
	}

	template<typename THeap, typename T>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::ReverseImpl()&
	{
		for (SizeT i = 0; i < count / 2; i++)
		{
			Swap(array[i], array[(count - 1) - i]);
		}

		return *this;
	}

	template<typename THeap, typename T>
	ForceInline BaseArray<THeap, T> BaseArray<THeap, T>::ReverseImpl()&&
	{
		for (SizeT i = 0; i < count / 2; i++)
		{
			Swap(array[i], array[(count - 1) - i]);
		}

		return MoveRValue(*this);
	}

	template<typename THeap, typename T>
	ForceInline void BaseArray<THeap, T>::UnsafeSet(const T& Value, SizeT Index)
	{
		array[Index] = Value;
	}

	template<typename THeap, typename T>
	ForceInline void BaseArray<THeap, T>::UnsafeSet(T&& Value, SizeT Index)
	{
		array[Index] = MoveRValue(Value);
	}

	template<typename THeap, typename T>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::RemoveAllImpl()
	{
		delete[] this->array;
		this->array = nullptr;
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline BaseArray<THeap, T>& BaseArray<THeap, T>::RemoveAllImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsInvocable<TLambda, T&&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(MoveRValue(array[i]));
			}
			delete[] this->array;
			this->array = nullptr;
			this->count = 0;
			return *this;
		}
		else
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(MoveRValue(array[i]), i);
			}
			delete[] this->array;
			this->array = nullptr;
			this->count = 0;
			return *this;
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	SizeT BaseArray<THeap, T>::GetCountIfImpl(TLambda&& lambda) const
	{
		SizeT selectedCount = 0;
		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(array[i]))
				selectedCount++;
		}
		return selectedCount;
	}

	template<typename THeap, typename T>
	template<typename TLambda> requires (Concepts::IsInvocable<TLambda, const T&>)
	SizeT BaseArray<THeap, T>::IndexOfImpl(TLambda&& lambda) const
	{
		for (SizeT i = 0; i < this->count; i++)
		{
			if (lambda(this->array[i]))
				return i;
		}

		return Math::Max<SizeT>();
	}

	template<typename THeap, typename T>
	BaseArray<THeap, T>::BaseArray(InitilizerList<T>&& list): count(list.size()), array(nullptr)
	{
		if (list.size() != 0)
		{
			this->array = new T[list.size()];
			auto it = list.begin();
			for (SizeT i = 0; i < count; i++)
			{
				array[i] = MoveRValue(*it); it++;
			}
		}
	}

	template<typename THeap, typename T>
	BaseArray<THeap, T>::BaseArray(const BaseArray& original): count(original.count), array(nullptr)
	{
		if (count != 0)
		{
			this->array = new T[count];
			if constexpr (!(TypeTraits::IsStandardLayout<T> && TypeTraits::HasTriviaCopyCtor<T>))
			{
				for (SizeT i = 0; i < count; i++)
				{
					array[i] = original.array[i];
				}
			}
			else
			{
				Memory::Copy(original.array, this->array, sizeof(T) * count);
			}
		}
	}

	template<typename THeap, typename T>
	BaseArray<THeap, T>::BaseArray(BaseArray&& original):
		array(MoveRValue(original.array)),
		count(MoveRValue(original.count))
	{
		original.array = nullptr;
		original.count = 0;
	}

	template<typename THeap, typename T>
	ForceInline BaseArray<THeap, T>::BaseArray(SizeT startCount): count(0), array(nullptr)
	{
		if (startCount != 0)
		{
			array = new T[startCount];
			count = startCount;
		}
	}

	template<typename THeap, typename T>
	InternalApiCall BaseArray<THeap, T>::BaseArray(const T*& array, SizeT startCount): count(0), array(nullptr)
	{
		if (startCount != 0)
		{
			this->array = new T[startCount];
			count = startCount;

			if constexpr (TypeTraits::IsStandardLayout<T>)
			{
				Memory::Copy(array, this->array, startCount * sizeof(T));
			}
			else
			{
				for (SizeT i = 0; i < startCount; i++)
				{
					this->array[i] = array[i];
				}
			}
		}
	}

	template<typename THeap, typename T>
	InternalApiCall BaseArray<THeap, T>::BaseArray(T*&& array, SizeT startCount):
		count(startCount),
		array(MoveRValue(array))
	{
		array = nullptr;
	}

	template<typename THeap, typename T>
	template<SizeT TN>
	BaseArray<THeap, T>::BaseArray(const T(&array)[TN]):
		count(TN),
		array(nullptr)
	{
		this->array = new T[TN];

		if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			Memory::Copy((Char*) array, this->array, TN * sizeof(T));
		}
		else
		{
			for (SizeT i = 0; i < TN; i++)
			{
				this->array[i] = array[i];
			}
		}
	}

	template<typename THeap, typename T>
	BaseArray<THeap, T>& BaseArray<THeap, T>::operator=(const BaseArray& original)
	{
		this->~BaseArray();
		this->BaseArray::BaseArray(original);
		return *this;
	}

	template<typename THeap, typename T>
	BaseArray<THeap, T>& BaseArray<THeap, T>::operator=(BaseArray&& original)
	{
		this->~BaseArray();
		this->BaseArray::BaseArray(MoveRValue(original));
		return *this;
	}

	template<typename THeap, typename T>
	ForceInline InternalApiCall BaseArray<THeap, T>::operator const T* const() const
	{
		return array;
	}

	template<typename THeap, typename T>
	ForceInline T& BaseArray<THeap, T>::operator[](SizeT Index)&
	{
		if (Index < count)
		{
			return array[Index];
		}
		else
		{
			Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, Index, count);
		}
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseArray<THeap, T>::operator[](SizeT Index) const&
	{
		if (Index < count)
		{
			return array[Index];
		}
		else
		{
			Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, Index, count);
		}
	}

	template<typename THeap, typename T>
	ForceInline T&& BaseArray<THeap, T>::operator[](SizeT Index)&&
	{
		if (Index < count)
		{
			return MoveRValue(array[Index]);
		}
		else
		{
			Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, Index, count);
		}
	}

	template<typename THeap, typename T>
	BaseArray<THeap, T>& BaseArray<THeap, T>::Resize(SizeT NewSize)
	{
		if (this->array == nullptr || this->count == 0)
		{
			this->array = new T[NewSize];
			this->count = NewSize;
		}
		else
		{
			if (NewSize == 0)
			{
				delete[] array;
				count = 0;
			}
			else
			{
				T* buf = new T[NewSize];
				SizeT CopySize = (NewSize > count) ? count : NewSize;
				if constexpr (TypeTraits::IsStandardLayout<T>)
				{
					Memory::Copy(array, buf, CopySize * sizeof(T));
				}
				else
				{
					for (SizeT i = 0; i < CopySize; i++)
					{
						buf[i] = MoveRValue(array[i]);
					}
				}
				count = NewSize;
				delete[] array;
				array = buf;
			}
		}
		return *this;
	}

	template<typename THeap, typename T>
	template<typename> requires (Concepts::IsClonable<T>)
	BaseArray<THeap, T> BaseArray<THeap, T>::Clone() const
	{
		if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			return BaseArray((const T*&) (array), count);
		}
		else
		{
			auto newArray = BaseArray((const T*&) (this->array), count);
			for (SizeT i = 0; i < count; i++)
			{
				newArray.array[i] = this->array[i].Clone();
			}
			return newArray;
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseArray<THeap, T> BaseArray<THeap, T>::Clone(TLambda&& lambda) const
	{
		T* newArray = nullptr;
		try
		{
			newArray = new T[count];
			for (SizeT i = 0ui64; i < count; i++)
			{
				newArray[i] = lambda(newArray[i]);
			}

			return BaseArray(MoveRValue(newArray), count);
		}
		catch (...)
		{
			delete[] newArray;
			throw;
		}
	}

	template<typename THeap, typename T>
	bool BaseArray<THeap, T>::operator==(const BaseArray& other) const requires Core::Concepts::HaveEqual<T, T>
	{
		if (other.array == this->array)
			return true;

		if (other.count != this->count)
			return false;

		for (SizeT i = 0; i < this->count; i++)
		{
			if (!(other.array[i] == this->array[i]))
				return false;
		}

		return true;
	}

	template<typename THeap, typename T>
	bool BaseArray<THeap, T>::operator!=(const BaseArray& other) const requires Core::Concepts::HaveEqual<T, T>
	{
		return !(*this == other);
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseArray<THeap, T>::GetCount() const
	{
		return count;
	}

	template<typename THeap, typename T>
	ForceInline const T* const BaseArray<THeap, T>::GetRaw() const
	{
		return array;
	}

	template<typename THeap, typename T>
	BaseArray<THeap, T> BaseArray<THeap, T>::SubArray(SizeT startIdx, SizeT itemCount) const
	{
		if (itemCount == Math::Max<SizeT>())
		{
			itemCount = this->count - startIdx;
		}
		else if (this->count < itemCount + startIdx)
		{
			Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, itemCount + startIdx, this->count);
		}

		auto innerArray = BaseArray(itemCount);

		if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			Memory::Copy(this->array + startIdx, innerArray.array, itemCount * sizeof(T));
		}
		else
		{
			for (SizeT i = 0; i < itemCount; i++)
			{
				innerArray.array[i] = this->array[startIdx + i].Clone();
			}
		}

		return innerArray;
	}

	template<typename THeap, typename T>
	BaseArray<THeap, T>::~BaseArray()
	{
		delete[] array;
		array = nullptr;
		count = 0;
	}

	template<typename T> requires Concepts::IsStandardLayout<T>
	Blob AsBlob(const T& value)
	{
		union {
			const Byte* bytes[sizeof(T)];
			const T* valueRef;
		};

		valueRef = &value;

		return Blob((const Byte*&) (bytes), sizeof(T));
	}
}

namespace Seed::Core
{
	template<typename THeap, typename T>
	void Swap(Containers::BaseArray<THeap, T>& A, Containers::BaseArray<THeap, T> B)
	{
		Swap(A.This, B.This);
	}
}

namespace Seed::Core::Memory
{
	template<Concepts::IsStandardLayout T, SizeT count>
	ForceInline void Copy(const Containers::Blob& source, T(&destination)[count], SizeT offset)
	{
		Copy(((T*) (const T* const) source) + offset, destination, Min(source.Count - offset, (count * sizeof(T))));
	}

	template<Concepts::IsStandardLayout T, SizeT count>
	ForceInline void Copy(T(&source)[count], const Containers::Blob& destination, SizeT offset)
	{
		Copy(source, ((T*) (const T* const) destination) + offset, Min(destination.Count - offset, (count * sizeof(T))));
	}

	template<Concepts::IsStandardLayout T>
	ForceInline void Copy(const Containers::Blob& source, T destination, SizeT offset)
	{
		Copy(((T*) (const T* const) source) + offset, &destination, Min(source.Count - offset, sizeof(T)));
	}

	template<Concepts::IsStandardLayout T>
	ForceInline void Copy(T source, Containers::Blob& destination, SizeT offset)
	{
		Copy(source, ((T*) (const T* const) destination) + offset, Min(destination.Count - offset, (sizeof(T))));
	}
}

#pragma warning(pop)