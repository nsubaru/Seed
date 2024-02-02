#pragma once
#include "Seed.Core.Containers.NonConcurency.DynamicQueue.h"

#pragma warning(push)
// This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)
#pragma warning(disable: 4172)

namespace Seed::Core::Containers
{
	template<typename THeap, typename T>
	BaseDynamicQueue<THeap, T>::~BaseDynamicQueue()
	{
		delete[] queue;
		front = rear = count = bufferSize = 0;
		queue = nullptr;
	}

	template<typename THeap, typename T>
	ForceInline T& BaseDynamicQueue<THeap, T>::GetByIdx(SizeT Index)
	{
		return queue[(front + Index) % bufferSize];
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseDynamicQueue<THeap, T>::GetByIdx(SizeT Index) const
	{
		return queue[(front + Index) % bufferSize];
	}

	template<typename THeap, typename T>
	ForceInline BaseDynamicQueue<THeap, T>::BaseDynamicQueue():
		BaseDynamicQueue(CurrentSettings().MemoryManager.QueueReserveSize)
	{
	}

	template<typename THeap, typename T>
	ForceInline BaseDynamicQueue<THeap, T>::BaseDynamicQueue(SizeT startCount):
		front(0),
		rear(0),
		count(0),
		queue(new T[startCount]),
		bufferSize(startCount)
	{
	}

	template<typename THeap, typename T>
	BaseDynamicQueue<THeap, T>::BaseDynamicQueue(const BaseDynamicQueue& original):
		front(original.front),
		rear(original.rear),
		count(original.count),
		queue(nullptr),
		bufferSize(original.bufferSize)
	{
		if (original.bufferSize != 0)
		{
			this->queue = new T[original.bufferSize];
		}
		else
		{
			return;
		}

		if (this->count == 0)
		{
			return;
		}

		if constexpr (!TypeTraits::IsStandardLayout<T>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				this->GetByIdx(i) = original.GetByIdx(i);
			}
		}
		else
		{
			Memory::Copy(original.queue, this->queue, sizeof(T) * bufferSize);
		}
	}

	template<typename THeap, typename T>
	BaseDynamicQueue<THeap, T>::BaseDynamicQueue(BaseDynamicQueue&& original):
		front(MoveRValue(original.front)),
		rear(MoveRValue(original.rear)),
		count(MoveRValue(original.count)),
		queue(MoveRValue(original.queue)),
		bufferSize(MoveRValue(original.bufferSize))
	{
		original.queue = nullptr;
		original.front = 0;
		original.rear = 0;
		original.count = 0;
		original.bufferSize = 0;
	}

	template<typename THeap, typename T>
	BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::operator=(const BaseDynamicQueue& original)
	{
		this->~BaseDynamicQueue();
		this->BaseDynamicQueue::BaseDynamicQueue(original);
		return *this;
	}

	template<typename THeap, typename T>
	BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::operator=(BaseDynamicQueue&& original)
	{
		this->~BaseDynamicQueue();
		this->BaseDynamicQueue::BaseDynamicQueue(MoveRValue(original));
		return *this;
	}

	template<typename THeap, typename T>
	BaseDynamicQueue<THeap, T>::BaseDynamicQueue(InitilizerList<T>&& list):
		front(0),
		rear(0),
		count(0),
		queue(nullptr),
		bufferSize(0)
	{
		if (list.size() == 0)
			return;

		SizeT actuallyCopied = list.size();
		bufferSize = list.size() > CurrentSettings().MemoryManager.QueueReserveSize ? list.size() : CurrentSettings().MemoryManager.QueueReserveSize;
		queue = new T[bufferSize];
		auto it = list.begin();
		for (SizeT i = 0; i < actuallyCopied; i++)
		{
			queue[i] = MoveRValue(*it); ++it;
		}
		rear = count = actuallyCopied;
		front = 0;
	}

	template<typename THeap, typename T>
	void BaseDynamicQueue<THeap, T>::Enqueue(const T& value, bool isResizeIfFull)
	{
		if (count >= bufferSize)
		{
			if (isResizeIfFull)
				this->ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
			else
				Throw(FullQueueException(SourcePosition, bufferSize));
		}

		SizeT newRear = (rear + 1) % bufferSize;
		queue[rear] = value;
		rear = newRear;
		++count;
	}

	template<typename THeap, typename T>
	void BaseDynamicQueue<THeap, T>::Enqueue(T&& value, bool isResizeIfFull)
	{
		if (count >= bufferSize)
		{
			if (isResizeIfFull)
				this->ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
			else
				Throw(FullQueueException(SourcePosition, bufferSize));
		}

		SizeT newRear = (rear + 1) % bufferSize;
		queue[rear] = MoveRValue(value);
		rear = newRear;
		++count;
	}

