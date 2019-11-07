#pragma once
#include "Seed.Core.Containers.API.LINQ.IFixedCollection.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Containers
		{
			namespace LINQ
			{
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class SeedKernelCall IBitMapT : public IFixedCollection<TCollection, THeap, T, TSize, TIsConcurencySuport>
				{
					using ThisCollection = TCollection<THeap, TSize, T>;
				public:
					virtual ~IBitMapT() = 0
					{}
				};
			}
		}
	}
}