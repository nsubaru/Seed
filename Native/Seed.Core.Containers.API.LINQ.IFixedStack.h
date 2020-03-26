#pragma once
#include "Seed.Core.Containers.API.LINQ.IFixedCollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Абстракція фіксованого стеку
				/// </summary>
				/// <typeparam name="TCollection">Тип фіксованого стеку/typeparam>
				/// <typeparam name="THeap">Тип кучі колекції</typeparam>
				/// <typeparam name="TSize">Кількість елементів в колекції</typeparam>
				/// <typeparam name="T">Тип елементів колекції</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class IFixedStack : public IFixedCollection<TCollection, THeap, T, TSize, TIsConcurencySuport> {
				public:
					forceinline virtual ~IFixedStack() = 0 {}
				};
			}
		}
	}
}