	template<typename THeap, typename T>
	T BaseDynamicQueue<THeap, T>::Dequeue()
	{
		if (count == 0)
			Throw(EmptyQueueException(SourcePosition));

		T res = MoveRValue(queue[front]);
		front = (front + 1) % bufferSize;
		--count;

		return MoveRValue(res);
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseDynamicQueue<THeap, T>::Top()
	{
		if (front == rear)
			Throw(EmptyQueueException(SourcePosition));

		return queue[front];
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseDynamicQueue<THeap, T>::Top() const
	{
		if (front == rear)
			Throw(EmptyQueueException(SourcePosition));

		return queue[front];
	}

	template<typename THeap, typename T>
	ForceInline void BaseDynamicQueue<THeap, T>::Extend(SizeT additionalSize)
	{
		ExtendImpl(additionalSize);
	}

	template<typename THeap, typename T>
	void BaseDynamicQueue<THeap, T>::Shrink()
	{
		const SizeT newSize = count;
		T* newBuffer = new T[newSize];
		for (SizeT i = 0; i < count; i++)
		{
			newBuffer[i] = GetByIdx(i);
		}
		front = 0; rear = count;
		delete[] queue;
		queue = newBuffer;
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseDynamicQueue<THeap, T>::GetCount() const
	{
		return count;
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseDynamicQueue<THeap, T>::GetMaxCount() const
	{
		return bufferSize;
	}

	template<typename THeap, typename T>
	Boolean BaseDynamicQueue<THeap, T>::GetIsFull() const
	{
		return count == bufferSize;
	}

	template<typename THeap, typename T>
	template<typename> requires (Concepts::IsClonable<T>)
	BaseDynamicQueue<THeap, T> BaseDynamicQueue<THeap, T>::Clone() const
	{
		auto item = BaseDynamicQueue(this->bufferSize);
		item.front = this->front;
		item.rear = this->rear;
		item.count = this->count;

		if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			Memory::Copy(this->queue, item.queue, sizeof(T) * this->bufferSize);
		}
		else
		{
			for (SizeT i = 0; i < count; i++)
			{
				item.GetByIdx(i) = Core::Clone(GetByIdx(i));
			}
		}

		return item;
	}

	template<typename THeap, typename T>
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseDynamicQueue<THeap, T> BaseDynamicQueue<THeap, T>::Clone(TLambda&& lambda) const
	{
		auto item = BaseDynamicQueue(this->count);
		item.front = this->front;
		item.rear = this->rear;
		item.count = this->count;

		for (SizeT i = 0; i < count; i++)
		{
			item.GetByIdx(i) = lambda(GetByIdx(i));
		}

		return item;
	}

	template<typename THeap, typename T>
	void BaseDynamicQueue<THeap, T>::ExtendImpl(SizeT additionalSize)
	{
		const SizeT newSize = bufferSize + additionalSize;
		T* newBuffer = nullptr;
		try
		{
			newBuffer = new T[newSize];
			for (SizeT i = 0; i < count; i++)
			{
				newBuffer[i] = GetByIdx(i);
			}
			front = 0; rear = count;
			Swap(queue, newBuffer);
			delete[] newBuffer; newBuffer = nullptr;
			bufferSize = newSize;
		}
		catch (...)
		{
			delete[] newBuffer; newBuffer = nullptr;
			Internals::Exceptions::__ReThrow();
		}

	}

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::ForEachImpl(TLambda&& lambda)
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
			TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{

			SizeT index = 0;

			for (SizeT i = 0; i < count; i++, index++)
			{
				lambda(GetByIdx(i), index);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
			__assume(false);
			return *this;
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < this->count; i++)
			{
				lambda(GetByIdx(i));
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			SizeT index = 0;

			for (SizeT i = 0; i < this->count; i++, index++)
			{
				lambda(GetByIdx(i), index);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
			__assume(false);
			return *this;
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT requiredCount)
	{
		if (requiredCount > this->count)
			requiredCount = this->count;

		if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
			TypeTraits::IsInvocable<TLambda, const T&>)
		{

			for (SizeT i = 0; i < requiredCount; i++)
			{
				lambda(GetByIdx(i));
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{

			SizeT index = 0;

			for (SizeT i = 0; i < requiredCount; i++, index++)
			{
				lambda(GetByIdx(i), index);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
			__assume(false);
			return *this;
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT requiredCount) const
	{
		if (requiredCount > this->count)
			requiredCount = this->count;

		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < requiredCount; i++)
			{
				lambda(GetByIdx(i));
			}
			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			SizeT index = 0;

			for (SizeT i = 0; i < requiredCount; i++, index++)
			{
				lambda(GetByIdx(i), index);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
			__assume(false);
			return *this;
		}
	}

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	U BaseDynamicQueue<THeap, T>::AggregateImpl(TLambda&& lambda, const U& value) const
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

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	U BaseDynamicQueue<THeap, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
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

	template<typename THeap, typename T>
	ForceInline bool BaseDynamicQueue<THeap, T>::AnyImpl() const
	{
		return count > 0;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	bool BaseDynamicQueue<THeap, T>::AnyImpl(TLambda&& lambda) const
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

	template<typename THeap, typename T>
	template<typename TLambda>
	bool BaseDynamicQueue<THeap, T>::AllImpl(TLambda&& lambda) const
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

	template<typename THeap, typename T>
	template<typename TLambda>
	T& BaseDynamicQueue<THeap, T>::FirstImpl(TLambda&& lambda)
	{
		if (count == 0)
			Internals::Exceptions::ThrowNotFound(SourcePosition);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				return GetByIdx(i);
		}

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T& BaseDynamicQueue<THeap, T>::LastImpl(TLambda&& lambda)
	{
		if (count == 0)
			Internals::Exceptions::ThrowNotFound(SourcePosition);

		SizeT idx = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				idx = i;
		}

		if (idx == Math::Max<SizeT>())
		{
			Internals::Exceptions::ThrowNotFound(SourcePosition);
		}

		return GetByIdx(idx);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const T& BaseDynamicQueue<THeap, T>::FirstImpl(TLambda&& lambda) const
	{
		if (count == 0)
			Internals::Exceptions::ThrowNotFound(SourcePosition);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				return GetByIdx(i);
		}

		Internals::Exceptions::ThrowNotFound(SourcePosition);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const T& BaseDynamicQueue<THeap, T>::LastImpl(TLambda&& lambda) const
	{
		if (count == 0)
			Internals::Exceptions::ThrowNotFound(SourcePosition);

		SizeT idx = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				idx = i;
		}

		if (idx == Math::Max<SizeT>())
		{
			Internals::Exceptions::ThrowNotFound(SourcePosition);
		}

		return GetByIdx(idx);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseDynamicQueue<THeap, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
	{
		if (count == 0)
			return MoveRValue(item);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				return GetByIdx(i);
		}

		return MoveRValue(item);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseDynamicQueue<THeap, T>::LastOrImpl(TLambda&& lambda, T&& item) const
	{
		if (count == 0)
			return MoveRValue(item);

		SizeT idx = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				idx = i;
		}

		if (idx == Math::Max<SizeT>())
		{
			return MoveRValue(item);
		}

		return GetByIdx(idx);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseDynamicQueue<THeap, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
	{
		if (count == 0)
			return item;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				return GetByIdx(i);
		}

		return item;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseDynamicQueue<THeap, T>::LastOrImpl(TLambda&& lambda, const T& item) const
	{
		if (count == 0)
			return item;

		SizeT idx = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				idx = i;
		}

		if (idx == Math::Max<SizeT>())
		{
			return item;
		}

		return GetByIdx(idx);
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseDynamicQueue<THeap, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				return GetByIdx(i);
		}

		return otherLambda();
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseDynamicQueue<THeap, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		SizeT idx = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				idx = i;
		}

		if (idx == Math::Max<SizeT>())
		{
			return otherLambda();
		}

		return GetByIdx(idx);
	}

	template<typename THeap, typename T>
	BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::ReverseImpl()
	{
		for (SizeT i = 0; i < count / 2; i++)
		{
			Swap(GetByIdx(i), GetByIdx((count - 1) - i));
		}
		return *this;
	}

	template<typename THeap, typename T>
	ForceInline void BaseDynamicQueue<THeap, T>::UnsafeSet(const T& value, SizeT index)
	{
		queue[index] = value;
		rear = index;
		count = bufferSize;
	}

	template<typename THeap, typename T>
	ForceInline void BaseDynamicQueue<THeap, T>::UnsafeSet(T&& value, SizeT index)
	{
		queue[index] = MoveRValue(value);
		rear = index;
		count = bufferSize;
	}

	template<typename THeap, typename T>
	ForceInline BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::RemoveAllImpl()
	{
		if (count == 0)
			return *this;

		delete[] queue;
		front = rear = count = bufferSize = 0;
		queue = nullptr;
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline BaseDynamicQueue<THeap, T>& BaseDynamicQueue<THeap, T>::RemoveAllImpl(TLambda&& lambda)
	{
		if (count == 0)
			return *this;

		for (SizeT i = 0; i < count; i++)
		{
			lambda(MoveRValue(GetByIdx(i)));
		}

		delete[] queue;
		front = rear = count = bufferSize = 0;
		queue = nullptr;
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	SizeT BaseDynamicQueue<THeap, T>::GetCountIfImpl(TLambda&& lambda) const
	{
		SizeT selectedCount = 0;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(GetByIdx(i)))
				selectedCount++;
		}

		return selectedCount;
	}
}

#pragma warning(pop)