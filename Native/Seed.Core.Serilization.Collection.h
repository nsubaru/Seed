#pragma once
#include "Seed.Core.Serilization.Base.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Serilization
        {
            namespace Details
            {
				template<typename T>
				constexpr bool IsSerilizableString = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::String
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<class THeap> typename TString, typename TStringHeap>
				struct BaseSerlizationHandler<THeap, TString<TStringHeap>, typename TypeTraits::EnableIfV<IsSerilizableString<TString<TStringHeap>>, void>> {
					using CharacterType = typename CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::Character;

					forceinline static void SerilizeImpl(const TString<TStringHeap>& data, Byte* rawStream, SizeT& offset) {
						SizeT Count = (data.Lenght + 1) * sizeof(CharacterType);
						BaseSerlizationHandler<THeap, SizeT>::SerilizeImpl(Count, rawStream, offset);
						Memory::Copy(reinterpret_cast<const Byte*>(const_cast<const Char*>(static_cast<const ConstCharStr>(data))),
									rawStream + offset,
									Count);
						offset += Count;
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const TString<TStringHeap>& data) {
						return ((data.Lenght + 1) * sizeof(CharacterType)) + sizeof(SizeT);
					}

					forceinline static constexpr auto TypeHint() {
						return
							CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::CodePage.Resize<
								CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::CodePage.Count - 1
						>();
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<TString<TStringHeap>>().Resize<CTTI::TypeName<TString<TStringHeap>>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TString<TStringHeap>)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, CharacterType>::TypeMetaData() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(TString<TStringHeap>& data, const Byte* rawStream, SizeT& offset) {
						SizeT Count;
						BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset);
						data = TString<TStringHeap>(reinterpret_cast<const CharStr>(const_cast<Byte*>(rawStream + offset)), (Count - sizeof(CharacterType)) / sizeof(CharacterType));
						offset += Count;
					}
				};

				template<typename T>
				constexpr bool IsSerilizablePair = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Pair
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<class THeap, typename TKey, typename TValue> typename TPair, typename TPairHeap, typename TKey, typename TValue>
				struct BaseSerlizationHandler<THeap, TPair<TPairHeap, TKey, TValue>, typename TypeTraits::EnableIfV<IsSerilizablePair<TPair<TPairHeap, TKey, TValue>>, void>> {
					forceinline static void SerilizeImpl(const TPair<TPairHeap, TKey, TValue>& data, Byte* rawStream, SizeT& offset) {
						BaseSerlizationHandler<THeap, TKey>::SerilizeImpl(data.Key, rawStream, offset);
						BaseSerlizationHandler<THeap, TValue>::SerilizeImpl(data.Value, rawStream, offset);
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const TPair<TPairHeap, TKey, TValue>& data) {
						return BaseSerlizationHandler<THeap, TKey>::DataSize(data.Key) +
							BaseSerlizationHandler<THeap, TValue>::DataSize(data.Value);
					}

					forceinline static constexpr auto TypeHint() {
						return CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<TPair<TPairHeap, TKey, TValue>>().Resize<CTTI::TypeName<TPair<TPairHeap, TKey, TValue>>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TPair<TPairHeap, TKey, TValue>>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TPair<TPairHeap, TKey, TValue>)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, TKey>::TypeMetaData() +
							BaseSerlizationHandler<THeap, TValue>::TypeMetaData() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(TPair<TPairHeap, TKey, TValue>& data, const Byte* rawStream, SizeT& offset) {
						TKey key;
						TValue value;
						BaseSerlizationHandler<THeap, TKey>::DeserelizeImpl(key, rawStream, offset);
						BaseSerlizationHandler<THeap, TValue>::DeserelizeImpl(value, rawStream, offset);
						data = TPair<TPairHeap, TKey, TValue>(MoveRValue(key), MoveRValue(value));
					}
				};

				template<typename T>
				constexpr bool IsSerilizableCollection = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Collection
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<typename TCollectionHeap, typename T> typename TCollection, typename TCollectionHeap, typename T>
				struct BaseSerlizationHandler<THeap, TCollection<TCollectionHeap, T>, typename TypeTraits::EnableIfV<IsSerilizableCollection<TCollection<TCollectionHeap, T>>, void>> {
					using TCollectionClass = TCollection<TCollectionHeap, T>;

					forceinline static void SerilizeImpl(const TCollectionClass& data, Byte* rawStream, SizeT& offset) {
						BaseSerlizationHandler<THeap, SizeT>::SerilizeImpl(data.Count, rawStream, offset);
						data.ForEach([&](const T& elem) mutable
							{
								BaseSerlizationHandler<THeap, T>::SerilizeImpl(elem, rawStream, offset);
							});
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const TCollectionClass& data) {
						if constexpr (BaseSerlizationHandler<THeap, T>::IsMemCopy())
						{
							return (data.Count * sizeof(T)) + sizeof(SizeT);
						}
						else
						{
							return data.Aggregate(
								[] (const SizeT& size, const T& item) -> SizeT
								{
									return size + BaseSerlizationHandler<THeap, T>::DataSize(item);
								},0Sz) 
								+ sizeof(SizeT);
						}
					}

					forceinline static constexpr auto TypeHint() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<TCollectionClass>().Resize<CTTI::TypeName<TCollectionClass>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(TCollectionClass& data, const Byte* rawStream, SizeT& offset) {
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							SizeT Count;
							BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset);

							if constexpr (!TCollectionClass::IsDynamic)
							{
								data = TCollectionClass(Count);
								for (SizeT i = 0; i < Count; i++)
								{
									T tmp = T();
									BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset);
									data.UnsafeSet(MoveRValue(tmp), i);
								}
							}
							else
							{
								data = TCollectionClass();
								for (SizeT i = 0; i < Count; i++)
								{
									T tmp = T();
									BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset);
									data.UnsafeAdd(MoveRValue(tmp));
								}
							}
						}
						else
						{
							Throw(NullPointException(ExceptionPosition));
						}
					}
				};

				template<typename T>
				constexpr bool IsSerilizableFixedCollection = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::FixedCollection
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<typename TCollectionHeap, SizeT TSize, typename T> typename TFixedCollection, typename TCollectionHeap, SizeT TSize, typename T>
				struct BaseSerlizationHandler<THeap, TFixedCollection<TCollectionHeap, TSize, T>, typename TypeTraits::EnableIfV<IsSerilizableFixedCollection<TFixedCollection<TCollectionHeap, TSize, T>>, void>> {
					using TCollectionClass = TFixedCollection<TCollectionHeap, TSize, T>;

					forceinline static void SerilizeImpl(const TCollectionClass& data, Byte* rawStream, SizeT& offset) {
						SizeT size = data.Count;
						BaseSerlizationHandler<THeap, SizeT>::SerilizeImpl(size, rawStream, offset);

						data.ForEach([&](const T& elem) mutable
							{
								BaseSerlizationHandler<THeap, T>::SerilizeImpl(elem, rawStream, offset);
							});
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const TCollectionClass& data) {
						if constexpr (BaseSerlizationHandler<THeap, T>::IsMemCopy())
						{
							return (data.Count * sizeof(T)) + sizeof(SizeT);
						}
						else
						{
							return data.Aggregate([](SizeT & size, const T & item)
								{
									size += BaseSerlizationHandler<THeap, T>::DataSize(item);
								}, 0ui64) + sizeof(SizeT);
						}
					}

					forceinline static constexpr auto TypeHint() {
						return
							CTTI::Details::ToCharStr<TSize>().Resize<CTTI::Details::ToCharStr<TSize>().Count - 1>();
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<TCollectionClass>().Resize<CTTI::TypeName<TCollectionClass>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(TCollectionClass& data, const Byte* rawStream, SizeT& offset) {
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							SizeT Count;
							BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset);

							data = TCollectionClass();
							for (SizeT i = 0; i < Count; i++)
							{
								T tmp = T();
								BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset);
								data.UnsafeSet(MoveRValue(tmp), i);
							}
						}
						else
						{
							Throw(NullPointException(ExceptionPosition));
						}
					}
				};

				template<typename T>
				constexpr bool IsSerilizableAssociableCollection = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::AssociableCollection
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				constexpr bool f = IsSerilizableAssociableCollection<Containers::Dictionary<Int32>>;

				template<typename THeap, template<typename TCollectionHeap, typename TKey, typename TValue> typename TAssociableCollection, typename TCollectionHeap, typename TKey, typename TValue>
				struct BaseSerlizationHandler<THeap, TAssociableCollection<TCollectionHeap, TKey, TValue>, typename TypeTraits::EnableIfV<IsSerilizableAssociableCollection<TAssociableCollection<TCollectionHeap, TKey, TValue>>, void>> {
					using TCollectionClass = TAssociableCollection<TCollectionHeap, TKey, TValue>;
					using T = typename TCollectionClass::DataType;

					forceinline static void SerilizeImpl(const TCollectionClass& data, Byte* rawStream, SizeT& offset) {
						BaseSerlizationHandler<THeap, SizeT>::SerilizeImpl(data.Count, rawStream, offset);
						data.ForEach([&](const T& elem) mutable
							{
								BaseSerlizationHandler<THeap, T>::SerilizeImpl(elem, rawStream, offset);
							});
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const TCollectionClass& data) {
						if constexpr (BaseSerlizationHandler<THeap, T>::IsMemCopy())
						{
							return (data.Count * sizeof(T)) + sizeof(SizeT);
						}
						else
						{
							SizeT size = sizeof(T);
							data.ForEach([&](const T& item){
								size += BaseSerlizationHandler<THeap, T>::DataSize(item);
							});
							return size;
						}
					}

					forceinline static constexpr auto TypeHint() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<TCollectionClass>().Resize<CTTI::TypeName<TCollectionClass>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(TCollectionClass& data, const Byte* rawStream, SizeT& offset) {
						SizeT Count;
						BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset);

						data = TCollectionClass();

						for (SizeT i = 0; i < Count; i++)
						{
							T tmp = T();
							BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset);
							data.UnsafeAdd(MoveRValue(tmp)); 
						}
					}
				};
            }
        }
    }
}