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
	inline T& BaseWeakPtr<THeap, T>::operator*()
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return *(T*) This->ptr;
	}

	template<typename THeap, typename T>
	inline const T& BaseWeakPtr<THeap, T>::operator*() const
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return (const T*) *This->ptr;
	}

	template<typename THeap, typename T>
	inline T* BaseWeakPtr<THeap, T>::operator->()
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return (T*) This->ptr;
	}

	template<typename THeap, typename T>
	inline const T* BaseWeakPtr<THeap, T>::operator->() const
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return (const T*) This->ptr;
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	inline const TInterface& BaseWeakPtr<THeap, T>::As() const
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return dynamic_cast<const TInterface&>(*(T*) This->ptr);
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	inline TInterface& BaseWeakPtr<THeap, T>::As()
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return dynamic_cast<TInterface&>(*(T*) This->ptr);
	}

	template<typename THeap, typename T>
	inline decltype(auto) BaseWeakPtr<THeap, T>::GetTypeInfo() const
	{
		if (This == nullptr || This->ptr == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return typeid(*(T*) This->ptr);
	}

	template<typename THeap, typename T>
	inline SizeT BaseWeakPtr<THeap, T>::GetWeakRefCount() const
	{
		if (This == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return This->WeakRefCount;
	}

	template<typename THeap, typename T>
	inline SizeT BaseWeakPtr<THeap, T>::GetRefCount() const
	{
		if (This == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		return This->RefCount;
	}

	template<typename THeap, typename T>
	inline Boolean BaseWeakPtr<THeap, T>::GetHasValue()
	{
		return !(This == nullptr || This->RefCount == 0);
	}

	template<typename THeap, typename T>
	inline bool BaseWeakPtr<THeap, T>::operator==(const std::nullptr_t&) const
	{
		return This == nullptr || This->ptr == nullptr;
	}

	template<typename THeap, typename T>
	inline bool BaseWeakPtr<THeap, T>::operator==(std::nullptr_t&&) const
	{
		return This == nullptr || This->ptr == nullptr;
	}

	template<typename THeap, typename T>
	inline BaseSmartPtr<THeap, T> Seed::Core::BaseWeakPtr<THeap, T>::CaptureRef()
	{
		if (This == nullptr)
		{
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		// --------------------------------------------------------------------------------------------------
		// Try to increment and check is data exist
		// If after increment we get value "1" then data was deleted or 
		//  being deleted but other thread alredy started execution of SmartPtr dtor 
		//  (RefCount.DecrementAndCheckIsZero was called and Dispose branch was selected) 
		// --------------------------------------------------------------------------------------------------
		// If we got into "then" branch then we should restore previous value of RefCount and throw exception
		if (This->RefCount.IncrementAndCheckWith(1))
		{
			--This->RefCount;
			Internals::Exceptions::ThrowNullPointException(SourcePosition);
		}

		Data* newThis = This;

		return BaseSmartPtr<THeap, T>(MoveRValue(newThis));
	}
}