#pragma once
#include "Seed.Core.Serilization.Core.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Serilization
        {
            namespace Details
            {
				template<typename T>
				constexpr bool IsSerilizableString = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::String
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<class THeap> typename TString, typename TStringHeap>
				struct BaseSerlizationHandler<THeap, TString<TStringHeap>, typename TypeTraits::EnableIfV<IsSerilizableString<TString<TStringHeap>>, void>> {
					using CharacterType = typename CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::Character;

					ForceInline static void SerilizeImpl(const TString<TStringHeap>& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						SizeT Count = (data.Lenght + 1) * sizeof(CharacterType);
						BaseSerlizationHandler<THeap, SizeT>::SerilizeImpl(Count, rawStream, offset, registry);
						Memory::Copy(reinterpret_cast<const Byte*>(const_cast<const Char*>(static_cast<const ConstCharStr>(data))),
									rawStream + offset,
									Count);
						offset += Count;
					}

					ForceInline static constexpr Boolean IsMemCopy() {
						return false;
					}

					ForceInline static constexpr SizeT DataSize(const TString<TStringHeap>& data, UnusedData const TypesRegistry& registry) {
						return ((data.Lenght + 1) * sizeof(CharacterType)) + sizeof(SizeT);
					}

					ForceInline static constexpr auto TypeHint() {
						return
							CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::CodePage.Resize<
								CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::CodePage.Count - 1
						>();
					}

					ForceInline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::CreateTypeStr(L"0");
					}

					ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<TString<TStringHeap>>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(TString<TStringHeap>)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            BaseSerlizationHandler<THeap, CharacterType>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					ForceInline static void DeserelizeImpl(TString<TStringHeap>& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						SizeT Count = 0;
						BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset, registry);
						data = TString<TStringHeap>(reinterpret_cast<const CharStr>(const_cast<Byte*>(rawStream + offset)), (Count - sizeof(CharacterType)) / sizeof(CharacterType));
						offset += Count;
					}

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return TypeMetaDataRecord(
							CTTI::TypeName<TString<TStringHeap>>(),
							CTTI::TypeTraits::CTTITypeMetaData<TString<TStringHeap>>::TypeClass,
							sizeof(TString<TStringHeap>),
							0,
							TypeHint(),
							{ BaseSerlizationHandler<THeap, CharacterType>::GetTypeMetaDataRecord() }
						);
					}
				};

				template<typename T>
				constexpr bool IsSerilizablePair = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Pair
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<class THeap, typename TKey, typename TValue> typename TPair, typename TPairHeap, typename TKey, typename TValue>
				struct BaseSerlizationHandler<THeap, TPair<TPairHeap, TKey, TValue>, typename TypeTraits::EnableIfV<IsSerilizablePair<TPair<TPairHeap, TKey, TValue>>, void>> {
					ForceInline static void SerilizeImpl(const TPair<TPairHeap, TKey, TValue>& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						BaseSerlizationHandler<THeap, TKey>::SerilizeImpl(data.Key, rawStream, offset, registry);
						BaseSerlizationHandler<THeap, TValue>::SerilizeImpl(data.Value, rawStream, offset, registry);
					}

					ForceInline static constexpr Boolean IsMemCopy() {
						return false;
					}

					ForceInline static constexpr SizeT DataSize(const TPair<TPairHeap, TKey, TValue>& data, UnusedData const TypesRegistry& registry) {
						return BaseSerlizationHandler<THeap, TKey>::DataSize(data.Key, registry) +
							BaseSerlizationHandler<THeap, TValue>::DataSize(data.Value, registry);
					}

					ForceInline static constexpr auto TypeHint() {
						return CTTI::Details::CreateTypeStr(L"");
					}

					ForceInline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::CreateTypeStr(L"0");
					}

					ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<TPair<TPairHeap, TKey, TValue>>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TPair<TPairHeap, TKey, TValue>>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(TPair<TPairHeap, TKey, TValue>)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            BaseSerlizationHandler<THeap, TKey>::TypeMetaData() +
							BaseSerlizationHandler<THeap, TValue>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					ForceInline static void DeserelizeImpl(TPair<TPairHeap, TKey, TValue>& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						TKey key{};
						TValue value{};
						BaseSerlizationHandler<THeap, TKey>::DeserelizeImpl(key, rawStream, offset, registry);
						BaseSerlizationHandler<THeap, TValue>::DeserelizeImpl(value, rawStream, offset, registry);
						data = TPair<TPairHeap, TKey, TValue>(MoveRValue(key), MoveRValue(value));
					}

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return BaseTypeMetaDataRecord<THeap>(
							CTTI::TypeName<TPair<TPairHeap, TKey, TValue>>(),
							CTTI::TypeTraits::CTTITypeMetaData<TPair<TPairHeap, TKey, TValue>>::TypeClass,
							sizeof(TPair<TPairHeap, TKey, TValue>),
							0,
							TypeHint(),
							{ BaseSerlizationHandler<THeap, TKey>::GetTypeMetaDataRecord(), BaseSerlizationHandler<THeap, TValue>::GetTypeMetaDataRecord() }
						);
					}
				};

				template<typename T>
				constexpr bool IsSerilizableCollection = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Collection
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<typename TCollectionHeap, typename T> typename TCollection, typename TCollectionHeap, typename T>
				struct BaseSerlizationHandler<THeap, TCollection<TCollectionHeap, T>, typename TypeTraits::EnableIfV<IsSerilizableCollection<TCollection<TCollectionHeap, T>>, void>> {
					using TCollectionClass = TCollection<TCollectionHeap, T>;

					ForceInline static void SerilizeImpl(const TCollectionClass& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						BaseSerlizationHandler<THeap, SizeT>::SerilizeImpl(data.Count, rawStream, offset, registry);
						data.ForEach([&](const T& elem) mutable InlineLambda
							{
								BaseSerlizationHandler<THeap, T>::SerilizeImpl(elem, rawStream, offset, registry);
							});
					}

					ForceInline static constexpr Boolean IsMemCopy() {
						return false;
					}

					ForceInline static constexpr SizeT DataSize(const TCollectionClass& data, UnusedData const TypesRegistry& registry) {
						if constexpr (BaseSerlizationHandler<THeap, T>::IsMemCopy())
						{
							return (data.Count * sizeof(T)) + sizeof(SizeT);
						}
						else
						{
							return data.Aggregate(
								[&] (const SizeT& size, const T& item) -> SizeT InlineLambda
								{
									return size + BaseSerlizationHandler<THeap, T>::DataSize(item, registry);
								},0Sz) 
								+ sizeof(SizeT);
						}
					}

					ForceInline static constexpr auto TypeHint() {
						return
							CTTI::Details::CreateTypeStr(L"");
					}

					ForceInline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::CreateTypeStr(L"0");
					}

					ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<TCollectionClass>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					ForceInline static void DeserelizeImpl(TCollectionClass& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							SizeT Count = 0;
							BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset, registry);

							if constexpr (!TCollectionClass::IsDynamic)
							{
								data = TCollectionClass(Count);

								for (SizeT i = 0; i < Count; i++)
								{
									T tmp = T();
									BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset, registry);
									data.UnsafeSet(MoveRValue(tmp), i);
								}
							}
							else
							{
								data = TCollectionClass();

								for (SizeT i = 0; i < Count; i++)
								{
									T tmp;
									BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset, registry);
									data.UnsafeAdd(MoveRValue(tmp));
								}
							}
						}
						else
						{
							Throw(NullPointException(ExceptionPosition));
						}
					}

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return BaseTypeMetaDataRecord<THeap>(
							CTTI::TypeName<TCollectionClass>(),
							CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass,
							sizeof(TCollectionClass),
							0,
							TypeHint(),
							{ BaseSerlizationHandler<THeap, T>::GetTypeMetaDataRecord() }
						);
					}
				};

				template<typename T>
				constexpr bool IsSerilizableFixedCollection = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::FixedCollection
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<typename TCollectionHeap, SizeT TSize, typename T> typename TFixedCollection, typename TCollectionHeap, SizeT TSize, typename T>
				struct BaseSerlizationHandler<THeap, TFixedCollection<TCollectionHeap, TSize, T>, typename TypeTraits::EnableIfV<IsSerilizableFixedCollection<TFixedCollection<TCollectionHeap, TSize, T>>, void>> {
					using TCollectionClass = TFixedCollection<TCollectionHeap, TSize, T>;

					ForceInline static void SerilizeImpl(const TCollectionClass& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						SizeT size = data.Count;
						BaseSerlizationHandler<THeap, SizeT>::SerilizeImpl(size, rawStream, offset, registry);

						data.ForEach([&](const T& elem) mutable
							{
								BaseSerlizationHandler<THeap, T>::SerilizeImpl(elem, rawStream, offset, registry);
							});
					}

					ForceInline static constexpr Boolean IsMemCopy() {
						return false;
					}

					ForceInline static constexpr SizeT DataSize(const TCollectionClass& data, UnusedData const TypesRegistry& registry) {
						if constexpr (BaseSerlizationHandler<THeap, T>::IsMemCopy())
						{
							return (data.Count * sizeof(T)) + sizeof(SizeT);
						}
						else
						{
							return data.Aggregate([&](SizeT & size, const T & item)
								{
									size += BaseSerlizationHandler<THeap, T>::DataSize(item, registry);
								}, 0ui64) + sizeof(SizeT);
						}
					}

					ForceInline static constexpr auto TypeHint() {
						return
							CTTI::Details::ToCharStr<TSize>();
					}

					ForceInline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::CreateTypeStr(L"0");
					}

					ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<TCollectionClass>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					ForceInline static void DeserelizeImpl(TCollectionClass& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							SizeT Count = 0;
							BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset, registry);

							data = TCollectionClass();

							for (SizeT i = 0; i < Count; i++)
							{
								T tmp = T();
								BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset, registry);
								data.UnsafeSet(MoveRValue(tmp), i);
							}
						}
						else
						{
							Throw(NullPointException(ExceptionPosition));
						}
					}

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return BaseTypeMetaDataRecord<THeap>(
							CTTI::TypeName<TCollectionClass>(),
							CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass,
							sizeof(TCollectionClass),
							0,
							TypeHint(),
							{ BaseSerlizationHandler<THeap, T>::GetTypeMetaDataRecord() }
						);
					}
				};

				template<typename T>
				constexpr bool IsSerilizableAssociableCollection = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::AssociableCollection
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				constexpr bool f = IsSerilizableAssociableCollection<Containers::Dictionary<Int32>>;

				template<typename THeap, template<typename TCollectionHeap, typename TKey, typename TValue> typename TAssociableCollection, typename TCollectionHeap, typename TKey, typename TValue>
				struct BaseSerlizationHandler<THeap, TAssociableCollection<TCollectionHeap, TKey, TValue>, typename TypeTraits::EnableIfV<IsSerilizableAssociableCollection<TAssociableCollection<TCollectionHeap, TKey, TValue>>, void>> {
					using TCollectionClass = TAssociableCollection<TCollectionHeap, TKey, TValue>;
					using T = typename TCollectionClass::DataType;

					ForceInline static void SerilizeImpl(const TCollectionClass& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						BaseSerlizationHandler<THeap, SizeT>::SerilizeImpl(data.Count, rawStream, offset, registry);
						data.ForEach([&](const T& elem) mutable InlineLambda
							{
								BaseSerlizationHandler<THeap, T>::SerilizeImpl(elem, rawStream, offset, registry);
							});
					}

					ForceInline static constexpr Boolean IsMemCopy() {
						return false;
					}

					ForceInline static constexpr SizeT DataSize(const TCollectionClass& data, UnusedData const TypesRegistry& registry) {
						if constexpr (BaseSerlizationHandler<THeap, T>::IsMemCopy())
						{
							return (data.Count * sizeof(T)) + sizeof(SizeT);
						}
						else
						{
							SizeT size = sizeof(T);
							data.ForEach([&](const T& item) InlineLambda {
								size += BaseSerlizationHandler<THeap, T>::DataSize(item, registry);
							});
							return size;
						}
					}

					ForceInline static constexpr auto TypeHint() {
						return
							CTTI::Details::CreateTypeStr(L"");
					}

					ForceInline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::CreateTypeStr(L"0");
					}

					ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<TCollectionClass>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(TCollectionClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					ForceInline static void DeserelizeImpl(TCollectionClass& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							SizeT Count = 0;
							BaseSerlizationHandler<THeap, SizeT>::DeserelizeImpl(Count, rawStream, offset, registry);

							data = TCollectionClass();

							for (SizeT i = 0; i < Count; i++)
							{
								T tmp = T();
								BaseSerlizationHandler<THeap, T>::DeserelizeImpl(tmp, rawStream, offset, registry);
								data.UnsafeAdd(MoveRValue(tmp));
							}
						}
						else
						{
							Throw(NullPointException(ExceptionPosition));
						}
					}

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return BaseTypeMetaDataRecord<THeap>(
							CTTI::TypeName<TCollectionClass>(),
							CTTI::TypeTraits::CTTITypeMetaData<TCollectionClass>::TypeClass,
							sizeof(TCollectionClass),
							0,
							TypeHint(),
							{ BaseSerlizationHandler<THeap, T>::GetTypeMetaDataRecord() }
						);
					}
				};
            }
        }
    }
}