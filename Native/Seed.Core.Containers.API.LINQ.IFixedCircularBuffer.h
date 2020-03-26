#pragma once
#include "Seed.Core.Containers.API.LINQ.ICircularBuffer.h"
#include "Seed.Core.Containers.API.LINQ.IFixedCollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Абстракція фіксованого циклічного буфера
				/// </summary>
				/// <typeparam name="TCircularBuffer">Тип динамічного циклічного буфера</typeparam>
				/// <typeparam name="THeap">Тип кучі колекції</typeparam>
				/// <typeparam name="TSize">Кількість елементів</typeparam>
				/// <typeparam name="T">Тип елементів</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename THeap, SizeT TSize, typename T>typename TCircularBuffer, SizeT TSize, typename THeap, typename T, bool TIsConcurencySuport>
				class IFixedCircularBuffer :
					public ICircularBuffer<TCircularBuffer<THeap, TSize, T>, THeap, T, TIsConcurencySuport>,  
					public IFixedCollection<TCircularBuffer, THeap, T, TSize, TIsConcurencySuport> {
				private:
					using ThisCollection = typename TCircularBuffer<THeap, TSize, T>;
				public:
					forceinline virtual ~IFixedCircularBuffer() = 0 {}
				};
			}
		}
	}
}