#pragma once
#include "Seed.Core.NewCTTI.TypeTraits.Base.h"
#include "Seed.Core.NewCTTI.TypeTraits.Primitives.h"
#include "Seed.Core.NewCTTI.TypeTraits.HeapMetaData.h"
#include "Seed.Core.NewCTTI.TypeTraits.String.h"

namespace Seed
{
	inline namespace Core
	{
		namespace NewCTTI
		{
			namespace TypeTraits
			{
				namespace Details
				{
					template<typename THeap, typename TCollection, typename TItemType, bool TIsConcurencySupport, EType TType, bool TIsConst, bool TIsVolatile>
					struct BaseICollection :
						BaseHeapCTTITypeData<TCollection, THeap, TType, true, TIsConst, TIsVolatile>
					{
						using ItemType = TItemType;
						using ItemTypeMetaData = CTTITypeData<TItemType>;
						constexpr static Boolean IsConcurencySupport = TIsConcurencySupport;
					};

					//Pair

					template<typename THeap, typename TKey, typename TValue, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BasePair<THeap, TKey, TValue>, TIsConst, TIsVolatile> :
						BaseHeapCTTITypeData<Containers::BasePair<THeap, TKey, TValue>, THeap, EType::Pair, true, TIsConst, TIsVolatile>
					{
						using KeyType = TKey;
						using ValueType = TValue;

						using KeyTypeMetaData = CTTITypeData<TKey>;
						using ValueTypeMetaData = CTTITypeData<TValue>;
					};

#pragma region Collections
					//Base implementation

					template<typename THeap, typename TCollection, typename TItemType, bool TIsConcurencySupport, bool TIsConst, bool TIsVolatile>
					struct BaseLINQCollection :
						BaseICollection<THeap, TCollection, TItemType, TIsConcurencySupport, EType::Collection, TIsConst, TIsVolatile>
					{};


					//Array

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseArray<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseArray<THeap, T>, T, false, TIsConst, TIsVolatile>
					{};

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyArray<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseConcurencyArray<THeap, T>, T, true, TIsConst, TIsVolatile>
					{};

					//VList

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseVList<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseVList<THeap, T>, T, false, TIsConst, TIsVolatile>
					{};

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyVList<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseConcurencyVList<THeap, T>, T, true, TIsConst, TIsVolatile>
					{};

					//DynamicQueue

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseDynamicQueue<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseDynamicQueue<THeap, T>, T, false, TIsConst, TIsVolatile>
					{};

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyDynamicQueue<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseConcurencyDynamicQueue<THeap, T>, T, true, TIsConst, TIsVolatile>
					{};

					//DynamicStack

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseDynamicStack<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseDynamicStack<THeap, T>, T, false, TIsConst, TIsVolatile>
					{};

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyDynamicStack<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseConcurencyDynamicStack<THeap, T>, T, true, TIsConst, TIsVolatile>
					{};

					//List

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseList<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseList<THeap, T>, T, false, TIsConst, TIsVolatile>
					{};

					template<typename THeap, typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyList<THeap, T>, TIsConst, TIsVolatile> :
						BaseLINQCollection<THeap, Containers::BaseConcurencyList<THeap, T>, T, true, TIsConst, TIsVolatile>
					{};
#pragma endregion
#pragma region Fixed collection
					//Base implementation

					template<typename THeap, typename TCollection, SizeT TSize, typename TItemType, bool TIsConcurencySupport, bool TIsConst, bool TIsVolatile>
					struct BaseLINQFixedCollection :
						BaseICollection<THeap, TCollection, TItemType, TIsConcurencySupport, EType::FixedCollection, TIsConst, TIsVolatile>
					{
						static constexpr auto ItemCount = TSize;
					};

					//Fixed Array

					template<typename THeap, typename T, SizeT TSize, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseFixedArray<THeap, TSize, T>, TIsConst, TIsVolatile> :
						BaseLINQFixedCollection<THeap, Containers::BaseFixedArray<THeap, TSize, T>, TSize, T, false, TIsConst, TIsVolatile>
					{};

					template<typename THeap, typename T, SizeT TSize, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyFixedArray<THeap, TSize, T>, TIsConst, TIsVolatile> :
						BaseLINQFixedCollection<THeap, Containers::BaseConcurencyFixedArray<THeap, TSize, T>, TSize, T, true, TIsConst, TIsVolatile>
					{};

					//Fixed Queue

					template<typename THeap, typename T, SizeT TSize, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseFixedQueue<THeap, TSize, T>, TIsConst, TIsVolatile> :
						BaseLINQFixedCollection<THeap, Containers::BaseFixedQueue<THeap, TSize, T>, TSize, T, false, TIsConst, TIsVolatile>
					{};

					template<typename THeap, typename T, SizeT TSize, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyFixedQueue<THeap, TSize, T>, TIsConst, TIsVolatile> :
						BaseLINQFixedCollection<THeap, Containers::BaseConcurencyFixedQueue<THeap, TSize, T>, TSize, T, true, TIsConst, TIsVolatile>
					{};

					//Fixed Queue

					template<typename THeap, typename T, SizeT TSize, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseFixedStack<THeap, TSize, T>, TIsConst, TIsVolatile> :
						BaseLINQFixedCollection<THeap, Containers::BaseFixedStack<THeap, TSize, T>, TSize, T, false, TIsConst, TIsVolatile>
					{};

					template<typename THeap, typename T, SizeT TSize, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyFixedStack<THeap, TSize, T>, TIsConst, TIsVolatile> :
						BaseLINQFixedCollection<THeap, Containers::BaseConcurencyFixedStack<THeap, TSize, T>, TSize, T, true, TIsConst, TIsVolatile>
					{};
#pragma endregion
#pragma region Associable Collection
					//Base implementation

					template<typename THeap, typename TCollection, typename TPair, typename TKey, typename TValue, bool TIsConst, bool TIsVolatile>
					struct BaseLINQAssociableCollection :
						BaseICollection<THeap, TCollection, TPair, TIsConcurencySupport, EType::Collection, TIsConst, TIsVolatile>
					{
						using PairType = TPair;
						using KeyType = TKey;
						using ValueType = TValue;

						using PairTypeMetaData = CTTITypeData<TPair>;
						using KeyTypeMetaData = CTTITypeData<TKey>;
						using ValueTypeMetaData = CTTITypeData<TValue>;
					};

					//Dictionary

					template<typename THeap, typename TKey, typename TValue, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseDictionary<THeap, TKey, TValue>, TIsConst, TIsVolatile> :
						BaseLINQAssociableCollection<
						THeap,
						Containers::BaseDictionary<THeap, TKey, TValue>,
						typename Containers::BaseDictionary<THeap, TKey, TValue>::DataType,
						TKey,
						TValue,
						false,
						TIsConst,
						TIsVolatile>
					{};

					template<typename THeap, typename TKey, typename TValue, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyDictionary<THeap, TKey, TValue>, TIsConst, TIsVolatile> :
						BaseLINQAssociableCollection<
						THeap,
						Containers::BaseConcurencyDictionary<THeap, TKey, TValue>,
						typename Containers::BaseConcurencyDictionary<THeap, TKey, TValue>::DataType,
						TKey,
						TValue,
						true,
						TIsConst,
						TIsVolatile>
					{};

					//Map

					template<typename THeap, typename TKey, typename TValue, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseMap<THeap, TKey, TValue>, TIsConst, TIsVolatile> :
						BaseLINQAssociableCollection<
						THeap,
						Containers::BaseMap<THeap, TKey, TValue>,
						typename Containers::BaseMap<THeap, TKey, TValue>::DataType,
						TKey,
						TValue,
						false,
						TIsConst,
						TIsVolatile>
					{};

					template<typename THeap, typename TKey, typename TValue, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyMap<THeap, TKey, TValue>, TIsConst, TIsVolatile> :
						BaseLINQAssociableCollection<
						THeap,
						Containers::BaseConcurencyMap<THeap, TKey, TValue>,
						typename Containers::BaseConcurencyMap<THeap, TKey, TValue>::DataType,
						TKey,
						TValue,
						true,
						TIsConst,
						TIsVolatile>
					{};

					//Priority Queue

					template<typename THeap, typename TKey, typename TValue, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BasePriorityQueue<THeap, TKey, TValue>, TIsConst, TIsVolatile> :
						BaseLINQAssociableCollection<
						THeap,
						Containers::BasePriorityQueue<THeap, TKey, TValue>,
						typename Containers::BasePriorityQueue<THeap, TKey, TValue>::DataType,
						TKey,
						TValue,
						false,
						TIsConst,
						TIsVolatile>
					{};

					template<typename THeap, typename TKey, typename TValue, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseConcurencyPriorityQueue<THeap, TKey, TValue>, TIsConst, TIsVolatile> :
						BaseLINQAssociableCollection<
						THeap,
						Containers::BaseConcurencyPriorityQueue<THeap, TKey, TValue>,
						typename Containers::BaseConcurencyPriorityQueue<THeap, TKey, TValue>::DataType,
						TKey,
						TValue,
						true,
						TIsConst,
						TIsVolatile>
					{};
#pragma endregion
#pragma region Special Collection
					//BitMapT

					template<typename THeap, typename T, SizeT TSize, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::BaseBitMapT<THeap, TSize, T>, TIsConst, TIsVolatile>
						: BaseLINQFixedCollection<THeap, Containers::BaseBitMapT<THeap, TSize, T>, Containers::BaseBitMapT<THeap, TSize, T>::Size, T, false, TIsConst, TIsVolatile>
					{
						static constexpr SizeT FullBytesCount = Containers::BaseBitMapT<THeap, TSize>::FullBytesCount;
						static constexpr SizeT LastByteBitsCount = Containers::BaseBitMap<THeap, TSize>::LastByteBitsCount;
						static constexpr SizeT BitCountInFragment = Containers::BaseBitMap<THeap, TSize>::BitCountInFragment;
					};

					//Tuple

					template<typename ... Types, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<Containers::Tuple<Types...>, TIsConst, TIsVolatile>
						: BaseCTTITypeDataImpl<
						Containers::Tuple<Types...>,
						EType::Tuple,
						Seed::TypeTraits::And<CTTITypeData<Types>::IsSerilizable...>,
						TIsConst,
						TIsVolatile
						>
					{};
#pragma endregion
				}
			}
		}
	}
}