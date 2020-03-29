#pragma once
#include "Seed.Core.Containers.API.LINQ.ICollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Абстракція динамічного стеку
				/// </summary>
				/// <typeparam name="TDynamicQueue">Тип динамічного стеку</typeparam>
				/// <typeparam name="THeap">Тип кучі колекції</typeparam>
				/// <typeparam name="T">Тип елементів</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename THeap, typename T>typename TDynamicStack, typename THeap, typename T, bool TIsConcurencySuport>
				class IDynamicStack :
					public ICollection<TDynamicStack, THeap, T, false, TIsConcurencySuport> {
				public:
					forceinline virtual ~IDynamicStack() = 0 {}
				};
			}
		}
	}
}