#pragma once
#include "Seed.Core.Containers.API.LINQ.IOrderable.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Клас LINQ розширення списку масивів(VList)
				/// </summary>
				/// <typeparam name="TArray">Тип списку масивів(VList) </typeparam>
				/// <typeparam name="THeap">Куча списку масивів(VList) </typeparam>
				/// <typeparam name="T">Тип елементів </typeparam>
				/// <typeparam name="TIsConcurencySuport">Вказує чи колекція підтримує багато поточність </typeparam>
				template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsConcurencySuport>
				class IVList :
					public ICollection<TList, THeap, T, false, TIsConcurencySuport> {
				public:
					ForceInline virtual ~IVList() = 0 {}
				};
			}
		}
	}
}