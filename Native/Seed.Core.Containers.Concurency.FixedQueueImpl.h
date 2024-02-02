#pragma once
#include "Seed.Core.Containers.Concurency.FixedQueue.h"

#pragma warning(push)
#pragma warning(disable: 4172)
#pragma warning(disable: 4996)

namespace Seed::Core::Containers
{
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	T& BaseConcurencyFixedQueue<THeap, TSize, T>::GetByIdx(SizeT Index)
	{
		return queue[(front + Index) % TSize];
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	const T& BaseConcurencyFixedQueue<THeap, TSize, T>::GetByIdx(SizeT Index) const
	{
		return queue[(front + Index) % TSize];
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedQueue<THeap, TSize, T>::~BaseConcurencyFixedQueue()
	{
		if constexpr (TypeTraits::IsSameV<THeap, Memory::SensitiveDataHeap> &&
			TypeTraits::IsStandardLayout<T>)
		{
			Memory::SecureZeroMemory(queue);
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedQueue<THeap, TSize, T>::BaseConcurencyFixedQueue(InitilizerList<T>&& list): BaseConcurencyFixedQueue()
	{
		if (list.size() != 0)
		{
			SizeT actuallyCopied = (TSize < list.size()) ? TSize : list.size();
			auto it = list.begin();
			for (SizeT i = 0; i < actuallyCopied; i++)
			{
				this->queue[i] = MoveRValue(*it); ++it;
			}
			this->rear = this->count = actuallyCopied;
			this->front = 0;
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedQueue<THeap, TSize, T>::BaseConcurencyFixedQueue(const BaseConcurencyFixedQueue& queue): flowController(), front(0), rear(0)
	{
		queue.flowController.SharedLock([&] () {
			this->front = queue.front;
			this->rear = queue.rear;
			this->count = queue.count;

			if constexpr (Concepts::IsStandardLayout<T>)
			{
				Memory::Copy(queue.queue, this->queue);
			}
			else
			{
				for (SizeT i = 0; i < this->count; i++)
				{
					this->GetByIdx(i) = queue.GetByIdx(i);
				}
			}
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedQueue<THeap, TSize, T>::BaseConcurencyFixedQueue(BaseConcurencyFixedQueue&& queue): flowController(), front(0), rear(0)
	{
		queue.flowController.SharedLock([&]() InlineLambda{
			this->front = queue.front;
			queue.front = 0;
			this->rear = queue.rear;
			queue.rear = 0;
			this->count = queue.count;
			queue.count = 0;

			if constexpr (Concepts::IsStandardLayout<T>)
			{
				Memory::Copy(queue.queue, this->queue);
			}
			else
			{
				for (SizeT i = 0; i < this->count; i++)
				{
					this->GetByIdx(i) = MoveRValue(queue.GetByIdx(i));
				}
			}
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedQueue<THeap, TSize, T>& BaseConcurencyFixedQueue<THeap, TSize, T>::operator=(const BaseConcurencyFixedQueue& original)
	{
		this->~BaseConcurencyFixedQueue();
		this->BaseConcurencyFixedQueue::BaseConcurencyFixedQueue(original);
		return *this;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedQueue<THeap, TSize, T>& BaseConcurencyFixedQueue<THeap, TSize, T>::operator=(BaseConcurencyFixedQueue&& original)
	{
		this->~BaseConcurencyFixedQueue();
		this->BaseConcurencyFixedQueue::BaseConcurencyFixedQueue(MoveRValue(original));
		return *this;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	void BaseConcurencyFixedQueue<THeap, TSize, T>::Enqueue(const T& Value)
	{
		this->flowController.Produce([&]() InlineLambda{
			if (++this->count > TSize)
				this->flowController.WaitForConsumer();

			SizeT newRear = (this->rear + 1) % TSize;
			this->GetByIdx(this->rear - this->front) = Value;
			this->rear = newRear;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	void BaseConcurencyFixedQueue<THeap, TSize, T>::Enqueue(T&& Value)
	{
		this->flowController.Produce([&, Value = MoveRValue(Value)]() InlineLambda{
			if (++this->count > TSize)
				this->flowController.WaitForConsumer();

			SizeT newRear = (this->rear + 1) % TSize;
			this->GetByIdx(this->rear - this->front) = MoveRValue(Value);
			this->rear = newRear;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	T BaseConcurencyFixedQueue<THeap, TSize, T>::Dequeue()
	{
		return this->flowController.Consume([&] () -> T InlineLambda {
			if (this->count == 0)
				this->flowController.WaitForProducer();

			T res = MoveRValue(this->GetByIdx(0));
			this->front = (this->front + 1) % TSize;
			--this->count;
			return res;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline const T& BaseConcurencyFixedQueue<THeap, TSize, T>::Top()
	{
		return this->flowController.SharedLock([&]() -> const T & InlineLambda{
			if (front == rear)
				Throw(EmptyQueueException(SourcePosition));

			return queue[front];
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline const T& BaseConcurencyFixedQueue<THeap, TSize, T>::Top() const
	{
		return this->flowController.SharedLock([&]() -> const T & InlineLambda{
			if (front == rear)
				Throw(EmptyQueueException(SourcePosition));

			return queue[front];
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline SizeT BaseConcurencyFixedQueue<THeap, TSize, T>::GetCount() const
	{
		return this->count;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename> requires (Concepts::IsClonable<T>)
	BaseConcurencyFixedQueue<THeap, TSize, T> BaseConcurencyFixedQueue<THeap, TSize, T>::Clone() const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			BaseConcurencyFixedQueue result;
			result.front = this->front;
			result.rear = this->rear;
			result.count = this->count;

			if constexpr (Concepts::IsStandardLayout<T>)
			{
				Memory::Copy(this->queue, result.queue);
			}
			else
			{
				for (SizeT i = 0; i < this->count; i++)
				{
					result.GetByIdx(i) = Core::Clone(this->GetByIdx(i));
				}
			}
			return result;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseConcurencyFixedQueue<THeap, TSize, T> BaseConcurencyFixedQueue<THeap, TSize, T>::Clone(TLambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			BaseConcurencyFixedQueue result;
			result.front = this->front;
			result.rear = this->rear;
			result.count = this->count;

			for (SizeT i = 0; i < this->count; i++)
			{
				result.GetByIdx(i) = lambda(this->GetByIdx(i));
			}
			return result;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseConcurencyFixedQueue<THeap, TSize, T>& BaseConcurencyFixedQueue<THeap, TSize, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
			TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->BaseConcurencyFixedQueue & InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					lambda(this->GetByIdx(i));
				}
				return *this;
			});
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
			TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->BaseConcurencyFixedQueue & InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					lambda(this->GetByIdx(i), i);
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const BaseConcurencyFixedQueue<THeap, TSize, T>& BaseConcurencyFixedQueue<THeap, TSize, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]() -> const BaseConcurencyFixedQueue & InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					lambda(this->GetByIdx(i));
				}
				return *this;
			});
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]() -> const BaseConcurencyFixedQueue & InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					lambda(this->GetByIdx(i), i);
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
	U BaseConcurencyFixedQueue<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		return this->flowController.SharedLock([&]() mutable InlineLambda{
			U result = value;
			U & data = result;
			if (this->count == 0)
			{
				return MoveRValue(result);
			}

			for (SizeT i = 0; i < this->count; i++)
			{
				if constexpr (
					Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
					Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
				{
					data = lambda(data, this->GetByIdx(i));
				}
				else
				{
					lambda(data, this->GetByIdx(i));
				}
			}

			return MoveRValue(result);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename U, typename TLambda>
	U BaseConcurencyFixedQueue<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		return this->flowController.SharedLock([&, value = MoveRValue(value)]() mutable InlineLambda{
			U result = MoveRValue(value);

			if (this->count == 0)
				return MoveRValue(result);

			for (SizeT i = 0; i < this->count; i++)
			{
				if constexpr (
					Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
					Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
				{
					value = lambda(value, this->GetByIdx(i));
				}
				else
				{
					lambda(value, this->GetByIdx(i));
				}
			}

			return MoveRValue(value);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline bool BaseConcurencyFixedQueue<THeap, TSize, T>::AnyImpl() const
	{
		return this->count > 0;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	bool BaseConcurencyFixedQueue<THeap, TSize, T>::AnyImpl(TLambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (this->count == 0)
				return false;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
					return true;
			}

			return false;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	bool BaseConcurencyFixedQueue<THeap, TSize, T>::AllImpl(TLambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (this->count == 0)
				return false;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (!lambda(this->GetByIdx(i)))
					return false;
			}

			return true;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T& BaseConcurencyFixedQueue<THeap, TSize, T>::FirstImpl(TLambda&& lambda)
	{
		return this->flowController.SharedLock([&]()->T & InlineLambda{
			if (this->count == 0)
				Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
				return this->GetByIdx(i);
				}
			}

			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T& BaseConcurencyFixedQueue<THeap, TSize, T>::LastImpl(TLambda&& lambda)
	{
		return this->flowController.SharedLock([&]()->T & InlineLambda{
			if (this->count == 0)
				Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

			T* item = nullptr;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
					item = &this->GetByIdx(i);
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
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const T& BaseConcurencyFixedQueue<THeap, TSize, T>::FirstImpl(TLambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() -> const T & InlineLambda{
			if (this->count == 0)
				Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
					return this->GetByIdx(i);
				}
			}

			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const T& BaseConcurencyFixedQueue<THeap, TSize, T>::LastImpl(TLambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() -> const T & InlineLambda{
			if (this->count == 0)
				Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

			const T* item = nullptr;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
					item = &this->GetByIdx(i);
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
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T BaseConcurencyFixedQueue<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, T&& element) const
	{
		return this->flowController.SharedLock([&, element = MoveRValue(element)]() InlineLambda{
			if (count == 0)
				return MoveRValue(element);

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
					return this->GetByIdx(i);
				}
			}

			return MoveRValue(element);
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T BaseConcurencyFixedQueue<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, T&& element) const
	{
		return this->flowController.SharedLock([&, element = MoveRValue(element)]() InlineLambda{
			if (count == 0)
				return MoveRValue(element);

			const T* item = nullptr;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
					item = &this->GetByIdx(i);
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
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T BaseConcurencyFixedQueue<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, const T& element) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (count == 0)
				return element;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
					return this->GetByIdx(i);
				}
			}

			return element;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T BaseConcurencyFixedQueue<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, const T& element) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (count == 0)
				return element;

			const T* item = nullptr;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
					item = &this->GetByIdx(i);
				}
			}

			if (item != nullptr)
			{
				return *item;
			}
			else
			{
				return element;
			}
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	T BaseConcurencyFixedQueue<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (count == 0)
				return otherLambda();

			for (SizeT i = 0; i < this->count; i++)
			{
				if (lambda(this->GetByIdx(i)))
				{
					return this->GetByIdx(i);
				}
			}

			return otherLambda();
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	T BaseConcurencyFixedQueue<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			if (count == 0)
				return otherLambda();

			const T* item = nullptr;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (!lambda(this->GetByIdx(i)))
				{
					item = &this->GetByIdx(i);
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
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedQueue<THeap, TSize, T>& BaseConcurencyFixedQueue<THeap, TSize, T>::ReverseImpl()
	{
		return this->flowController.SharedLock([&]()->BaseConcurencyFixedQueue & InlineLambda{
			for (SizeT i = 0; i < this->count / 2; i++)
			{
				Seed::Core::Swap(this->GetByIdx(i), this->GetByIdx((this->count - 1) - i));
			}
			return *this;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseConcurencyFixedQueue<THeap, TSize, T>::UnsafeSet(const T& value, SizeT index)
	{
		this->GetByIdx(index) = value;
		this->count = index + 1;
		this->rear = index;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseConcurencyFixedQueue<THeap, TSize, T>::UnsafeSet(T&& value, SizeT index)
	{
		this->GetByIdx(index) = MoveRValue(value);
		this->count = index + 1;
		this->rear = index;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseConcurencyFixedQueue<THeap, TSize, T>& BaseConcurencyFixedQueue<THeap, TSize, T>::RemoveAllImpl()
	{
		return this->flowController.SharedLock([&]()->BaseConcurencyFixedQueue & InlineLambda{
			if (this->count == 0)
				return *this;
			for (SizeT i = 0; i < this->count; i++)
			{
				this->GetByIdx(i).~T();
			}
			this->count = 0;
			return *this;
		});
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseConcurencyFixedQueue<THeap, TSize, T>& BaseConcurencyFixedQueue<THeap, TSize, T>::RemoveAllImpl(TLambda&& lambda)
	{
		return this->flowController.SharedLock([&, lambda = MoveRValue(lambda)]()->BaseConcurencyFixedQueue & InlineLambda{
			if (this->count == 0)
				return *this;
			for (SizeT i = 0; i < this->count; i++)
			{
				lambda(MoveRValue(this->GetByIdx(i)));
			}
			this->count = 0;
			return *this;
		});
	}
}

#pragma warning(push)