#pragma once
#include "Seed.Core.Containers.Concurency.DynamicQueue.h"

#pragma warning(push)
// This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

#pragma warning(disable: 4172)

namespace Seed::Core::Containers
{
	template<typename THeap, typename T>
	ForceInline T& BaseConcurencyDynamicQueue<THeap, T>::GetByIdx(SizeT index)
	{
		return queue[(front + index) % bufferSize];
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseConcurencyDynamicQueue<THeap, T>::GetByIdx(SizeT index) const
	{
		return queue[(front + index) % bufferSize];
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicQueue<THeap, T>::~BaseConcurencyDynamicQueue()
	{
		delete[] queue;
		front = rear = count = bufferSize = 0;
		queue = nullptr;
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicQueue<THeap, T>::BaseConcurencyDynamicQueue(InitilizerList<T>&& list):
		front(0),
		rear(0),
		count(0),
		queue(nullptr),
		bufferSize(0),
		flowController()
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
	BaseConcurencyDynamicQueue<THeap, T>::BaseConcurencyDynamicQueue(const BaseConcurencyDynamicQueue& original):
		front(0),
		rear(0),
		count(0),
		queue(nullptr),
		bufferSize(0),
		flowController()
	{
		original.flowController.SharedLock([&] () {
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
		});
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicQueue<THeap, T>::BaseConcurencyDynamicQueue(BaseConcurencyDynamicQueue&& queue): front(0), rear(0), queue(nullptr), bufferSize(0)
	{
		queue.flowController.ExclusiveLock([&] () mutable {
			this->front = queue.front;
			queue.front = 0;
			this->count = queue.count;
			queue.count = 0;
			this->rear = queue.rear;
			queue.rear = 0;
			this->bufferSize = queue.bufferSize;
			queue.bufferSize = 0;
			this->queue = queue.queue;
			queue.queue = nullptr;
		});
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::operator=(const BaseConcurencyDynamicQueue& original)
	{
		this->~BaseConcurencyDynamicQueue();
		this->BaseConcurencyDynamicQueue::BaseConcurencyDynamicQueue(original);
		return *this;
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::operator=(BaseConcurencyDynamicQueue&& original)
	{
		this->~BaseConcurencyDynamicQueue();
		this->BaseConcurencyDynamicQueue::BaseConcurencyDynamicQueue(MoveRValue(original));
		return *this;
	}

	template<typename THeap, typename T>
	void BaseConcurencyDynamicQueue<THeap, T>::Enqueue(const T& value, bool isResizeIfFull) {
		this->flowController.Produce([&]() InlineLambda {
			if (this->count >= this->bufferSize) {
				if (!isResizeIfFull)
					this->flowController.WaitForConsumer();
				else
					ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
			}

			SizeT newRear = (this->rear + 1) % this->bufferSize;
			this->queue[this->rear] = value;
			this->rear = newRear;
			++this->count;
		});
	}

	template<typename THeap, typename T>
	void BaseConcurencyDynamicQueue<THeap, T>::Enqueue(T&& value, bool isResizeIfFull) {
		this->flowController.Produce([&, value = MoveRValue(value)]() mutable InlineLambda {
			if (this->count >= this->bufferSize) {
				if (!isResizeIfFull)
					this->flowController.WaitForConsumer();
				else
					ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
			}

			SizeT newRear = (this->rear + 1) % this->bufferSize;
			this->queue[this->rear] = MoveRValue(value);
			this->rear = newRear;
			++this->count;
		});
	}

	template<typename THeap, typename T>
	T BaseConcurencyDynamicQueue<THeap, T>::Dequeue() {
		return this->flowController.Consume([&]() InlineLambda {
			while (this->count == 0) this->flowController.WaitForProducer();

			T res = MoveRValue(this->queue[this->front]);
			this->front = (this->front + 1) % this->bufferSize;
			--this->count;

			return MoveRValue(res);
		});
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseConcurencyDynamicQueue<THeap, T>::Top()
	{
		return this->flowController.SharedLock([&]() -> const T & InlineLambda{
			if (this->front == this->rear)
				Throw(EmptyQueueException(SourcePosition));

			return this->queue[this->front];
		});
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseConcurencyDynamicQueue<THeap, T>::Top() const
	{
		return this->flowController.SharedLock([&]() -> const T & InlineLambda{
			if (this->front == this->rear)
				Throw(EmptyQueueException(SourcePosition));

			return this->queue[this->front];
		});
	}

	template<typename THeap, typename T>
	ForceInline void BaseConcurencyDynamicQueue<THeap, T>::Extend(SizeT AdditionalSize)
	{
		this->flowController.ExclusiveLock([&]() InlineLambda{
			ExtendImpl(AdditionalSize);
		});
	}

	template<typename THeap, typename T>
	void BaseConcurencyDynamicQueue<THeap, T>::Shrink()
	{
		this->flowController.ExclusiveLock([&]() InlineLambda{
			const SizeT newSize = this->count;
			T * newBuffer = new T[newSize];
			for (SizeT i = 0; i < this->count; i++)
			{
				newBuffer[i] = GetByIdx(i);
			}
			this->front = 0; this->rear = this->count;
			delete[] this->queue;
			this->queue = newBuffer;
		});
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseConcurencyDynamicQueue<THeap, T>::GetCount() const
	{
		return this->count;
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseConcurencyDynamicQueue<THeap, T>::GetMaxCount() const
	{
		return this->bufferSize;
	}

	template<typename THeap, typename T>
	Boolean BaseConcurencyDynamicQueue<THeap, T>::GetIsFull() const
	{
		return this->count == this->bufferSize;
	}

	template<typename THeap, typename T>
	template<typename> requires (Concepts::IsClonable<T>)
	BaseConcurencyDynamicQueue<THeap, T> BaseConcurencyDynamicQueue<THeap, T>::Clone() const
	{
		return this->flowController.SharedLock([&] () -> BaseConcurencyDynamicQueue InlineLambda {
			auto item = BaseConcurencyDynamicQueue(this->count);

			if constexpr (Concepts::IsStandardLayout<T>)
			{
				Memory::Copy(static_cast<const void*>(this->queue), static_cast<void*>(item.queue), this->count * sizeof(T));
			}
			else
			{
				for (SizeT i = 0; i < this->count; i++)
				{
					item.GetByIdx(i) = Core::Clone(this->GetByIdx(i));
				}
			}

			item.front = this->front;
			item.rear = this->rear;
			item.count = this->count;
			return item;
		});
	}

	template<typename THeap, typename T>
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseConcurencyDynamicQueue<THeap, T> BaseConcurencyDynamicQueue<THeap, T>::Clone(TLambda&& lambda) const
	{
		return this->flowController.SharedLock([&] () -> BaseConcurencyDynamicQueue InlineLambda {
			auto item = BaseConcurencyDynamicQueue(this->count);

			for (SizeT i = 0; i < this->count; i++)
			{
				item.GetByIdx(i) = lambda(this->GetByIdx(i));
			}

			item.front = this->front;
			item.rear = this->rear;
			item.count = this->count;
			return item;
		});
	}

	template<typename THeap, typename T>
	void BaseConcurencyDynamicQueue<THeap, T>::ExtendImpl(SizeT additionalSize)
	{
		const SizeT newSize = this->bufferSize + additionalSize;
		T* newBuffer = new T[newSize];

		for (SizeT i = 0; i < this->count; i++)
		{
			newBuffer[i] = GetByIdx(i);
		}

		this->front = 0; this->rear = this->count;
		delete[] this->queue;
		this->queue = newBuffer;
		this->bufferSize = newSize;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
			TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->BaseConcurencyDynamicQueue & InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					lambda(GetByIdx(i));
				}
				return *this;
			});
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]()->BaseConcurencyDynamicQueue & InlineLambda{
				SizeT index = 0;

				for (SizeT i = 0; i < this->count; i++, index++)
				{
					lambda(GetByIdx(i), index);
				}

				return *this;
			});
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
			__assume(0);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]() -> const BaseConcurencyDynamicQueue & InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					lambda(GetByIdx(i));
				}
				return *this;
			});
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]() -> const BaseConcurencyDynamicQueue & InlineLambda{
				SizeT Index = 0;

				for (SizeT i = 0; i < this->count; i++, Index++)
				{
					lambda(GetByIdx(i), Index);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT requiredCount)
	{
		if (requiredCount > this->Count)
			requiredCount = this->Count;

		if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
			TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->BaseConcurencyDynamicQueue & InlineLambda{
				for (SizeT i = 0; i < requiredCount; i++)
				{
					lambda(GetByIdx(i));
				}
				return *this;
			});
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]()->BaseConcurencyDynamicQueue & InlineLambda{
				SizeT index = 0;

				for (SizeT i = 0; i < requiredCount; i++, index++)
				{
					lambda(GetByIdx(i), index);
				}

				return *this;
			});
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, T&> ||
				TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
				TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
			__assume(0);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT requiredCount) const
	{
		if (requiredCount > this->Count)
			requiredCount = this->Count;

		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]() -> const BaseConcurencyDynamicQueue & InlineLambda{
				for (SizeT i = 0; i < requiredCount; i++)
				{
					lambda(GetByIdx(i));
				}
				return *this;
			});
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]() -> const BaseConcurencyDynamicQueue & InlineLambda{
				SizeT Index = 0;

				for (SizeT i = 0; i < requiredCount; i++, Index++)
				{
					lambda(GetByIdx(i), Index);
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

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	U BaseConcurencyDynamicQueue<THeap, T>::AggregateImpl(TLambda&& lambda, const U& value) const {
		return this->flowController.SharedLock([&]() mutable InlineLambda {
			U result = value;
			U& data = result;
			if (this->count == 0) {
				return MoveRValue(result);
			}

			for (SizeT i = 0; i < this->count; i++)
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
		});
	}

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	U BaseConcurencyDynamicQueue<THeap, T>::AggregateImpl(TLambda&& lambda, U&& value) const {
		return this->flowController.SharedLock([&] () mutable InlineLambda {
			if (this->count == 0)
				return MoveRValue(value);

			for (SizeT i = 0; i < this->count; i++)
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
		});
	}

	template<typename THeap, typename T>
	ForceInline bool BaseConcurencyDynamicQueue<THeap, T>::AnyImpl() const
	{
		return this->count > 0;
	}

	template<typename THeap, typename T>
	template<typename Lambda>
	bool BaseConcurencyDynamicQueue<THeap, T>::AnyImpl(Lambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (this->count == 0)
				return false;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					return true;
			}

			return false;
		});
	}

	template<typename THeap, typename T>
	template<typename Lambda>
	bool BaseConcurencyDynamicQueue<THeap, T>::AllImpl(Lambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (this->count == 0)
				return false;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (!lambda(GetByIdx(i)))
					return false;
			}

			return true;
		});
	}

	template<typename THeap, typename T>
	template<typename Lambda>
	T& BaseConcurencyDynamicQueue<THeap, T>::FirstImpl(Lambda&& lambda)
	{
		return this->flowController.SharedLock([&]()->T & InlineLambda{
			if (this->count == 0)
				Internals::Exceptions::ThrowNotFound(SourcePosition);

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					return GetByIdx(i);
			}

			Internals::Exceptions::ThrowNotFound(SourcePosition);
		});
	}

	template<typename THeap, typename T>
	template<typename Lambda>
	T& BaseConcurencyDynamicQueue<THeap, T>::LastImpl(Lambda&& lambda)
	{
		return this->flowController.SharedLock([&]()->T & InlineLambda{
			if (this->count == 0)
				Internals::Exceptions::ThrowNotFound(SourcePosition);

			SizeT idx = -1;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					idx = i;
			}

			if (idx == -1)
			{
				Internals::Exceptions::ThrowNotFound(SourcePosition);
			}

			return GetByIdx(idx);
		});
	}

	template<typename THeap, typename T>
	template<typename Lambda>
	const T& BaseConcurencyDynamicQueue<THeap, T>::FirstImpl(Lambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() -> const T & InlineLambda{
			if (this->count == 0)
				Internals::Exceptions::ThrowNotFound(SourcePosition);

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					return GetByIdx(i);
			}

			Internals::Exceptions::ThrowNotFound(SourcePosition);
		});
	}

	template<typename THeap, typename T>
	template<typename Lambda>
	const T& BaseConcurencyDynamicQueue<THeap, T>::LastImpl(Lambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() -> const T & InlineLambda{
			if (this->count == 0)
				Internals::Exceptions::ThrowNotFound(SourcePosition);

			SizeT idx = -1;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					idx = i;
			}

			if (idx == -1)
			{
				Internals::Exceptions::ThrowNotFound(SourcePosition);
			}

			return GetByIdx(idx);
		});
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T BaseConcurencyDynamicQueue<THeap, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
	{
		return this->flowController.SharedLock([&, item = MoveRValue(item)]() InlineLambda{
			if (count == 0)
				return MoveRValue(item);

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					return GetByIdx(i);
			}

			return MoveRValue(item);
		});
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T BaseConcurencyDynamicQueue<THeap, T>::LastOrImpl(TLambda&& lambda, T&& item) const
	{
		return this->flowController.SharedLock([&, item = MoveRValue(item)]() InlineLambda{
			if (count == 0)
				return MoveRValue(item);

			SizeT idx = -1;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					idx = i;
			}

			if (idx == -1)
			{
				return MoveRValue(item);
			}

			return GetByIdx(idx);
		});
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T BaseConcurencyDynamicQueue<THeap, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (count == 0)
				return item;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					return GetByIdx(i);
			}

			return item;
		});
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T BaseConcurencyDynamicQueue<THeap, T>::LastOrImpl(TLambda&& lambda, const T& item) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (count == 0)
				return item;

			SizeT idx = -1;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					idx = i;
			}

			if (idx == -1)
			{
				return item;
			}

			return GetByIdx(idx);
		});
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	T BaseConcurencyDynamicQueue<THeap, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (count == 0)
				return otherLambda();

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					return GetByIdx(i);
			}

			return otherLambda();
		});
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	T BaseConcurencyDynamicQueue<THeap, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (count == 0)
				return otherLambda();

			SizeT idx = -1;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					idx = i;
			}

			if (idx == -1)
			{
				return otherLambda();
			}

			return GetByIdx(idx);
		});
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::ReverseImpl()
	{
		return this->flowController.SharedLock([&]()->BaseConcurencyDynamicQueue & InlineLambda{
			for (SizeT i = 0; i < this->count / 2; i++)
			{
				Seed::Core::Swap(GetByIdx(i), GetByIdx((this->count - 1) - i));
			}
			return *this;
		});
	}

	template<typename THeap, typename T>
	ForceInline void BaseConcurencyDynamicQueue<THeap, T>::UnsafeSet(const T& value, SizeT index)
	{
		this->queue[index] = value;
		this->rear = index;
		this->count = this->bufferSize;
	}

	template<typename THeap, typename T>
	ForceInline void BaseConcurencyDynamicQueue<THeap, T>::UnsafeSet(T&& value, SizeT index)
	{
		this->queue[index] = MoveRValue(value);
		this->rear = index;
		this->count = this->bufferSize;
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::RemoveAllImpl()
	{
		return this->flowController.SharedLock([&]()->BaseConcurencyDynamicQueue & InlineLambda{
			if (this->count == 0)
				return *this;
			delete[] this->queue;
			this->front = this->rear = this->count = this->bufferSize = 0;
			this->queue = nullptr;
			return *this;
		});
	}

	template<typename THeap, typename T>
	template<typename Lambda>
	BaseConcurencyDynamicQueue<THeap, T>& BaseConcurencyDynamicQueue<THeap, T>::RemoveAllImpl(Lambda&& lambda)
	{
		return this->flowController.SharedLock([&, lambda = MoveRValue(lambda)]()->BaseConcurencyDynamicQueue & InlineLambda{
			if (this->count == 0)
				return *this;
			for (SizeT i = 0; i < this->count; i++)
			{
				lambda(MoveRValue(GetByIdx(i)));
			}
			delete[] this->queue;
			this->front = this->rear = this->count = this->bufferSize = 0;
			this->queue = nullptr;
			return *this;
		});
	}

	template<typename THeap, typename T>
	template<typename Lambda>
	SizeT BaseConcurencyDynamicQueue<THeap, T>::GetCountIfImpl(Lambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			SizeT selectedCount = 0;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(GetByIdx(i)))
					selectedCount++;
			}

			return selectedCount;
		});
	}
}

#pragma warning(pop)