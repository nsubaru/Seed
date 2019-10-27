#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<
					typename TCollection,
					typename THeap,
					typename T,
					Boolean IsDynamicT,
					Boolean IsResizeableT,
					Boolean IsConcurencySupportT
				>
					struct SeedKernelCall TLinq {
					using CollectionType = TCollection;
					using HeapType = THeap;
					using DataType = T;
					static constexpr Boolean IsDynamic = IsDynamicT;
					static constexpr Boolean IsResizeable = IsResizeableT;
				};

				class SeedKernelCall ILinqBase { public: virtual ~ILinqBase() = 0 {}; };
				class SeedKernelCall ICollectionGenericBase { public: virtual ~ICollectionGenericBase() = 0 {}; };
				class SeedKernelCall IFixedCollectionGenericBase { public: virtual ~IFixedCollectionGenericBase() = 0 {}; };
				class SeedKernelCall IAssociableCollectionGenericBase { public: virtual ~IAssociableCollectionGenericBase() = 0 {}; };
#pragma region LINQ for one typearg Collection

				template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				class SeedKernelCall ICollectionBase :
					public ILinqBase,
					public ICollectionGenericBase,
					public TLinq<TCollection<THeap, T>, THeap, T, TIsDynamic, true, TIsConcurencySuport>
				{
				public:
					template<typename NewFixedCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<IFixedCollectionGenericBase, NewFixedCollection> &&
						TypeTraits::IsSameV<typename NewFixedCollection::DataType, typename TLinq<TCollection<THeap, T>, THeap, T, TIsDynamic, true, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						NewFixedCollection CopyToFixed(Boolean IgnoreSizeMismatch = false) {
						auto ThisCollectionRef = static_cast<const TCollection<THeap, T>&>(*this);
						if (IgnoreSizeMismatch) {
							if (NewFixedCollection::FixedSize == ThisCollectionRef.Count || NewFixedCollection::FixedSize > ThisCollectionRef.Count) {
								NewFixedCollection Instance;
								ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
									Instance.UnsafeSet(Item, Index);
									});
								return Instance;
							}
							else {
								NewFixedCollection Instance;
								ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
									Instance.UnsafeSet(Item, Index);
									}, NewFixedCollection::FixedSize);
								return Instance;
							}
						}
						else {
							if (NewFixedCollection::FixedSize != ThisCollectionRef.Count)
								//TODO: Throw error 
								return NewFixedCollection();
							else {
								NewFixedCollection Instance;
								ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
									Instance.UnsafeSet(Item, Index);
									});
								return Instance;
							}
						}
					}

					template<typename NewFixedCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<IFixedCollectionGenericBase, NewFixedCollection> &&
						TypeTraits::IsSameV<typename NewFixedCollection::DataType, typename TLinq<TCollection<THeap, T>, THeap, T, TIsDynamic, true, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						NewFixedCollection CopyToFixed(Boolean IgnoreSizeMismatch = false) const {
						auto ThisCollectionRef = static_cast<const TCollection<THeap, T>&>(*this);
						if (IgnoreSizeMismatch) {
							if (NewFixedCollection::FixedSize == ThisCollectionRef.Count || NewFixedCollection::FixedSize > ThisCollectionRef.Count) {
								NewFixedCollection Instance;
								ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
									Instance.UnsafeSet(Item, Index);
									});
								return Instance;
							}
							else {
								NewFixedCollection Instance;
								ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
									Instance.UnsafeSet(Item, Index);
									}, NewFixedCollection::FixedSize);
								return Instance;
							}
						}
						else {
							if (NewFixedCollection::FixedSize != ThisCollectionRef.Count)
								//TODO: Throw error 
								return NewFixedCollection();
							else {
								NewFixedCollection Instance;
								ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
									Instance.UnsafeSet(Item, Index);
									});
								return Instance;
							}
						}
					}
				public:
					virtual ~ICollectionBase() = 0 {}
				};
