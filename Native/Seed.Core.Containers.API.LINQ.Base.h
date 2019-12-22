#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.ConstString.h"

namespace Seed {
	inline namespace Core {
        namespace NewSerilization
        {
            namespace Details
            {
                /// <summary>
                /// Ядро серелізатора
                /// </summary>
                /// <typeparam name="THeap">Куча в які серелізатор працюватиме</typeparam>
                /// <typeparam name="T">Тип з яким серелізатор працюватиме</typeparam>
                template<typename THeap, typename T, typename TEnable = void>
                struct BaseSerlizationHandler;
            }
        }

        /// <summary>
        /// Простір імен контейнерів(колекцій)
        /// </summary>
		namespace Containers {
            /// <summary>
            /// Простір імен апі LINQ
            /// </summary>
			namespace LINQ {
                /// <summary>
                /// Надає CTTI інформацію для колекцій
                /// </summary>
                /// <typeparam name="TCollection">Тип колекції</typeparam>
                /// <typeparam name="THeap">Куча колекції</typeparam>
				template<
					typename TCollection,
					typename THeap,
					typename T,
					Boolean TIsDynamicT,
					Boolean TIsResizeableT,
					Boolean TIsConcurencySupportT
				>
                // ReSharper disable CppInconsistentNaming
                struct TLinq {
                    // ReSharper restore CppInconsistentNaming

                    /// <summary>
                    /// Тип колекції
                    /// </summary>
                    using CollectionType = TCollection;
                    /// <summary>
                    /// Тип кучі
                    /// </summary>
					using HeapType = THeap;
                    /// <summary>
                    /// Тип елемента
                    /// </summary>
					using DataType = T;
                    /// <summary>
                    /// True якщо колекція динамічна
                    /// </summary>
					static constexpr Boolean IsDynamic = TIsDynamicT;
                    /// <summary>
                    /// True якщо колекція дозволяє змінну кількості елементів
                    /// </summary>
					static constexpr Boolean IsResizeable = TIsResizeableT;
				};

                /// <summary>
                /// Базовий клас для LINQ контейнерів
                /// </summary>
				class ILinqBase { public: virtual ~ILinqBase() = 0 {}; };
                /// <summary>
                /// Базовий клас для загальних колекцій
                /// </summary>
				class ICollectionGenericBase { public: virtual ~ICollectionGenericBase() = 0 {}; };
                /// <summary>
                /// Базовий клас для фіксованих колекцій
                /// </summary>
				class IFixedCollectionGenericBase { public: virtual ~IFixedCollectionGenericBase() = 0 {}; };
                /// <summary>
                /// Базовий клас для асоціативних колекцій
                /// </summary>
				class IAssociableCollectionGenericBase { public: virtual ~IAssociableCollectionGenericBase() = 0 {}; };
#pragma region LINQ for one typearg Collection

                /// <summary>
                /// Загальний клас загальних колекцій
                /// </summary>
                /// <typeparam name="TCollection">Тип колекція</typeparam>
                /// <typeparam name="THeap">Куча колекції</typeparam>
                /// <typeparam name="T">Тип елементів</typeparam>
                /// <typeparam name="TIsDynamic">Вказує чи колекція динамічно змінює розмір</typeparam>
                /// <typeparam name="TIsConcurencySuport">Вказує чи колекція підтримує багатопотоковість</typeparam>
				template<
                    template<typename THeap, typename T> typename TCollection,
			        typename THeap,
			        typename T,
			        Boolean TIsDynamic,
			        Boolean TIsConcurencySuport
			    >
				class ICollectionBase :
					public ILinqBase,
					public ICollectionGenericBase,
					public TLinq<TCollection<THeap, T>, THeap, T, TIsDynamic, true, TIsConcurencySuport>
				{
				public:
                    /// <summary>
                    /// Копіює елементи до фіксованої колекції
                    /// </summary>
                    /// <typeparam name="TNewCollection">Фіксована колекція в яку скопіюються елементи</typeparam>
                    /// <param name="ignoreSizeMismatch">Вказує чи ігнорувати різницю кількості елементів</param>
                    /// <returns>Нову фіксовану колекцію яка містить копію елементів</returns>
					template<typename TNewFixedCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<IFixedCollectionGenericBase, TNewFixedCollection> &&
						TypeTraits::IsSameV<typename TNewFixedCollection::DataType, typename TLinq<TCollection<THeap, T>, THeap, T, TIsDynamic, true, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						TNewFixedCollection CopyToFixed(const Boolean ignoreSizeMismatch = false) {
						auto thisCollectionRef = static_cast<const TCollection<THeap, T>&>(*this);
						if (ignoreSizeMismatch) {
							if (TNewFixedCollection::FixedSize == thisCollectionRef.Count || TNewFixedCollection::FixedSize > thisCollectionRef.Count) {
								TNewFixedCollection instance;
								thisCollectionRef.ForEach([&](const T& item, SizeT index) {
                                        if constexpr(TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
								        {
                                            instance.UnsafeSet(item.Clone(), index);
                                        }
								        else
                                        {
                                            instance.UnsafeSet(T(item), index);
                                        }
									});
								return instance;
							}
							else {
								TNewFixedCollection instance;
								thisCollectionRef.ForEach([&](const T& item, SizeT index) {
                                    if constexpr (TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
                                    {
                                        instance.UnsafeSet(item.Clone(), index);
                                    }
                                    else
                                    {
                                        instance.UnsafeSet(T(item), index);
                                    }
									}, TNewFixedCollection::FixedSize);
								return instance;
							}
						}
						else {
                            if (TNewFixedCollection::FixedSize != thisCollectionRef.Count)
                                Internals::Exceptions::ThrowIndexOutOfRange(__FILE__,
                                                                            __FUNCSIG__,
                                                                            __LINE__,
                                                                            thisCollectionRef.Count,
                                                                            TNewFixedCollection::FixedSize);
							else {
								TNewFixedCollection instance;
								thisCollectionRef.ForEach([&](const T& item, SizeT index) {
									instance.UnsafeSet(item, index);
									});
								return instance;
							}
						}
					}

                    /// <summary>
                    /// Копіює елементи до фіксованої колекції
                    /// </summary>
                    /// <typeparam name="TNewCollection">Фіксована колекція в яку скопіюються елементи</typeparam>
                    /// <param name="ignoreSizeMismatch">Вказує чи ігнорувати різницю кількості елементів</param>
                    /// <returns>Нову фіксовану колекцію яка містить копію елементів</returns>
					template<typename TNewFixedCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<IFixedCollectionGenericBase, TNewFixedCollection> &&
						TypeTraits::IsSameV<typename TNewFixedCollection::DataType, typename TLinq<TCollection<THeap, T>, THeap, T, TIsDynamic, true, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						TNewFixedCollection CopyToFixed(Boolean ignoreSizeMismatch = false) const {
						auto thisCollectionRef = static_cast<const TCollection<THeap, T>&>(*this);
						if (ignoreSizeMismatch) {
							if (TNewFixedCollection::FixedSize == thisCollectionRef.Count || TNewFixedCollection::FixedSize > thisCollectionRef.Count) {
								TNewFixedCollection instance;
								thisCollectionRef.ForEach([&](const T& item, SizeT index) {
                                    if constexpr (TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
                                    {
                                        instance.UnsafeSet(item.Clone(), index);
                                    }
                                    else
                                    {
                                        instance.UnsafeSet(T(item), index);
                                    }
									});
								return instance;
							}
							else {
								TNewFixedCollection instance;
								thisCollectionRef.ForEach([&](const T& item, SizeT index) {
                                    if constexpr (TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
                                    {
                                        instance.UnsafeSet(item.Clone(), index);
                                    }
                                    else
                                    {
                                        instance.UnsafeSet(T(item), index);
                                    }
									}, TNewFixedCollection::FixedSize);
								return instance;
							}
						}
						else {
							if (TNewFixedCollection::FixedSize != thisCollectionRef.Count)
                                Internals::Exceptions::ThrowIndexOutOfRange(__FILE__,
                                                                            __FUNCSIG__,
                                                                            __LINE__,
                                                                            thisCollectionRef.Count,
                                                                            TNewFixedCollection::FixedSize);
							else {
								TNewFixedCollection instance;
								thisCollectionRef.ForEach([&](const T& item, SizeT index) {
                                    if constexpr (TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
                                    {
                                        instance.UnsafeSet(item.Clone(), index);
                                    }
                                    else
                                    {
                                        instance.UnsafeSet(T(item), index);
                                    }
									});
								return instance;
							}
						}
					}
				public:
					virtual ~ICollectionBase() = 0 {}
				};
#pragma endregion
#pragma region LINQ for fixed size Collection
                /// <summary>
                /// Базовий клас CTTI метаданих для фіксованих колекцій
                /// </summary>
                /// <typeparam name="TCollection">Тип колекції</typeparam>
                /// <typeparam name="THeap">Куча колекції</typeparam>
                /// <typeparam name="T">Тип елементів</typeparam>
                /// <typeparam name="TSize">Кількість елементів</typeparam>
                /// <typeparam name="TIsConcurencySuport">Чи підтримує багатопотоковість</typeparam>
				template<typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
                // ReSharper disable once CppInconsistentNaming
                struct TFixedLinq : public TLinq<TCollection, THeap, T, false, false, TIsConcurencySuport> {
                    /// <summary>
                    /// Кількість елементів
                    /// </summary>
					static constexpr SizeT FixedSize = TSize;
				};

                /// <summary>
                /// Базовий клас для фіксованих колекцій
                /// </summary>
                /// <typeparam name="TCollection">Тип фіксованої колекції</typeparam>
                /// <typeparam name="THeap">Тип кучі</typeparam>
                /// <typeparam name="T">Тип елементів</typeparam>
                /// <typeparam name="TSize">Кількість елементів</typeparam>
                /// <typeparam name="TIsConcurencySuport">Чи підтримує багатопотоковість</typeparam>
				template<
                    template<typename, SizeT, typename> typename TCollection,
			        typename THeap,
			        typename T,
			        SizeT TSize,
			        Boolean TIsConcurencySuport
			    >
				class IFixedCollectionBase :
					public ILinqBase,
					public IFixedCollectionGenericBase,
					public TFixedLinq<TCollection<THeap, TSize, T>, THeap, T, TSize, TIsConcurencySuport>
				{
				public:
                    /// <summary>
                    /// Копіює елементи до нової загальної колекції
                    /// </summary>
                    /// <typeparam name="TNewCollection">Нова загальна колекція</typeparam>
                    /// <returns>Нову колекцію з копією елементів</returns>
					template<typename TNewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, TNewCollection> &&
						TypeTraits::IsSameV<typename TNewCollection::DataType, typename TFixedLinq<TCollection<THeap, TSize, T>, THeap, T, TSize, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						TNewCollection CopyTo() {
						if constexpr (!TNewCollection::IsDynamic) {
							auto thisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							TNewCollection instance(thisCollectionRef.Count);
                            thisCollectionRef.ForEach([&] (const T& value, SizeT index)
                            {
                                if constexpr (TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
                                {
                                    instance.UnsafeSet(value.Clone(), index);
                                }
                                else
                                {
                                    instance.UnsafeSet(T(value), index);
                                }
                            }
                                );
							return instance;
						}
						else {
							auto thisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							TNewCollection instance;
                            thisCollectionRef.ForEach([&] (const T& value)
                            {
                                if constexpr (TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
                                {
                                    instance.UnsafeAdd(value.Clone());
                                }
                                else
                                {
                                    instance.UnsafeAdd(T(value));
                                }
                            });
							return instance;
						}
					}

                    /// <summary>
                    /// Копіює елементи до нової загальної колекції
                    /// </summary>
                    /// <typeparam name="TNewCollection">Нова загальна колекція</typeparam>
                    /// <returns>Нову колекцію з копією елементів</returns>
					template<typename TNewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, TNewCollection> &&
						TypeTraits::IsSameV<typename TNewCollection::DataType, typename TFixedLinq<TCollection<THeap, TSize, T>, THeap, T, TSize, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
                    TNewCollection CopyTo() const
                    {
						if constexpr (!TNewCollection::IsDynamic) {
							auto thisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							TNewCollection instance(thisCollectionRef.Count);
							thisCollectionRef.ForEach([&](const T& value, SizeT index) {
                                if constexpr (TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
                                {
                                    instance.UnsafeSet(value.Clone(), index);
                                }
                                else
                                {
                                    instance.UnsafeSet(T(value), index);
                                }
								});
							return instance;
						}
						else {
							auto thisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							TNewCollection instance;
							thisCollectionRef.ForEach([&](const T& value) {
                                if constexpr (TypeTraits::IsMemberFunctionPointerV<decltype(&T::Clone)>)
                                {
                                    instance.UnsafeAdd(value.Clone());
                                }
                                else
                                {
                                    instance.UnsafeAdd(T(value));
                                }
								});
							return instance;
						}
					}

                    /// <summary>
                    /// Переміщає елементи в нову загальну колекцію, в старі колекції елементи стають не валідними
                    /// </summary>
                    /// <typeparam name="TNewCollection">Тип нової загальної колекції колекції</typeparam>
                    /// <returns>Нова загальна колекція з елементами з поточної</returns>
					template<typename TNewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, TNewCollection> &&
						TypeTraits::IsSameV<typename TNewCollection::DataType, typename TFixedLinq<TCollection<THeap, TSize, T>, THeap, T, TSize, TIsConcurencySuport>::DataType>
						>,
						bool
					> = true>
						TNewCollection MoveTo() {
						if constexpr (!TNewCollection::IsDynamic) {
							auto thisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							TNewCollection instance(thisCollectionRef.Count);
							thisCollectionRef.RemoceAll([&](T&& value, SizeT index) {
								instance.UnsafeSet(MoveRValue(value), index);
								});
							return instance;
						}
						else {
							auto thisCollectionRef = static_cast<const TCollection<THeap, TSize, T>&>(*this);
							TNewCollection instance;
							thisCollectionRef.RemoceAll([&](T&& value) {
								instance.UnsafeAdd(MoveRValue(value));
								});
							return instance;
						}
					}
				public:
					virtual ~IFixedCollectionBase() = 0 {}
				};
#pragma endregion
#pragma region LINQ for associable Collection
                /// <summary>
                /// Базовий клас CTTI метаданих для асоціативних колекцій
                /// </summary>
                /// <typeparam name="TCollection">Тип колекції</typeparam>
                /// <typeparam name="THeap">Тип кучі</typeparam>
                /// <typeparam name="TKey">Тип ключа</typeparam>
                /// <typeparam name="TValue">Тип значення</typeparam>
                /// <typeparam name="TPair">Тип пари ключ значення</typeparam>
                /// <typeparam name="TIsConcurencySuport">Чи підтримує багатопотоковість</typeparam>
				template<typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, Boolean TIsConcurencySuport>
                // ReSharper disable once CppInconsistentNaming
                struct TAssociableLinq : public TLinq<TCollection, THeap, TPair, true, true, TIsConcurencySuport> {
                    // ReSharper restore CppInconsistentNaming
                    
                    /// <summary>
                    /// Тип ключа
                    /// </summary>
					using KeyType = TKey;
                    /// <summary>
                    /// Тип значення
                    /// </summary>
					using ValueType = TValue;
				};

                /// <summary>
                /// Базовий тип асоціативних колекцій
                /// </summary>
                /// <typeparam name="TCollection">Тип асоціативної колекції</typeparam>
                /// <typeparam name="THeap">Тип кучі</typeparam>
                /// <typeparam name="TKey">Тип ключа</typeparam>
                /// <typeparam name="TValue">Тип значення</typeparam>
                /// <typeparam name="TPair">Тип пари ключ значення</typeparam>
                /// <typeparam name="TIsConcurencySuport">Чи підтримує багатопотоковість</typeparam>
				template<
                    template<typename THeap, typename TKey, typename TValue> typename TCollection,
			        typename THeap,
			        typename TKey,
			        typename TValue,
			        typename TPair,
			        bool TIsConcurencySuportT
			    >
				class IAssociableCollectionBase :
					public ILinqBase,
					public IAssociableCollectionGenericBase,
					public TAssociableLinq<TCollection<THeap, TKey, TValue>, THeap, TKey, TValue, TPair, true>
				{
				public:
					virtual ~IAssociableCollectionBase() = 0 {}
				};
#pragma endregion
#pragma region LINQ for Pair
                /// <summary>
                /// Базовий клас CTTI метаданих для пар ключ значення
                /// </summary>
                /// <typeparam name="TPair">Тип пари</typeparam>
                /// <typeparam name="THeap">Тип кучі</typeparam>
                /// <typeparam name="TKey">Тип ключа</typeparam>
                /// <typeparam name="TValue">Тип значення</typeparam>
                /// <typeparam name="TIsConcurencySuport">Чи підтримує багатопотоковість</typeparam>
				template<typename TPair, typename THeap, typename TKey, typename TValue, Boolean TIsConcurencySuport>
                // ReSharper disable CppInconsistentNaming
                struct TPairLinq : public TLinq<TPair, THeap, TPair, false, false, TIsConcurencySuport> {
                    // ReSharper restore CppInconsistentNaming

                    /// <summary>
                    /// Тип ключа
                    /// </summary>
                    using KeyType = TKey;
                    /// <summary>
                    /// Тип значення
                    /// </summary>
					using ValueType = TValue;
				};
#pragma endregion
#pragma region LINQ for String
                /// <summary>
                /// Базовий клас CTTI метаданих для рядкових змінних
                /// </summary>
                /// <typeparam name="TString">Тип рядка</typeparam>
                /// <typeparam name="THeap">Тип кучі</typeparam>
                /// <typeparam name="TChar">Тип символа</typeparam>
                /// <typeparam name="TIsConcurencySuport">Чи підтримує багатопотоковість</typeparam>
				template<typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
                // ReSharper disable CppInconsistentNaming
                struct TStringLinq : public TLinq<TString, THeap, TChar, false, true, TIsConcurencySuport> {
                    // ReSharper restore CppInconsistentNaming

                    /// <summary>
                    /// Тип символа
                    /// </summary>
                    using CharacterType = TChar;
				};

                /// <summary>
                /// Загальний тип рядкової змінної
                /// </summary>
                /// <typeparam name="TString">Тип рядкової змінної</typeparam>
                /// <typeparam name="THeap">Тип кучі</typeparam>
                /// <typeparam name="TChar">Тип символа</typeparam>
				template<template<typename THeap> typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
				class IStringCollectionBaseLinq : public TStringLinq<TString<THeap>, THeap, TChar, TIsConcurencySuport> {
				public:
					virtual ~IStringCollectionBaseLinq() = 0 {};
				};
#pragma endregion
			}
		}
	}
}