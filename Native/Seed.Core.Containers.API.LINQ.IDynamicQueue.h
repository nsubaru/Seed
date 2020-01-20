#pragma once
#include "Seed.Core.Containers.API.LINQ.ICollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Абстракція динамічної черги
				/// </summary>
				/// <typeparam name="TDynamicQueue">Тип динамічної черги</typeparam>
				/// <typeparam name="THeap">Тип кучі колекції</typeparam>
				/// <typeparam name="T">Тип елементів</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename THeap, typename T>typename TDynamicQueue, typename THeap, typename T, bool TIsConcurencySuport>
				class IDynamicQueue : 
					public ICollection<TDynamicQueue, THeap, T, false, TIsConcurencySuport> {
				public:
					virtual ~IDynamicQueue() = 0 {};
				};
			}
		}
	}
}