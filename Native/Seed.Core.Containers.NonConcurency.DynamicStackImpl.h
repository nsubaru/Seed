#pragma once
#include "Seed.Core.Containers.NonConcurency.DynamicStack.h"

#pragma warning(push)
#pragma warning(disable: 4172)

namespace Seed::Core::Containers
{
	template<typename THeap, typename T>
	BaseDynamicStack<THeap, T>::~BaseDynamicStack()
	{
		delete[] stack;
		count = bufferSize = 0;
		stack = nullptr;
	}

	template<typename THeap, typename T>
	ForceInline BaseDynamicStack<THeap, T>::BaseDynamicStack():
		count(0),
		bufferSize(RuntimeConfigurations().MemoryManager.StackReserveSize),
		stack(new T[RuntimeConfigurations().MemoryManager.StackReserveSize])
	{
	}

	template<typename THeap, typename T>
	ForceInline BaseDynamicStack<THeap, T>::BaseDynamicStack(SizeT bufferSize):
		count(0),
		stack(new T[bufferSize]),
		bufferSize(bufferSize)
	{
	}

	template<typename THeap, typename T>
	BaseDynamicStack<THeap, T>::BaseDynamicStack(const BaseDynamicStack& original):
		count(original.count),
		bufferSize(original.bufferSize),
		stack(nullptr)
	{
		this->stack = new T[original.bufferSize];

		if constexpr (!TypeTraits::IsStandardLayout<T>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				this->stack[i] = original.stack[i];
			}
		}
		else
		{
			Memory::Copy(original.stack, this->stack, sizeof(T) * original.count);
		}

