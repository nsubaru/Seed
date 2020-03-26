#pragma once
#include "Seed.Core.Containers.API.LINQ.IOrderable.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Клас LINQ розширення масиву
				/// </summary>
				/// <typeparam name="TArray">Тип масиву </typeparam>
				/// <typeparam name="THeap">Куча масиву </typeparam>
				/// <typeparam name="T">Тип елементів </typeparam>
				/// <typeparam name="TIsConcurencySuport">Вказує чи колекція підтримує багато поточність </typeparam>
				template<template<typename THeap, typename T>typename TArray, typename THeap, typename T, bool TIsConcurencySuport>
				class IArray :
					public IOrderable<TArray, THeap, T, false, TIsConcurencySuport> {
				public:
					forceinline virtual ~IArray() = 0 {}
				};
			}
		}
	}
}