#pragma once
#include "Seed.Core.Containers.UniqueArrayPtr.h"

namespace Seed::Core
{
	template<typename THeap, typename T>
	constexpr BaseUniqueArrayPtr<THeap, T>::BaseUniqueArrayPtr(BaseUniqueArrayPtr&& old): data(old.data)
	{
		old.data = nullptr;
	}

	template<typename THeap, typename T>
	constexpr BaseUniqueArrayPtr<THeap, T>& BaseUniqueArrayPtr<THeap, T>::operator=(BaseUniqueArrayPtr&& old)
	{
		this->~BaseUniqueArrayPtr();
		this->BaseUniqueArrayPtr::BaseUniqueArrayPtr(MoveRValue(old));
		return *this;
	}

	template<typename THeap, typename T>
	constexpr T* BaseUniqueArrayPtr<THeap, T>::operator*()
	{
		return data;
	}

	template<typename THeap, typename T>
	constexpr const T* BaseUniqueArrayPtr<THeap, T>::operator*() const
	{
		return data;
	}

	template<typename THeap, typename T>
	BaseUniqueArrayPtr<THeap, T>::~BaseUniqueArrayPtr()
	{
		Memory::Allocator<THeap>().operator delete[](data);
		data = nullptr;
	}
}