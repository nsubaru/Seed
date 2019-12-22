#pragma once
#include "Seed.Core.Containers.API.LINQ.IAssociableCollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				class IDictionary :
					public IAssociableCollection<TCollection, THeap, TKey, TValue, TPair, IsConcurencySuportT>
				{
				public:
					virtual ~IDictionary() = 0 {}
				};
			}
		}
	}
}