#pragma endregion
#pragma region LINQ for fixed size Collection
				template<typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				struct SeedKernelCall TFixedLinq : public TLinq<TCollection, THeap, T, false, false, TIsConcurencySuport> {
					static constexpr SizeT FixedSize = TSize;
				};

				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class SeedKernelCall IFixedCollectionBase :
					public ILinqBase,
					public IFixedCollectionGenericBase,
					public TFixedLinq<TCollection<THeap, TSize, T>, THeap, T, TSize, TIsConcurencySuport>
				{
				public:
					template<typename NewFixedCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewFixedCollection> &&
						TypeTraits::IsSameV<typename NewFixedCollection::DataType, typename TFixedLinq<TCollection<THeap, TSize, T>, THeap, T, TSize, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						NewFixedCollection CopyTo() {
						if constexpr (!NewFixedCollection::IsDynamic) {
							auto ThisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							NewFixedCollection Instance(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Value, SizeT Index) {
								Instance.UnsafeSet(Value, Index);
								});
							return Instance;
						}
						else {
							auto ThisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							NewFixedCollection Instance;
							ThisCollectionRef.ForEach([&](const T& Value) {
								Instance.UnsafeAdd(Value);
								});
							return Instance;
						}
					}

					template<typename NewFixedCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewFixedCollection> &&
						TypeTraits::IsSameV<typename NewFixedCollection::DataType, typename TFixedLinq<TCollection<THeap, TSize, T>, THeap, T, TSize, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						const NewFixedCollection CopyTo() const {
						if constexpr (!NewFixedCollection::IsDynamic) {
							auto ThisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							NewFixedCollection Instance(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Value, SizeT Index) {
								Instance.UnsafeSet(Value, Index);
								});
							return Instance;
						}
						else {
							auto ThisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							NewFixedCollection Instance;
							ThisCollectionRef.ForEach([&](const T& Value) {
								Instance.UnsafeAdd(Value);
								});
							return Instance;
						}
					}

					template<typename NewFixedCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewFixedCollection> &&
						TypeTraits::IsSameV<typename NewFixedCollection::DataType, typename TFixedLinq<TCollection<THeap, TSize, T>, THeap, T, TSize, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						NewFixedCollection MoveTo() {
						if constexpr (!NewFixedCollection::IsDynamic) {
							auto ThisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							NewFixedCollection Instance(ThisCollectionRef.Count);
							ThisCollectionRef.RemoceAll([&](T&& Value, SizeT Index) {
								Instance.UnsafeSet(MoveRValue(Value), Index);
								});
							return Instance;
						}
						else {
							auto ThisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							NewFixedCollection Instance;
							ThisCollectionRef.RemoceAll([&](T&& Value) {
								Instance.UnsafeAdd(MoveRValue(Value));
								});
							return Instance;
						}
					}
				public:
					virtual ~IFixedCollectionBase() = 0 {}
				};
#pragma endregion
#pragma region LINQ for associable Collection

				template<typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, Boolean TIsConcurencySuport>
				struct SeedKernelCall TAssociableLinq : public TLinq<TCollection, THeap, TPair, true, true, TIsConcurencySuport> {
					using KeyType = TKey;
					using ValueType = TValue;
				};

				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				class SeedKernelCall IAssociableCollectionBase :
					public ILinqBase,
					public IAssociableCollectionGenericBase,
					public TAssociableLinq<TCollection<THeap, TKey, TValue>, THeap, TKey, TValue, TPair, true>
				{
				public:
					virtual ~IAssociableCollectionBase() = 0 {}
				};
#pragma endregion
#pragma region LINQ for Pair
				template<typename TPair, typename THeap, typename TKey, typename TValue, Boolean TIsConcurencySuport>
				struct SeedKernelCall TPairLinq : public TLinq<TPair, THeap, TPair, false, false, TIsConcurencySuport> {
					using KeyType = TKey;
					using ValueType = TValue;
				};
#pragma endregion
#pragma region LINQ for String
				template<typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
				struct SeedKernelCall TStringLinq : public TLinq<TString, THeap, TChar, false, true, TIsConcurencySuport> {
					using CharacterType = TChar;
				};

				template<template<typename THeap> typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
				class SeedKernelCall IStringCollectionBaseLinq : public TStringLinq<TString<THeap>, THeap, TChar, TIsConcurencySuport> {
				public:
					virtual ~IStringCollectionBaseLinq() = 0 {};
				};
#pragma endregion
			}
		}
	}
}