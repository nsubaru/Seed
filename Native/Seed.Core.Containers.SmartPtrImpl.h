#pragma once
#include "Seed.Core.Containers.SmartPtr.h"

namespace Seed::Core
{
	template<typename THeap, typename T>
	bool BaseSmartPtr<THeap, T>::Dispose()
	{
		delete (T*) This->ptr;
		This->ptr = nullptr;

		return This->WeakRefCount == 0;
	}

	template<typename THeap, typename T>
	inline BaseSmartPtr<THeap, T>::BaseSmartPtr(T*&& ptr)
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
	inline BaseSmartPtr<THeap, T>::BaseSmartPtr(BaseUniquePtr<THeap, T>&& ptr)
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
	inline T& BaseSmartPtr<THeap, T>::operator*()
	{
		return *(T*) This->ptr;
	}

	template<typename THeap, typename T>
	inline const T& BaseSmartPtr<THeap, T>::operator*() const
	{
		return *(T*) This->ptr;
	}

	template<typename THeap, typename T>
	inline T* BaseSmartPtr<THeap, T>::operator->()
	{
		return (T*) This->ptr;
	}

	template<typename THeap, typename T>
	inline const T* BaseSmartPtr<THeap, T>::operator->() const
	{
		return (T*) This->ptr;
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	inline const TInterface& BaseSmartPtr<THeap, T>::As() const
	{
		return dynamic_cast<const TInterface&>(*(T*) This->ptr);
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	inline TInterface& BaseSmartPtr<THeap, T>::As()
	{
		return dynamic_cast<TInterface&>(*(T*) This->ptr);
	}

	template<typename THeap, typename T>
	template<typename U> requires Concepts::IsBasedOn<U, T> || Concepts::IsBasedOn<T, U>
	inline BaseSmartPtr<THeap, U> Seed::Core::BaseSmartPtr<THeap, T>::Cast() &
	{
		(void) dynamic_cast<U&>(*(T*) This->ptr);

		return BaseSmartPtr<THeap, U>(This);
	}

	template<typename THeap, typename T>
	template<typename U> requires Concepts::IsBasedOn<U, T> || Concepts::IsBasedOn<T, U>
	inline BaseSmartPtr<THeap, U> Seed::Core::BaseSmartPtr<THeap, T>::Cast() &&
	{
		(void) dynamic_cast<U&>(*(T*) This->ptr);

		auto ret = BaseSmartPtr<THeap, U>(nullptr); 
		Swap(this->This, ret.This);

		return ret;
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
	inline decltype(auto) BaseSmartPtr<THeap, T>::GetTypeInfo() const
	{
		return typeid(*(T*) This->ptr);
	}

	template<typename THeap, typename T>
	inline SizeT BaseSmartPtr<THeap, T>::GetWeakRefCount() const
	{
		return This->WeakRefCount;
	}

	template<typename THeap, typename T>
	inline SizeT BaseSmartPtr<THeap, T>::GetRefCount() const
	{
		return This->RefCount;
	}

	template<typename THeap, typename T>
	inline bool BaseSmartPtr<THeap, T>::operator==(const std::nullptr_t&) const
	{
		return This == nullptr || This->ptr == nullptr;
	}

	template<typename THeap, typename T>
	inline bool BaseSmartPtr<THeap, T>::operator==(std::nullptr_t&&) const
	{
		return This == nullptr || This->ptr == nullptr;
	}
}