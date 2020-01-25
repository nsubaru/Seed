#pragma once
#include "Seed.Core.NewSerilization.Base.h"

namespace Seed
{
    inline namespace Core
    {
        namespace NewSerilization
        {
            namespace Details
            {
				template<typename T>
				constexpr bool IsSerilizableString = NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::String
					&& NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<class THeap> typename TString, typename TStringHeap>
				struct BaseSerlizationHandler<THeap, TString<TStringHeap>, typename TypeTraits::EnableIfV<IsSerilizableString<TString<TStringHeap>>, void>> {
					using CharacterType = typename NewCTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::Character;

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
							NewCTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::CodePage.Resize<
								NewCTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::CodePage.Count - 1
						>();
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<TString<TStringHeap>>().Resize<NewCTTI::TypeName<TString<TStringHeap>>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TString<TStringHeap>)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, CharacterType>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(TString<TStringHeap>& data, const Byte* rawStream, SizeT& offset) {
						SizeT Count;
						BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset);
						data = TString<TStringHeap>(reinterpret_cast<const CharStr>(const_cast<Byte*>(rawStream + offset)), (Count - sizeof(CharacterType)) / sizeof(CharacterType));
						offset += Count;
					}
				};

				template<typename T>
				constexpr bool IsSerilizablePair = NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::Pair
					&& NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

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
						return NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<TPair<TPairHeap, TKey, TValue>>().Resize<NewCTTI::TypeName<TPair<TPairHeap, TKey, TValue>>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<TPair<TPairHeap, TKey, TValue>>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TPair<TPairHeap, TKey, TValue>)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, TKey>::TypeMetaData() +
							BaseSerlizationHandler<THeap, TValue>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
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
				constexpr bool IsSerilizableCollection = NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::Collection
					&& NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

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
							return data.Aggregate([](SizeT & size, const T & item)
								{
									size += BaseSerlizationHandler<THeap, T>::DataSize(item);
								}, 0ui64) + sizeof(SizeT);
						}
					}

					forceinline static constexpr auto TypeHint() {
						return
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<TCollectionClass>().Resize<NewCTTI::TypeName<TCollectionClass>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(TCollectionClass& data, const Byte* rawStream, SizeT& offset) {
						SizeT Count;
						BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset);
					   
						if constexpr (!TCollectionClass::IsDynamic){
							data = TCollectionClass(Count);
							for (SizeT i = 0; i < Count; i++)
							{
								T tmp = T();
								BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset);
								data.UnsafeSet(MoveRValue(tmp), i); 
							}
						}
						else {
							data = TCollectionClass();
							for (SizeT i = 0; i < Count; i++)
							{
								T tmp = T();
								BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset);
								data.UnsafeAdd(MoveRValue(tmp)); 
							}
						}
					}
				};

				template<typename T>
				constexpr bool IsSerilizableFixedCollection = NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::FixedCollection
					&& NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

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
							NewCTTI::Details::ToCharStr<TSize>().Resize<NewCTTI::Details::ToCharStr<TSize>().Count - 1>();
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<TCollectionClass>().Resize<NewCTTI::TypeName<TCollectionClass>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(TCollectionClass& data, const Byte* rawStream, SizeT& offset) {
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
				};

				template<typename T>
				constexpr bool IsSerilizableAssociableCollection = NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::AssociableCollection
					&& NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

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
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<TCollectionClass>().Resize<NewCTTI::TypeName<TCollectionClass>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
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