		this->count = original.count;
	}

	template<typename THeap, typename T>
	BaseDynamicStack<THeap, T>::BaseDynamicStack(BaseDynamicStack&& original)
	{
		this->count = MoveRValue(original.count);
		this->stack = MoveRValue(original.stack);
		this->bufferSize = MoveRValue(original.bufferSize);

		original.count = 0;
		original.stack = nullptr;
		original.bufferSize = 0;
	}

	template<typename THeap, typename T>
	BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::operator=(const BaseDynamicStack& original)
	{
		this->~BaseDynamicStack();
		this->BaseDynamicStack::BaseDynamicStack(original);
		return *this;
	}

	template<typename THeap, typename T>
	BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::operator=(BaseDynamicStack&& original)
	{
		this->~BaseDynamicStack();
		this->BaseDynamicStack::BaseDynamicStack(MoveRValue(original));
		return *this;
	}

	template<typename THeap, typename T>
	BaseDynamicStack<THeap, T>::BaseDynamicStack(InitilizerList<T>&& list):
		count(0),
		bufferSize(0),
		stack(nullptr)
	{
		if (list.size() != 0)
		{
			count = list.size();
			bufferSize = count + RuntimeConfigurations().MemoryManager.StackReserveSize;
			this->stack = new T[bufferSize];

			auto it = list.begin();

			for (SizeT i = 0; i < count; i++)
			{
				stack[i] = MoveRValue(*it); ++it;
			}
		}
		else
		{
			bufferSize = count + RuntimeConfigurations().MemoryManager.StackReserveSize;
			this->stack = new T[bufferSize];
		}
	}

	template<typename THeap, typename T>
	ForceInline void BaseDynamicStack<THeap, T>::Push(const T& item)
	{
		if (count == bufferSize)
		{
			ExtendBufferImpl(RuntimeConfigurations().MemoryManager.StackReserveSize);
		}

		stack[count] = item;
		++count;
	}

	template<typename THeap, typename T>
	ForceInline void BaseDynamicStack<THeap, T>::Push(T&& item)
	{
		if (count == bufferSize)
		{
			ExtendBufferImpl(RuntimeConfigurations().MemoryManager.StackReserveSize);
		}

		stack[count] = MoveRValue(item);
		++count;
	}

	template<typename THeap, typename T>
	ForceInline T BaseDynamicStack<THeap, T>::Pop()
	{
		if (count == 0)
			Throw(EmptyStackException(SourcePosition));

		--count;
		return MoveRValue(stack[count]);
	}

	template<typename THeap, typename T>
	ForceInline T BaseDynamicStack<THeap, T>::Pop(T&& item)
	{
		if (count == 0)
			return MoveRValue(item);

		--count;
		return MoveRValue(stack[count]);
	}

	template<typename THeap, typename T>
	ForceInline T BaseDynamicStack<THeap, T>::Pop(const T& item)
	{
		if (count == 0)
			return item;

		--count;
		return MoveRValue(stack[count]);
	}

	template<typename THeap, typename T>
	template<typename TLambda> requires Concepts::IsInvocableWithResult<TLambda, T>
	ForceInline T BaseDynamicStack<THeap, T>::Pop(TLambda&& lambda)
	{
		if (count == 0)
			return lambda();

		return MoveRValue(stack[count]);
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseDynamicStack<THeap, T>::Top() const
	{
		if (count == 0)
			Throw(EmptyStackException(SourcePosition));

		return stack[count - 1];
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseDynamicStack<THeap, T>::Top()
	{
		if (count == 0)
			Throw(EmptyStackException(SourcePosition));

		return stack[count - 1];
	}

	template<typename THeap, typename T>
	ForceInline void BaseDynamicStack<THeap, T>::ExtendBuffer(SizeT AdditionalSize)
	{
		ExtendBufferImpl(AdditionalSize);
	}

	template<typename THeap, typename T>
	void BaseDynamicStack<THeap, T>::Shrink()
	{
		T* newBuffer = new T[count];
		for (SizeT i = 0; i < count; i++)
		{
			newBuffer[i] = MoveRValue(stack[i]);
		}

		delete[] stack;
		stack = newBuffer;
		bufferSize = count;
	}

	template<typename THeap, typename T>
	template<typename> requires (Concepts::IsClonable<T>)
	BaseDynamicStack<THeap, T> BaseDynamicStack<THeap, T>::Clone() const
	{
		BaseDynamicStack result(this->count);
		result.bufferSize = this->bufferSize;
		result.count = this->count;

		if constexpr (Concepts::IsStandardLayout<T>)
		{
			Memory::Copy(this->stack, result.stack, sizeof(T) * this->count);
		}
		else
		{
			for (SizeT i = 0; i < count; i++)
			{
				result.stack[i] = stack[i];
			}
		}

		return result;
	}

	template<typename THeap, typename T>
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseDynamicStack<THeap, T> BaseDynamicStack<THeap, T>::Clone(TLambda&& lambda) const
	{
		BaseDynamicStack result(this->count);
		result.bufferSize = bufferSize;
		result.count = count;

		for (SizeT i = 0; i < count; i++)
		{
			result.stack[i] = lambda(stack[i]);
		}
		return result;
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseDynamicStack<THeap, T>::GetCount() const
	{
		return count;
	}

	template<typename THeap, typename T>
	void BaseDynamicStack<THeap, T>::ExtendBufferImpl(SizeT AdditionalSize)
	{
		T* newBuffer = nullptr;
		try
		{
			T* newBuffer = new T[bufferSize + AdditionalSize];

			for (SizeT i = 0; i < count; i++)
			{
				newBuffer[i] = MoveRValue(stack[i]);
			}

			Swap(stack, newBuffer);
			delete[] newBuffer; newBuffer = nullptr;
			bufferSize = bufferSize + AdditionalSize;
		}
		catch (...)
		{
			delete[] newBuffer;
			Unsafe(Internals::Exceptions::__ReThrow());
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::ForEachImpl(TLambda&& lambda)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&>)
		{
			if (count == 0)
				return *this;

			for (SizeT i = 0; i < count; i++)
			{
				lambda(stack[i]);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			if (count == 0)
				return *this;

			for (SizeT i = 0; i < count; i++)
			{
				lambda(stack[i], i);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT itemsCount)
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&>)
		{
			if (count == 0)
				return *this;

			SizeT end = (itemsCount > count) ? count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				lambda(stack[i]);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			if (count == 0)
				return *this;

			SizeT end = (itemsCount > count) ? count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				lambda(stack[i], i);
			}

			return *this;
		}
		else
		{
			static_assert(TypeTraits::IsInvocable<TLambda, T&> || TypeTraits::IsInvocable<TLambda, const T&> ||
				TypeTraits::IsInvocable<TLambda, T&, SizeT> || TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::ForEachImpl(TLambda&& lambda) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			if (count == 0)
				return *this;

			for (SizeT i = 0; i < count; i++)
			{
				lambda(stack[i]);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			if (count == 0)
				return *this;

			for (SizeT i = 0; i < count; i++)
			{
				lambda(stack[i], i);
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
	const BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT itemsCount) const
	{
		if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
		{
			if (count == 0)
				return *this;

			SizeT end = (itemsCount > count) ? count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				lambda(stack[i]);
			}

			return *this;
		}
		else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
		{
			if (count == 0)
				return *this;

			SizeT end = (itemsCount > count) ? count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				lambda(stack[i], i);
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
	U BaseDynamicStack<THeap, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		U result = value;
		U& data = result;

		if (count == 0)
			return MoveRValue(result);

		for (SizeT i = 0; i < count; i++)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				data = lambda(data, stack[i]);
			}
			else
			{
				lambda(data, stack[i]);
			}
		}

		return MoveRValue(result);
	}

	template<typename THeap, typename T>
	template<typename U, typename TLambda>
	U BaseDynamicStack<THeap, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		if (count == 0)
			return MoveRValue(value);

		for (SizeT i = 0; i < count; i++)
		{
			if constexpr (
				Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
				Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
			{
				value = lambda(value, stack[i]);
			}
			else
			{
				lambda(value, stack[i]);
			}
		}

		return MoveRValue(value);
	}

	template<typename THeap, typename T>
	ForceInline bool BaseDynamicStack<THeap, T>::AnyImpl() const
	{
		return count > 0;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	bool BaseDynamicStack<THeap, T>::AnyImpl(TLambda&& lambda) const
	{
		if (count == 0)
			return false;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				return true;
		}

		return false;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	bool BaseDynamicStack<THeap, T>::AllImpl(TLambda&& lambda) const
	{
		if (count == 0)
			return false;

		for (SizeT i = 0; i < count; i++)
		{
			if (!lambda(stack[i]))
				return false;
		}

		return true;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T& BaseDynamicStack<THeap, T>::FirstImpl(TLambda&& lambda)
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				return static_cast<T&>(stack[i]);
		}

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	T& BaseDynamicStack<THeap, T>::LastImpl(TLambda&& lambda)
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		SizeT index = -1;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				index = i;
		}

		if (index != -1)
			return static_cast<T&>(stack[index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const T& BaseDynamicStack<THeap, T>::FirstImpl(TLambda&& lambda) const
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				return static_cast<T&>(stack[i]);
		}

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	const T& BaseDynamicStack<THeap, T>::LastImpl(TLambda&& lambda) const
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		SizeT index = -1;
		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				index = i;
		}

		if (index != -1)
			return static_cast<T&>(stack[index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseDynamicStack<THeap, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
	{
		if (count == 0)
			return MoveRValue(item);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				return stack[i];
		}

		return MoveRValue(item);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseDynamicStack<THeap, T>::LastOrImpl(TLambda&& lambda, T&& item) const
	{
		if (count == 0)
			return MoveRValue(item);

		SizeT index = -1;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				index = i;
		}

		if (index != -1)
			return stack[index];

		return MoveRValue(item);
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseDynamicStack<THeap, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
	{
		if (count == 0)
			return item;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				return stack[i];
		}

		return item;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	ForceInline T BaseDynamicStack<THeap, T>::LastOrImpl(TLambda&& lambda, const T& item) const
	{
		if (count == 0)
			return item;

		SizeT index = -1;
		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				index = i;
		}

		if (index != -1)
			return stack[index];

		return item;
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseDynamicStack<THeap, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				return stack[i];
		}

		return otherLambda();
	}

	template<typename THeap, typename T>
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseDynamicStack<THeap, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		SizeT index = -1;
		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				index = i;
		}

		if (index != -1)
			return stack[index];

		return otherLambda();
	}

	template<typename THeap, typename T>
	ForceInline BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::ReverseImpl()
	{
		for (SizeT i = 0; i < count / 2; i++)
		{
			Swap(stack[i], stack[(count - 1) - i]);
		}
		return *this;
	}

	template<typename THeap, typename T>
	ForceInline void BaseDynamicStack<THeap, T>::UnsafeSet(T&& Value, SizeT index)
	{
		stack[index] = MoveRValue(Value);
		count = index + 1;
	}

	template<typename THeap, typename T>
	ForceInline void BaseDynamicStack<THeap, T>::UnsafeSet(const T& Value, SizeT index)
	{
		stack[index] = Value;
		count = index + 1;
	}

	template<typename THeap, typename T>
	ForceInline BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::RemoveAllImpl()
	{
		delete[] stack; stack = nullptr;
		count = bufferSize = 0;
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	BaseDynamicStack<THeap, T>& BaseDynamicStack<THeap, T>::RemoveAllImpl(TLambda&& lambda)
	{
		for (SizeT i = 0; i < count; i++)
		{
			lambda(MoveRValue(stack[i]));
		}

		delete[] stack; stack = nullptr;
		count = bufferSize = 0;
		this->count = 0;
		return *this;
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	SizeT BaseDynamicStack<THeap, T>::GetCountIfImpl(TLambda&& lambda) const
	{
		SizeT selectedCount = 0;

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				selectedCount++;
		}

		return selectedCount;
	}
}