#pragma once
#include "Seed.Core.Containers.Concurency.DynamicStack.h"

#pragma warning(push)
#pragma warning(disable: 4172)

namespace Seed::Core::Containers
{
	template<typename THeap, typename T>
	BaseConcurencyDynamicStack<THeap, T>::BaseConcurencyDynamicStack(const BaseConcurencyDynamicStack& stack): lock(), bufferSize(0), stack(nullptr)
	{
		auto _ = Threading::SharedScoppedLock(stack.lock);

		this->bufferSize = stack.bufferSize;
		this->stack = new T[this->bufferSize];

		if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			Memory::Copy(stack.stack, this->stack, sizeof(T) * this->bufferSize);
		}
		else
		{
			for (SizeT i = 0; i < stack.count; i++)
			{
				this->stack[i] = stack.stack[i];
			}
		}

		this->count = stack.count;
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicStack<THeap, T>::BaseConcurencyDynamicStack(BaseConcurencyDynamicStack&& stack): lock(), bufferSize(0), stack(nullptr)
	{
		auto _ = Threading::SharedScoppedLock(stack.lock);

		this->count = stack.count;
		stack.count = 0;

		this->bufferSize = stack.bufferSize;
		stack.bufferSize = 0;

		this->stack = stack.stack;
		stack.stack = nullptr;
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicStack<THeap, T>::BaseConcurencyDynamicStack(InitilizerList<T>&& list): lock()
	{
		this->bufferSize = RuntimeConfigurations().MemoryManager.StackReserveSize + list.size();
		this->stack = new T[this->bufferSize];

		if (list.size() != 0)
		{
			auto it = list.begin();
			SizeT i = list.size();

			this->count = list.size();

			for (SizeT i = 0; i < count; i++)
			{
				this->stack[i] = MoveRValue(*it); ++it;
			}
		}
		else
		{
			this->count = 0;
		}
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicStack<THeap, T>::~BaseConcurencyDynamicStack()
	{
		delete[] this->stack;
		this->stack = nullptr;
		this->count = this->bufferSize = 0;
	}

	template<typename THeap, typename T>
	void BaseConcurencyDynamicStack<THeap, T>::Push(const T& item) {
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		if (this->count == this->bufferSize)
		{
			ExtendBufferImpl(RuntimeConfigurations().MemoryManager.StackReserveSize);
		}

		this->stack[this->count] = item;
		++this->count;
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::operator=(const BaseConcurencyDynamicStack& original)
	{
		this->~BaseConcurencyDynamicStack();
		this->BaseConcurencyDynamicStack::BaseConcurencyDynamicStack(original);
		return *this;
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::operator=(BaseConcurencyDynamicStack&& original)
	{
		this->~BaseConcurencyDynamicStack();
		this->BaseConcurencyDynamicStack::BaseConcurencyDynamicStack(MoveRValue(original));
		return *this;
	}

	template<typename THeap, typename T>
	void BaseConcurencyDynamicStack<THeap, T>::Push(T&& item)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		if (this->count == this->bufferSize)
		{
			ExtendBufferImpl(RuntimeConfigurations().MemoryManager.StackReserveSize);
		}

		this->stack[this->count] = MoveRValue(item);
		++this->count;
	}

	template<typename THeap, typename T>
	T BaseConcurencyDynamicStack<THeap, T>::Pop()
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		if (this->count == 0)
			Throw(EmptyStackException(SourcePosition));

		--this->count;
		return MoveRValue(this->stack[this->count]);
	}

	template<typename THeap, typename T>
	T BaseConcurencyDynamicStack<THeap, T>::Pop(T&& item)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		if (this->count == 0)
			return MoveRValue(item);

		--this->count;
		return MoveRValue(this->stack[this->count]);
	}

	template<typename THeap, typename T>
	T BaseConcurencyDynamicStack<THeap, T>::Pop(const T& item)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		if (this->count == 0)
			return MoveRValue(item);

		--this->count;
		return MoveRValue(this->stack[this->count]);
	}

	template<typename THeap, typename T>
	template<typename TLambda> requires Concepts::IsInvocableWithResult<TLambda, T>
	T BaseConcurencyDynamicStack<THeap, T>::Pop(TLambda&& lambda)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		if (this->count == 0)
			return lambda();

