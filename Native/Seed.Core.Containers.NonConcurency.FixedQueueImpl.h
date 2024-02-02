#pragma once
#include "Seed.Core.Containers.NonConcurency.FixedQueue.h"

#pragma warning(push)
#pragma warning(disable: 4172)
#pragma warning(disable: 4996)

namespace Seed::Core::Containers
{
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedQueue<THeap, TSize, T>::~BaseFixedQueue()
	{
		if constexpr (TypeTraits::IsSameV<THeap, Memory::SensitiveDataHeap> &&
			TypeTraits::IsStandardLayout<T>)
		{
			Memory::SecureZeroMemory(queue);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline T& BaseFixedQueue<THeap, TSize, T>::GetByIdx(SizeT Index)
	{
		return queue[(front + Index) % TSize];
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline const T& BaseFixedQueue<THeap, TSize, T>::GetByIdx(SizeT Index) const
	{
		return queue[(front + Index) % TSize];
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedQueue<THeap, TSize, T>::BaseFixedQueue(const BaseFixedQueue& original):
		front(original.front),
		rear(original.rear),
		count(original.count)
	{
		if (this->count != 0)
		{
			if constexpr (!TypeTraits::IsStandardLayout<T>)
			{
				for (SizeT i = 0; i < count; i++)
				{
					this->queue[i] = original.GetByIdx(i);
				}
			}
			else
			{
				Memory::Copy(original.queue, this->queue);
			}
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedQueue<THeap, TSize, T>::BaseFixedQueue(BaseFixedQueue&& original):
		front(original.front),
		rear(original.rear),
		count(original.count)
	{
		if (count != 0)
		{
			if constexpr (!TypeTraits::IsStandardLayout<T>)
			{
				for (SizeT i = 0; i < count; i++)
				{
					this->GetByIdx(i) = MoveRValue(original.GetByIdx(i));
				}
			}
			else
			{
				Memory::Copy(original.queue, this->queue);
			}
		}

		original.front = 0;
		original.rear = 0;
		original.count = 0;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedQueue<THeap, TSize, T>& BaseFixedQueue<THeap, TSize, T>::operator=(const BaseFixedQueue& original)
	{
		this->~BaseFixedQueue();
		this->BaseFixedQueue::BaseFixedQueue(original);
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedQueue<THeap, TSize, T>& BaseFixedQueue<THeap, TSize, T>::operator=(BaseFixedQueue&& original)
	{
		this->~BaseFixedQueue();
		this->BaseFixedQueue::BaseFixedQueue(MoveRValue(original));
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedQueue<THeap, TSize, T>::BaseFixedQueue(InitilizerList<T>&& list): BaseFixedQueue()
	{
		if (list.size() != 0)
		{
			SizeT actuallyCopied = (TSize < list.size()) ? TSize : list.size();
			auto it = list.begin();
			SizeT i = actuallyCopied;

			for (SizeT i = 0; i < actuallyCopied; i++)
			{
				queue[i] = MoveRValue(*it); it++;
			}

			rear = count = actuallyCopied;
			front = 0;
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedQueue<THeap, TSize, T>::Enqueue(const T& Value)
	{
		if (++count > TSize)
			Throw(FullQueueException(SourcePosition, TSize));

		SizeT newRear = (rear + 1) % TSize;
		queue[rear] = Value;
		rear = newRear;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedQueue<THeap, TSize, T>::Enqueue(T&& Value)
	{
		if (++count > TSize)
			Throw(FullQueueException(SourcePosition, TSize));

		SizeT newRear = (rear + 1) % TSize;
		queue[rear] = MoveRValue(Value);
		rear = newRear;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline T BaseFixedQueue<THeap, TSize, T>::Dequeue()
	{
		if (count == 0)
			Throw(EmptyQueueException(SourcePosition));

		T res = MoveRValue(queue[front]);
		front = (front + 1) % TSize;
		--count;
		return res;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline const T& BaseFixedQueue<THeap, TSize, T>::Top()
	{
		if (front == rear)
			Throw(EmptyQueueException(SourcePosition));

		return queue[front];
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline const T& BaseFixedQueue<THeap, TSize, T>::Top() const
	{
		if (front == rear)
			Throw(EmptyQueueException(SourcePosition));

		return queue[front];
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline SizeT BaseFixedQueue<THeap, TSize, T>::GetCount() const
	{
		return count;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename> requires (Concepts::IsClonable<T>)
	BaseFixedQueue<THeap, TSize, T> BaseFixedQueue<THeap, TSize, T>::Clone() const {
			BaseFixedQueue result;
			result.front = front;
			result.rear = rear;
			result.count = count;

			if constexpr (Concepts::IsStandardLayout<T>)
			{
				Memory::Copy(this->queue, result.queue);
			}
			else
			{
				for (SizeT i = 0; i < count; i++)
				{
					result.GetByIdx(i) = Core::Clone(GetByIdx(i));
				}
			}
			return result;
		}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseFixedQueue<THeap, TSize, T> BaseFixedQueue<THeap, TSize, T>::Clone(TLambda&& lambda) const
	{
		BaseFixedQueue result;
		result.front = front;
		result.rear = rear;
		result.count = count;

		for (SizeT i = 0; i < count; i++)
		{
			result.GetByIdx(i) = lambda(GetByIdx(i));
		}
		return result;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseFixedQueue<THeap, TSize, T>& BaseFixedQueue<THeap, TSize, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
			TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(GetByIdx(i));
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(GetByIdx(i), i);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const BaseFixedQueue<THeap, TSize, T>& BaseFixedQueue<THeap, TSize, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(GetByIdx(i));
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				lambda(GetByIdx(i), i);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename U, typename TLambda>
	U BaseFixedQueue<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		U result = value;
		U& data = result;

		if (count == 0)
		{
			return MoveRValue(result);
		}

		for (SizeT i = 0; i < count; i++)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				data = lambda(data, GetByIdx(i));
			}
			else
			{
				lambda(data, GetByIdx(i));
			}
		}

		return MoveRValue(result);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename U, typename TLambda>
	U BaseFixedQueue<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		U result = MoveRValue(value);

		if (count == 0)
			return MoveRValue(value);

		for (SizeT i = 0; i < count; i++)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				value = lambda(value, GetByIdx(i));
			}
			else
			{
				lambda(value, GetByIdx(i));
			}
		}

		return MoveRValue(value);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline bool BaseFixedQueue<THeap, TSize, T>::AnyImpl() const
	{
		return count > 0;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	bool BaseFixedQueue<THeap, TSize, T>::AnyImpl(TLambda&& lambda) const
	{
		if (count == 0)
			return false;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				return true;
		}

		return false;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	bool BaseFixedQueue<THeap, TSize, T>::AllImpl(TLambda&& lambda) const
	{
		if (count == 0)
			return false;

		for (SizeT i = 0; i < count; i++)
		{
			if (!lambda(GetByIdx(i)))
				return false;
		}

		return true;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T& BaseFixedQueue<THeap, TSize, T>::FirstImpl(TLambda&& lambda)
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				return GetByIdx(i);
			}
		}

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T& BaseFixedQueue<THeap, TSize, T>::LastImpl(TLambda&& lambda)
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		T* item = nullptr;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				item = &GetByIdx(i);
			}
		}

		if (item != nullptr)
		{
			return *item;
		}
		else
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const T& BaseFixedQueue<THeap, TSize, T>::FirstImpl(TLambda&& lambda) const
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				return GetByIdx(i);
			}
		}

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const T& BaseFixedQueue<THeap, TSize, T>::LastImpl(TLambda&& lambda) const
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		T* item = nullptr;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				item = &GetByIdx(i);
			}
		}

		if (item != nullptr)
		{
			return *item;
		}
		else
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedQueue<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, T&& element) const
	{
		if (count == 0)
			return MoveRValue(element);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				return GetByIdx(i);
			}
		}

		return MoveRValue(element);
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedQueue<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, T&& element) const
	{
		if (count == 0)
			return MoveRValue(element);

		const T* item = nullptr;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				item = &GetByIdx(i);
			}
		}

		if (item != nullptr)
		{
			return *item;
		}
		else
		{
			return MoveRValue(element);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedQueue<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, const T& element) const
	{
		if (count == 0)
			return element;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				return GetByIdx(i);
			}
		}

		return element;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedQueue<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, const T& element) const
	{
		if (count == 0)
			return element;

		const T* item = nullptr;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				item = &GetByIdx(i);
			}
		}

		if (item != nullptr)
		{
			return *item;
		}
		else
		{
			return MoveRValue(element);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseFixedQueue<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				return GetByIdx(i);
			}
		}

