#pragma once
#include "Seed.Core.Containers.UniqueAlignPtr.h"

namespace Seed::Core
{
	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	constexpr BaseUniqueAlignPtr<THeap, T, TAlignment>::BaseUniqueAlignPtr(BaseUniqueAlignPtr&& old): data(old.data)
	{
		old.data = nullptr;
	}

	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	constexpr BaseUniqueAlignPtr<THeap, T, TAlignment>& BaseUniqueAlignPtr<THeap, T, TAlignment>::operator=(BaseUniqueAlignPtr&& old)
	{
		this->~BaseUniqueAlignPtr();
		this->BaseUniqueAlignPtr::BaseUniqueAlignPtr(MoveRValue(old));
		return *this;
	}

	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	constexpr T* BaseUniqueAlignPtr<THeap, T, TAlignment>::operator*()
	{
		return data;
	}

	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	constexpr const T* BaseUniqueAlignPtr<THeap, T, TAlignment>::operator*() const
	{
		return data;
	}

	template<typename THeap, typename T, Memory::DataSizeT TAlignment>
	BaseUniqueAlignPtr<THeap, T, TAlignment>::~BaseUniqueAlignPtr()
	{
		Memory::Allocator<THeap>().operator delete(data, static_cast<AllignRequirements>(TAlignment.Bytes));
		data = nullptr;
	}
}