		--this->count;
		return MoveRValue(this->stack[this->count]);
	}

	template<typename THeap, typename T>
	const T& BaseConcurencyDynamicStack<THeap, T>::Top() const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			Throw(EmptyStackException(SourcePosition));

		return this->stack[this->count - 1];
	}

	template<typename THeap, typename T>
	const T& BaseConcurencyDynamicStack<THeap, T>::Top()
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			Throw(EmptyStackException(SourcePosition));

		return this->stack[this->count - 1];
	}

	template<typename THeap, typename T>
	ForceInline void BaseConcurencyDynamicStack<THeap, T>::ExtendBuffer(SizeT AdditionalSize)
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		ExtendBufferImpl(AdditionalSize);
	}

	template<typename THeap, typename T>
	void BaseConcurencyDynamicStack<THeap, T>::Shrink()
	{
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		T* newBuffer = new T[this->count];
		for (SizeT i = 0; i < this->count; i++)
		{
			newBuffer[i] = MoveRValue(this->stack[i]);
		}

		delete[] this->stack;
		this->stack = newBuffer;
		this->bufferSize = this->count;
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseConcurencyDynamicStack<THeap, T>::GetCount() const
	{
		return this->count;
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseConcurencyDynamicStack<THeap, T>::GetBufferSize() const
	{
		return this->bufferSize;
	}

	template<typename THeap, typename T>
	template<typename> requires (Concepts::IsClonable<T>)
	BaseConcurencyDynamicStack<THeap, T> BaseConcurencyDynamicStack<THeap, T>::Clone() const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		BaseConcurencyDynamicStack result(this->count);
		result.bufferSize = this->bufferSize;
		result.count = this->count;

		if constexpr (Concepts::IsStandardLayout<T>)
		{
			Memory::Copy(static_cast<const void*>(this->stack), static_cast<void*>(result.stack), this->count * sizeof(T));
		}
		else
		{
			for (SizeT i = 0; i < this->count; i++)
			{
				result.stack[i] = this->stack[i];
			}
		}

		return result;
	}

	template<typename THeap, typename T>
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseConcurencyDynamicStack<THeap, T> BaseConcurencyDynamicStack<THeap, T>::Clone(TLambda&& lambda) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		BaseConcurencyDynamicStack result(this->count);
		result.bufferSize = this->bufferSize;
		result.count = this->count;

		for (SizeT i = 0; i < this->count; i++)
		{
			result.stack[i] = lambda(this->stack[i]);
		}

		return result;
	}

	template<typename THeap, typename T>
	void BaseConcurencyDynamicStack<THeap, T>::ExtendBufferImpl(SizeT AdditionalSize)
	{
		T* newBuffer = new T[this->bufferSize + AdditionalSize];

		for (SizeT i = 0; i < this->count; i++)
		{
			newBuffer[i] = MoveRValue(this->stack[i]);
		}

		delete[] this->stack;
		this->stack = newBuffer;
		this->bufferSize = this->bufferSize + AdditionalSize;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			if (this->count == 0)
				return *this;

			for (SizeT i = 0; i < count; i++)
			{
				lambda(this->stack[i]);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			if (this->count == 0)
				return *this;

			for (SizeT i = 0; i < count; i++)
			{
				lambda(this->stack[i], i);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT itemsCount)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			if (this->count == 0)
				return *this;

			SizeT end = (itemsCount > this->Count) ? this->Count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				lambda(this->stack[i]);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			if (this->count == 0)
				return *this;

			SizeT end = (itemsCount > this->Count) ? this->Count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				lambda(this->stack[i], i);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	const BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			if (this->count == 0)
				return *this;

			for (SizeT i = 0; i < count; i++)
			{
				lambda(this->stack[i]);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			if (this->count == 0)
				return *this;

			for (SizeT i = 0; i < count; i++)
			{
				lambda(this->stack[i], i);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	const BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT itemsCount) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			if (this->count == 0)
				return *this;

			SizeT end = (itemsCount > this->Count) ? this->Count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				lambda(this->stack[i]);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(this->lock);

			if (this->count == 0)
				return *this;

			SizeT end = (itemsCount > this->Count) ? this->Count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				lambda(this->stack[i], i);
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

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	U BaseConcurencyDynamicStack<THeap, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		U result = value;

		if (this->count == 0)
			return result;

		for (SizeT i = 0; i < count; i++)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				result = lambda(result, this->stack[i]);
			}
			else
			{
				lambda(result, this->stack[i]);
			}
		}

		return MoveRValue(result);
	}

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	U BaseConcurencyDynamicStack<THeap, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			return value;

		for (SizeT i = 0; i < count; i++)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				value = lambda(value, this->stack[i]);
			}
			else
			{
				lambda(value, this->stack[i]);
			}
		}

		return MoveRValue(value);
	}

	template<typename THeap, typename T>
	ForceInline bool BaseConcurencyDynamicStack<THeap, T>::AnyImpl() const
	{
		return this->count > 0;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	bool BaseConcurencyDynamicStack<THeap, T>::AnyImpl(TLambda&& lambda) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			return false;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				return true;
		}

		return false;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	bool BaseConcurencyDynamicStack<THeap, T>::AllImpl(TLambda&& lambda) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			return false;

		for (SizeT i = 0; i < count; i++)
		{
			if (!lambda(this->stack[i]))
				return false;
		}

		return true;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T& BaseConcurencyDynamicStack<THeap, T>::FirstImpl(TLambda&& lambda) {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				return static_cast<T&>(this->stack[i]);
		}

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T& BaseConcurencyDynamicStack<THeap, T>::LastImpl(TLambda&& lambda) {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		SizeT index = -1;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				index = i;
		}

		if (index != -1)
			return static_cast<T&>(this->stack[index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const T& BaseConcurencyDynamicStack<THeap, T>::FirstImpl(TLambda&& lambda) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				return static_cast<T&>(this->stack[i]);
		}

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const T& BaseConcurencyDynamicStack<THeap, T>::LastImpl(TLambda&& lambda) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (this->count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		SizeT index = -1;
		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				index = i;
		}

		if (index != -1)
			return static_cast<T&>(this->stack[index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T BaseConcurencyDynamicStack<THeap, T>::FirstOrImpl(TLambda&& lambda, T&& item) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (count == 0)
			return MoveRValue(item);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				return this->stack[i];
		}

		return MoveRValue(item);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T BaseConcurencyDynamicStack<THeap, T>::LastOrImpl(TLambda&& lambda, T&& item) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (count == 0)
			return MoveRValue(item);

		SizeT index = -1;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				index = i;
		}

		if (index != -1)
			return this->stack[index];

		return MoveRValue(item);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T BaseConcurencyDynamicStack<THeap, T>::FirstOrImpl(TLambda&& lambda, const T& item) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (count == 0)
			return item;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				return this->stack[i];
		}

		return item;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T BaseConcurencyDynamicStack<THeap, T>::LastOrImpl(TLambda&& lambda, const T& item) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (count == 0)
			return item;

		SizeT index = -1;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				index = i;
		}

		if (index != -1)
			return this->stack[index];

		return item;
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	T BaseConcurencyDynamicStack<THeap, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (count == 0)
			return otherLambda();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				return this->stack[i];
		}

		return otherLambda();
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	T BaseConcurencyDynamicStack<THeap, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const {
		auto _ = Threading::SharedScoppedLock(this->lock);

		if (count == 0)
			return otherLambda();

		SizeT index = -1;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				index = i;
		}

		if (index != -1)
			return this->stack[index];

		return otherLambda();
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::ReverseImpl() {
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		for (SizeT i = 0; i < this->count / 2; i++)
		{
			Swap(this->stack[i], this->stack[(this->count - 1) - i]);
		}

		return *this;
	}

	template<typename THeap, typename T>
	ForceInline void BaseConcurencyDynamicStack<THeap, T>::UnsafeSet(const T& value, SizeT index)
	{
		this->stack[index] = value;
		this->count = index + 1;
	}

	template<typename THeap, typename T>
	ForceInline void BaseConcurencyDynamicStack<THeap, T>::UnsafeSet(T&& value, SizeT index)
	{
		this->stack[index] = MoveRValue(value);
		this->count = index + 1;
	}

	template<typename THeap, typename T>
	BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::RemoveAllImpl() {
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		delete[] this->stack; 
		this->stack = nullptr;
		this->count = this->bufferSize = 0;

		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseConcurencyDynamicStack<THeap, T>& BaseConcurencyDynamicStack<THeap, T>::RemoveAllImpl(TLambda&& lambda) {
		auto _ = Threading::ExclusiveScoppedLock(this->lock);

		for (SizeT i = 0; i < this->count; i++)
		{
			lambda(MoveRValue(this->stack[i]));
		}

		delete[] this->stack; 
		this->stack = nullptr;
		this->count = this->bufferSize = 0;

		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	SizeT BaseConcurencyDynamicStack<THeap, T>::GetCountIfImpl(TLambda&& lambda) const
	{
		auto _ = Threading::SharedScoppedLock(this->lock);

		SizeT selectedCount = 0;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(this->stack[i]))
				selectedCount++;
		}

		return selectedCount;
	}
}

#pragma warning(pop)