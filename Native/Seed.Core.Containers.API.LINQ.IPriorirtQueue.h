#pragma once
#include "Seed.Core.Containers.API.LINQ.IAssociableCollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// LINQ розширення пріоритетної черги
				/// </summary>
				/// <typeparam name="TCollection">Тип пріоритетної черги</typeparam>
				/// <typeparam name="THeap">Тип кучі</typeparam>
				/// <typeparam name="TKey">Тип ключа</typeparam>
				/// <typeparam name="TValue">Тип значення</typeparam>
				/// <typeparam name="TPair">Тип пари ключа - значення</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				class IPriorityQueue :
					public IAssociableCollection<TCollection, THeap, TKey, TValue, TPair, IsConcurencySuportT>
				{
				public:
					ForceInline virtual ~IPriorityQueue() = 0 {}
				};
			}
		}
	}
}