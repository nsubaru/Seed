#pragma once
#include "Seed.Core.Containers.NonConcurency.FixedStack.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4172)

namespace Seed::Core::Containers
{
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedStack<THeap, TSize, T>::~BaseFixedStack()
	{
		if constexpr (TypeTraits::IsSameV<THeap, Memory::SensitiveDataHeap> &&
			TypeTraits::IsStandardLayout<T>)
		{
			Memory::SecureZeroMemory(stack);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedStack<THeap, TSize, T>::BaseFixedStack(const BaseFixedStack& original): count(original.count)
	{
		if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			Memory::Copy(original.stack, this->stack);
		}
		else
		{
			for (SizeT i = 0; i < count; i++)
			{
				this->stack[i] = original.stack[i];
			}
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedStack<THeap, TSize, T>::BaseFixedStack(BaseFixedStack&& original): count(original.count)
	{
		if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			Memory::Copy(original.stack, this->stack);
		}
		else
		{
			for (SizeT i = 0; i < TSize; i++)
			{
				this->stack[i] = MoveRValue(original.stack[i]);
			}
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedStack<THeap, TSize, T>& BaseFixedStack<THeap, TSize, T>::operator=(const BaseFixedStack& original)
	{
		this->~BaseFixedStack();
		this->BaseFixedStack::BaseFixedStack(original);
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedStack<THeap, TSize, T>& BaseFixedStack<THeap, TSize, T>::operator=(BaseFixedStack&& original)
	{
		this->~BaseFixedStack();
		this->BaseFixedStack::BaseFixedStack(MoveRValue(original));
		return *this;
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedStack<THeap, TSize, T>::BaseFixedStack(InitilizerList<T>&& list): count(0)
	{
		if (list.size() != 0)
		{
			SizeT actuallyCopied = (TSize < list.size()) ? TSize : list.size();
			auto it = list.begin();

			for (SizeT i = 0; i < actuallyCopied; i++)
			{
				stack[i] = MoveRValue(*it); ++it;
			}

			count = actuallyCopied;
		}
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedStack<THeap, TSize, T>::Push(const T& item)
	{
		if (count == TSize)
			Throw(FullStackException(SourcePosition, TSize));

		stack[count] = item;
		++count;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedStack<THeap, TSize, T>::Push(T&& item)
	{
		if (count == TSize)
			Throw(FullStackException(SourcePosition, TSize));

		stack[count] = MoveRValue(item);
		++count;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline T BaseFixedStack<THeap, TSize, T>::Pop()
	{
		if (count == 0)
			Throw(EmptyStackException(SourcePosition));

		--count;
		return MoveRValue(stack[count]);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline T BaseFixedStack<THeap, TSize, T>::Pop(T&& item)
	{
		if (count == 0)
			return MoveRValue(item);

		--count;
		return MoveRValue(stack[count]);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline T BaseFixedStack<THeap, TSize, T>::Pop(const T& item)
	{
		if (count == 0)
			return item;

		--count;
		return MoveRValue(stack[count]);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda> requires Concepts::IsInvocableWithResult<TLambda, T>
	ForceInline T BaseFixedStack<THeap, TSize, T>::Pop(TLambda&& lambda)
	{
		if (count == 0)
			return lambda();

		--count;
		return MoveRValue(stack[count]);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline const T& BaseFixedStack<THeap, TSize, T>::Top()
	{
		if (count == 0)
			Throw(EmptyStackException(SourcePosition));

		return stack[count - 1];
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline const T& BaseFixedStack<THeap, TSize, T>::Top() const
	{
		if (count == 0)
			Throw(EmptyStackException(SourcePosition));

		return stack[count - 1];
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline SizeT BaseFixedStack<THeap, TSize, T>::GetCount() const
	{
		return count;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename> requires (Concepts::IsClonable<T>)
	BaseFixedStack<THeap, TSize, T> BaseFixedStack<THeap, TSize, T>::Clone() const
	{
		BaseFixedStack result;

		if constexpr (TypeTraits::IsStandardLayout<T>)
		{
			Memory::Copy(stack, result.stack);
		}
		else
		{
			for (SizeT i = 0; i < count; i++)
			{
				result.stack[i] = Core::Clone(stack[i]);
			}
		}
		result.count = this->count;
		return result;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
	BaseFixedStack<THeap, TSize, T> BaseFixedStack<THeap, TSize, T>::Clone(TLambda&& lambda) const
	{
		BaseFixedStack result;
		result.count = count;

		for (SizeT i = 0; i < count; i++)
		{
			result.stack[i] = lambda(stack[i]);
		}

		return result;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseFixedStack<THeap, TSize, T>& BaseFixedStack<THeap, TSize, T>::ForEachImpl(TLambda&& lambda)
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const BaseFixedStack<THeap, TSize, T>& BaseFixedStack<THeap, TSize, T>::ForEachImpl(TLambda&& lambda) const
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename U, typename TLambda>
	U BaseFixedStack<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, const U& value) const
	{
		U result = value;
		U& data = result;

		if (count == 0)
			return result;

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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename U, typename TLambda>
	U BaseFixedStack<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, U&& value) const
	{
		if (count == 0)
			return value;

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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline bool BaseFixedStack<THeap, TSize, T>::AnyImpl() const
	{
		return count > 0;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	bool BaseFixedStack<THeap, TSize, T>::AnyImpl(TLambda&& lambda) const
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	bool BaseFixedStack<THeap, TSize, T>::AllImpl(TLambda&& lambda) const
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T& BaseFixedStack<THeap, TSize, T>::FirstImpl(TLambda&& lambda)
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	T& BaseFixedStack<THeap, TSize, T>::LastImpl(TLambda&& lambda)
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				Index = i;
		}

		if (Index != Math::Max<SizeT>())
			return static_cast<T&>(stack[Index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const T& BaseFixedStack<THeap, TSize, T>::FirstImpl(TLambda&& lambda) const
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				return static_cast<const T&>(stack[i]);
		}

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	const T& BaseFixedStack<THeap, TSize, T>::LastImpl(TLambda&& lambda) const
	{
		if (count == 0)
			Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				Index = i;
		}

		if (Index != Math::Max<SizeT>())
			return static_cast<const T&>(stack[Index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedStack<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedStack<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, T&& item) const
	{
		if (count == 0)
			return MoveRValue(item);

		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				Index = i;
		}

		if (Index != Math::Max<SizeT>())
			return stack[Index];

		return MoveRValue(item);
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedStack<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	ForceInline T BaseFixedStack<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, const T& item) const
	{
		if (count == 0)
			return item;

		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				Index = i;
		}

		if (Index != Math::Max<SizeT>())
			return stack[Index];

		return item;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseFixedStack<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
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
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda, typename TGetterLambda>
	ForceInline T BaseFixedStack<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
	{
		if (count == 0)
			return otherLambda();

		SizeT Index = Math::Max<SizeT>();

		for (SizeT i = 0; i < count; i++)
		{
			if (lambda(stack[i]))
				Index = i;
		}

		if (Index != Math::Max<SizeT>())
			return stack[Index];

		return otherLambda();
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedStack<THeap, TSize, T>& BaseFixedStack<THeap, TSize, T>::ReverseImpl()
	{
		for (SizeT i = 0; i < count / 2; i++)
		{
			Swap(stack[i], stack[(count - 1) - i]);
		}
		return *this;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedStack<THeap, TSize, T>::UnsafeSet(T&& Value, SizeT Index)
	{
		stack[Index] = MoveRValue(Value);
		count = Index + 1;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	ForceInline void BaseFixedStack<THeap, TSize, T>::UnsafeSet(const T& Value, SizeT Index)
	{
		stack[Index] = Value;
		count = Index + 1;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	BaseFixedStack<THeap, TSize, T>& BaseFixedStack<THeap, TSize, T>::RemoveAllImpl()
	{
		for (SizeT i = 0; i < count; i++)
		{
			stack[i].~T();
		}
		this->count = 0;
		return *this;
	}
	
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	BaseFixedStack<THeap, TSize, T>& BaseFixedStack<THeap, TSize, T>::RemoveAllImpl(TLambda&& lambda)
	{
		for (SizeT i = 0; i < count; i++)
		{
			lambda(MoveRValue(stack[i])); stack[i].~T();
		}
		this->count = 0;
		return *this;
	}
}

#pragma warning(pop)