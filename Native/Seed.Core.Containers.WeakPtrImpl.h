#pragma once
#include "Seed.Core.Containers.WeakPtr.h"

namespace Seed::Core
{
	template<typename THeap, typename T>
	bool BaseWeakPtr<THeap, T>::Dispose()
	{
		return This->RefCount == 0;
	}

	template<typename THeap, typename T>
	BaseWeakPtr<THeap, T>::BaseWeakPtr(Data* This): This(This)
	{
		++This->WeakRefCount;
	}

	template<typename THeap, typename T>
	ForceInline T& BaseWeakPtr<THeap, T>::operator*()
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return *This->ptr;
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseWeakPtr<THeap, T>::operator*() const
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return *This->ptr;
	}

	template<typename THeap, typename T>
	ForceInline T* BaseWeakPtr<THeap, T>::operator->()
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return This->ptr;
	}

	template<typename THeap, typename T>
	ForceInline const T* BaseWeakPtr<THeap, T>::operator->() const
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return This->ptr;
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	ForceInline const TInterface& BaseWeakPtr<THeap, T>::As() const
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return dynamic_cast<const TInterface&>(*This->ptr);
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	ForceInline TInterface& BaseWeakPtr<THeap, T>::As()
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return dynamic_cast<TInterface&>(*This->ptr);
	}

	template<typename THeap, typename T>
	ForceInline decltype(auto) BaseWeakPtr<THeap, T>::GetTypeInfo() const
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return typeid(*This->ptr);
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseWeakPtr<THeap, T>::GetWeakRefCount() const
	{
		if (This == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return This->WeakRefCount;
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseWeakPtr<THeap, T>::GetRefCount() const
	{
		if (This == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return This->RefCount;
	}

	template<typename THeap, typename T>
	ForceInline Boolean BaseWeakPtr<THeap, T>::GetHasValue()
	{
		return !(This == nullptr || This->RefCount == 0);
	}

	template<typename THeap, typename T>
	ForceInline bool BaseWeakPtr<THeap, T>::operator==(const std::nullptr_t&) const
	{
		return This == nullptr || This->ptr == nullptr;
	}

	template<typename THeap, typename T>
	ForceInline bool BaseWeakPtr<THeap, T>::operator==(std::nullptr_t&&) const
	{
		return This == nullptr || This->ptr == nullptr;
	}
}