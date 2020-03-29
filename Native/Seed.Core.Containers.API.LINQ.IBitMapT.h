#pragma once
#include "Seed.Core.Containers.API.LINQ.IFixedCollection.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Containers
		{
			namespace LINQ
			{
				/// <summary>
				/// LINQ розширення для бітової карти
				/// </summary>
				/// <typeparam name="TCollection">Тип бітової карти</typeparam>
				/// <typeparam name="THeap">Тип кучі</typeparam>
				/// <typeparam name="T">Тип елементів</typeparam>
				/// <typeparam name="TSize">Кількість бітів</typeparam>
				/// <typeparam name="TIsConcurencySuport">True якщо підтримує багато потоковість</typeparam>
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class IBitMapT : public IFixedCollection<TCollection, THeap, T, TSize, TIsConcurencySuport>
				{
					using ThisCollection = TCollection<THeap, TSize, T>;
				public:
					forceinline virtual ~IBitMapT() = 0
					{}
				};
			}
		}
	}
}