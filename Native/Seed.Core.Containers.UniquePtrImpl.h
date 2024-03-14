#pragma once
#include "Seed.Core.Containers.UniquePtr.h"

namespace Seed::Core
{
	template<typename THeap, typename T>
	constexpr BaseUniquePtr<THeap, T>::BaseUniquePtr(BaseUniquePtr&& old): data(old.data)
	{
		old.data = nullptr;
	}

	template<typename THeap, typename T>
	constexpr BaseUniquePtr<THeap, T>& BaseUniquePtr<THeap, T>::operator=(BaseUniquePtr&& old)
	{
		this->~BaseUniquePtr();
		this->BaseUniquePtr::BaseUniquePtr(MoveRValue(old));
		return *this;
	}

	template<typename THeap, typename T>
	template<typename TInterface> requires Concepts::IsBasedOn<TInterface, T> || Concepts::IsBasedOn<T, TInterface>
	ForceInline BaseUniquePtr<THeap, TInterface> BaseUniquePtr<THeap, T>::As()&&
	{
		TInterface* newPtr = nullptr;

		if constexpr (Concepts::IsBasedOn<TInterface, T>)
		{
			newPtr = static_cast<TInterface*>(this->data);
		}
		else
		{
			newPtr = dynamic_cast<TInterface*>(this->data);
		}

		if (newPtr != nullptr)
		{
			this->data = nullptr;
		}

		return BaseUniquePtr<THeap, TInterface>(MoveRValue(newPtr));
	}

	template<typename THeap, typename T>
	T* BaseUniquePtr<THeap, T>::ToRaw()&&
	{
		T* res = nullptr;
		Swap(data, res);
		return res;
	}

	template<typename THeap, typename T>
	constexpr T* BaseUniquePtr<THeap, T>::operator*()
	{
		return data;
	}

	template<typename THeap, typename T>
	constexpr const T* BaseUniquePtr<THeap, T>::operator*() const
	{
		return data;
	}

	template<typename THeap, typename T>
	constexpr T* BaseUniquePtr<THeap, T>::ReleasePtr()
	{
		auto ptr = data;
		data = nullptr;
		return ptr;
	}

	template<typename THeap, typename T>
	BaseUniquePtr<THeap, T>::~BaseUniquePtr()
	{
		if (data != nullptr)
		{
			if constexpr (
				!(
					TypeTraits::IsVoidV<T> ||
					TypeTraits::IsNumeric<T> ||
					TypeTraits::IsStandardLayout<T>
					) ||
				TypeTraits::IsHaveVirtualMethods<T> ||
				TypeTraits::HasVirtualDtor<T>
				)
			{
				delete data;
			}
			else
			{
				Memory::Allocator<THeap>().operator delete(data);
			}
		}

		data = nullptr;
	}
}