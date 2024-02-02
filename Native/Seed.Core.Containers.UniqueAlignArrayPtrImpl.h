#pragma once
#include "Seed.Core.Containers.UniqueAlignArrayPtr.h"

namespace Seed::Core
{
	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	constexpr BaseUniqueAlignArrayPtr<THeap, T, TAlignment>::BaseUniqueAlignArrayPtr(BaseUniqueAlignArrayPtr&& old): data(old.data)
	{
		old.data = nullptr;
	}

	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	constexpr BaseUniqueAlignArrayPtr<THeap, T, TAlignment>& 
	BaseUniqueAlignArrayPtr<THeap, T, TAlignment>::operator=(BaseUniqueAlignArrayPtr&& old)
	{
		this->~BaseUniqueAlignArrayPtr();
		this->BaseUniqueAlignArrayPtr::BaseUniqueAlignArrayPtr(MoveRValue(old));
		return *this;
	}

	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	constexpr T* BaseUniqueAlignArrayPtr<THeap, T, TAlignment>::operator*()
	{
		return data;
	}

	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	constexpr const T* BaseUniqueAlignArrayPtr<THeap, T, TAlignment>::operator*() const
	{
		return data;
	}

	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	BaseUniqueAlignArrayPtr<THeap, T, TAlignment>::~BaseUniqueAlignArrayPtr()
	{
		Memory::Allocator<THeap>().operator delete[](data, static_cast<AllignRequirements>(TAlignment.Bytes));
		data = nullptr;
	}
}