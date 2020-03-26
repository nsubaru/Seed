#pragma once
#include "Seed.Core.Containers.API.LINQ.IAssociableCollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Абстракція для LINQ методів словника
				/// </summary>
				/// <typeparam name="TCollection">Тип колекції словника</typeparam>
				/// <typeparam name="THeap">Тип кучу колекції</typeparam>
				/// <typeparam name="TKey">Тип ключа елемента</typeparam>
				/// <typeparam name="TValue">Тип значення елеменів</typeparam>
				/// <typeparam name="TPair">Тип пари ключа і значення</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool TIsConcurencySuport>
				class IDictionary :
					public IAssociableCollection<TCollection, THeap, TKey, TValue, TPair, TIsConcurencySuport>
				{
				public:
					forceinline virtual ~IDictionary() = 0 {}
				};
			}
		}
	}
}