		return otherLambda();
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseFixedQueue<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		const T* item = nullptr;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
			{
				item = &GetByIdx(i);
			}
		}

		if (item != nullptr)
		{
			return *item;
		}
		else
		{
			return otherLambda();
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedQueue<THeap, TSize, T>& BaseFixedQueue<THeap, TSize, T>::ReverseImpl()
	{
		for (SizeT i = 0; i < count / 2; i++)
		{
			Seed::Core::Swap(GetByIdx(i), GetByIdx((count - 1) - i));
		}
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedQueue<THeap, TSize, T>::UnsafeSet(T&& Value, SizeT Index)
	{
		queue[Index] = MoveRValue(Value);
		count = Index + 1;
		rear = Index;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedQueue<THeap, TSize, T>::UnsafeSet(const T& Value, SizeT Index)
	{
		queue[Index] = Value;
		count = Index + 1;
		rear = Index;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedQueue<THeap, TSize, T>& BaseFixedQueue<THeap, TSize, T>::RemoveAllImpl()
	{
		if (count == 0)
			return *this;

		for (SizeT i = 0; i < count; i++)
		{
			GetByIdx(i).~T();
		}
		this->count = 0;
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseFixedQueue<THeap, TSize, T>& BaseFixedQueue<THeap, TSize, T>::RemoveAllImpl(TLambda&& lambda)
	{
		if (count == 0)
			return *this;

		for (SizeT i = 0; i < count; i++)
		{
			lambda(MoveRValue(GetByIdx(i)));
		}
		this->count = 0;
		return *this;
	}
}

#pragma warning(pop)