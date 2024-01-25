#pragma once
#include "Seed.Core.Containers.SmartPtr.h"

namespace Seed::Core
{
	template<typename THeap, typename T>
	bool BaseSmartPtr<THeap, T>::Dispose()
	{
		delete This->ptr;
		This->ptr = nullptr;

		return This->WeakRefCount == 0;
	}

	template<typename THeap, typename T>
	ForceInline BaseSmartPtr<THeap, T>::BaseSmartPtr(T*&& ptr)
	{
		if (ptr != nullptr)
		{
			This = new Data(MoveRValue(ptr));
		}
		else
		{
			This = nullptr;
		}
	}

	template<typename THeap, typename T>
	ForceInline BaseSmartPtr<THeap, T>::BaseSmartPtr(BaseUniquePtr<THeap, T>&& ptr)
	{
		if ((ptr.data) != nullptr)
		{
			This = new Data(MoveRValue(ptr.data));
			ptr.data = nullptr;
		}
		else
		{
			This = nullptr;
		}
	}

	template<typename THeap, typename T>
	ForceInline T& BaseSmartPtr<THeap, T>::operator*()
	{
		return *This->ptr;
	}

	template<typename THeap, typename T>
	ForceInline const T& BaseSmartPtr<THeap, T>::operator*() const
	{
		return *This->ptr;
	}

	template<typename THeap, typename T>
	ForceInline T* BaseSmartPtr<THeap, T>::operator->()
	{
		return This->ptr;
	}

	template<typename THeap, typename T>
	ForceInline const T* BaseSmartPtr<THeap, T>::operator->() const
	{
		return This->ptr;
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	ForceInline const TInterface& BaseSmartPtr<THeap, T>::As() const
	{
		return dynamic_cast<const TInterface&>(*This->ptr);
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	ForceInline TInterface& BaseSmartPtr<THeap, T>::As()
	{
		return dynamic_cast<TInterface&>(*This->ptr);
	}

	template<typename THeap, typename T>
	BaseWeakPtr<THeap, T> BaseSmartPtr<THeap, T>::AsWeak()
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return BaseWeakPtr<THeap, T>(This);
	};

	template<typename THeap, typename T>
	ForceInline decltype(auto) BaseSmartPtr<THeap, T>::GetTypeInfo() const
	{
		return typeid(*This->ptr);
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseSmartPtr<THeap, T>::GetWeakRefCount() const
	{
		return This->WeakRefCount;
	}

	template<typename THeap, typename T>
	ForceInline SizeT BaseSmartPtr<THeap, T>::GetRefCount() const
	{
		return This->RefCount;
	}

	template<typename THeap, typename T>
	ForceInline bool BaseSmartPtr<THeap, T>::operator==(const std::nullptr_t&) const
	{
		return This == nullptr || This->ptr == nullptr;
	}

	template<typename THeap, typename T>
	ForceInline bool BaseSmartPtr<THeap, T>::operator==(std::nullptr_t&&) const
	{
		return This == nullptr || This->ptr == nullptr;
	}

	template<typename THeap, typename T>
	void Swap(BaseSmartPtr<THeap, T>& a, BaseSmartPtr<THeap, T>& b)
	{
		Swap(a.This, b.This);
	}

}