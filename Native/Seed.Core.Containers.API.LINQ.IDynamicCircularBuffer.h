#pragma once
#include "Seed.Core.Containers.API.LINQ.ICircularBuffer.h"

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
				template<template<typename THeap, typename T>typename TCircularBuffer, typename THeap, typename T, bool TIsConcurencySuport>
				class IDynamicCircularBuffer :
					public ICircularBuffer<TCircularBuffer, THeap, T, TIsConcurencySuport> {
				private:
					using ThisCollection = typename TCircularBuffer<THeap, T>;
				public:
					void Resize(SizeT newSize) {
						static_cast<ThisCollection&>(*this).ResizeImpl(newSize);
					}
				public:
					virtual ~IDynamicCircularBuffer() = 0 {}
				};
			}
		}
	}
}