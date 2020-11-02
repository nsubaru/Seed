#pragma once
#include "Seed.Core.CTTI.TypeTraits.BaseTypes.h"

namespace Seed
{
	inline namespace Core
	{
		namespace CTTI
		{
			namespace TypeTraits
			{
				namespace Details
				{
					template<typename THeap>
					struct HeapCTTIMetaData
					{

					};

					template<>
					struct HeapCTTIMetaData<Memory::Heap>
					{
						using Type = Memory::Heap;
						static SizeT CurrentMemBlkCount()
						{
							return Memory::Heap().CurrentMemBlkCount();
						}

						static Memory::DataSizeT CurrentAllocatedMemory()
						{
							return Memory::Heap().CurrentAllocatedMemory();
						}

						static Memory::DataSizeT CurrentHeapUsage()
						{
							return Memory::Heap().CurrentHeapUsage();
						}

						static auto SettingsHive()
						{
							return CurrentSettings().MemoryManager.DefaultHeap;
						}
						
					};

					template<>
					struct HeapCTTIMetaData<Memory::SensitiveDataHeap>
					{
						using Type = Memory::SensitiveDataHeap;
						static SizeT CurrentMemBlkCount()
						{
							return Memory::SensitiveDataHeap().CurrentMemBlkCount();
						}

						static Memory::DataSizeT CurrentAllocatedMemory()
						{
							return Memory::SensitiveDataHeap().CurrentAllocatedMemory();
						}

						static Memory::DataSizeT CurrentHeapUsage()
						{
							return Memory::SensitiveDataHeap().CurrentHeapUsage();
						}

						static auto SettingsHive()
						{
							return CurrentSettings().MemoryManager.SensitiveDataHeap;
						}

					};
#pragma region Heap allocable base implementation
					template<typename T, typename THeap, EType TTypeClass, bool TIsSerilizable, bool TIsConst, bool TIsVolatile>
					struct BaseHeapCTTITypeData : BaseCTTITypeDataImpl<T, TTypeClass, TIsSerilizable, TIsConst, TIsVolatile>
					{
						using HeapType = THeap;

						using HeapMetaData = HeapCTTIMetaData<THeap>;
					};
#pragma endregion
				}
			}
		}
	}
}