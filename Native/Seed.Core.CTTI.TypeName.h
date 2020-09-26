#pragma once
#include "Seed.Core.CTTI.Base.h"
#include "Seed.Core.Containers.h"
#include "Seed.Core.CTTI.TypeName.Details.h"

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			namespace TypeNameSpecializations {
				static constexpr auto CLASS_TYPE_NAME = Details::CreateTypeStr(L"class ");
				static constexpr auto STRUCT_TYPE_NAME = Details::CreateTypeStr(L"struct ");

				template<typename T>
				struct TTypeName {
					ForceInline static constexpr auto TypeName() {
						if constexpr (CTTI::Details::TypeName<T>().IsStartWith(CLASS_TYPE_NAME))
							return CTTI::Details::TypeName<T>().ResizeLeft<CTTI::Details::TypeName<T>().Count - CLASS_TYPE_NAME.Count>();
						else if constexpr (CTTI::Details::TypeName<T>().IsStartWith(STRUCT_TYPE_NAME))
							return CTTI::Details::TypeName<T>().ResizeLeft<CTTI::Details::TypeName<T>().Count - STRUCT_TYPE_NAME.Count>();
						else
							return CTTI::Details::TypeName<T>();
					}
				};
#pragma region Specializations for array and reference
				template<typename T>
				struct TTypeName<T*>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName() + 
							Details::CreateTypeStr(L"*");
					}
				};

				template<typename T>
				struct TTypeName<const T>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName();
					}
				};

				template<typename T>
				struct TTypeName<volatile T>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName();
					}
				};

				template<typename T>
				struct TTypeName<const volatile T>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName();
					}
				};

				template<typename T>
				struct TTypeName<T&>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName() + 
							Details::CreateTypeStr(L"&");
					}
				};

				template<typename T>
				struct TTypeName<T&&>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName() + 
							Details::CreateTypeStr(L"&&");
					}
				};

				template<typename T, SizeT TSize>
				struct TTypeName<T[TSize]>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L"[") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L"]");
					}
				};
#pragma endregion
#pragma region Primitives
#pragma region Numeric
#pragma region Signed Int
				template<>
				struct TTypeName<Seed::Int8> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::Int8");
					}
				};

				template<>
				struct TTypeName<Seed::Int16> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::Int16");
					}
				};

				template<>
				struct TTypeName<Seed::Int32> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::Int32");
					}
				};

				template<>
				struct TTypeName<Seed::Int64> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::Int64");
					}
				};
#pragma endregion
#pragma region Unsigned Int
				template<>
				struct TTypeName<Seed::UInt8> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::UInt8");
					}
				};

				template<>
				struct TTypeName<Seed::UInt16> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::UInt16");
					}
				};

				template<>
				struct TTypeName<Seed::UInt32> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::UInt32");
					}
				};

				template<>
				struct TTypeName<Seed::UInt64> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::UInt64");
					}
				};
#pragma endregion
#pragma region Float and Double
				template<>
				struct TTypeName<Seed::Float> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::Float");
					}
				};

				template<>
				struct TTypeName<Seed::Double> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::Double");
					}
				};
#pragma endregion
#pragma endregion
#pragma region Character
				template<>
				struct TTypeName<Seed::UChar> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::UChar");
					}
				};
#pragma endregion
#pragma region Boolean
				template<>
				struct TTypeName<Seed::Boolean> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr(L"Seed::Boolean");
					}
				};
#pragma endregion
#pragma endregion
#pragma region Collections
#pragma region Array
				template<typename T>
				struct TTypeName<Seed::Containers::Array<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							CTTI::Details::CreateTypeStr(L"Seed::Containers::Array<") +
							TTypeName<T>::TypeName() +
							CTTI::Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveArray<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							CTTI::Details::CreateTypeStr(L"Seed::Containers::SensitiveArray<") +
							TTypeName<T>::TypeName() +
							CTTI::Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyArray<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyArray<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveArray<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitiveArray<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region List
				template<typename T>
				struct TTypeName<Seed::Containers::List<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::List<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveList<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveList<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyList<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyList<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveList<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitiveList<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region DynamicQueue
				template<typename T>
				struct TTypeName<Seed::Containers::DynamicQueue<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::DynamicQueue<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDynamicQueue<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveDynamicQueue<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDynamicQueue<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyDynamicQueue<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDynamicQueue<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitiveDynamicQueue<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region DynamicStack
				template<typename T>
				struct TTypeName<Seed::Containers::DynamicStack<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::DynamicStack<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDynamicStack<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveDynamicStack<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDynamicStack<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyDynamicStack<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDynamicStack<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitiveDynamicStack<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma endregion
#pragma region Fixed Size Collection
#pragma region FixedAray
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::FixedArray<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::FixedArray<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedArray<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyFixedArray<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedArray<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveFixedArray<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedArray<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitiveFixedArray<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region FixedQueue
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::FixedQueue<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::FixedQueue<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedQueue<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyFixedQueue<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedQueue<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveFixedQueue<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedQueue<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitiveFixedQueue<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region FixedStack
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::FixedStack<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::FixedStack<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedStack<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyFixedStack<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedStack<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveFixedStack<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedStack<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitiveFixedStack<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region BitMap
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::BitMapT<TSize, T>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::BitMapT<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveBitMapT<TSize, T>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveBitMapT<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(L", ") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma endregion
#pragma region Associable Collections
#pragma region Dictionary
				template<typename T>
				struct TTypeName<Seed::Containers::Dictionary<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::Dictionary<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDictionary<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::ConcurencyContainers::ConcurencyDictionary<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDictionary<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveDictionary<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDictionary<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::ConcurencyContainers::ConcurencySensitiveDictionary<") +
							TTypeName<T>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region Map
				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::Map<TKey, TValue>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::Map<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencyMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyMap<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitiveMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveMap<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencySensitiveMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitiveMap<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region PriorityQueue
				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::PriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::PriorityQueue<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencyPriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencyPriorityQueue<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitivePriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitivePriorityQueue<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencySensitivePriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::ConcurencySensitivePriorityQueue<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma endregion
#pragma region Pair
				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::Pair<TKey, TValue>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr(L"Seed::Containers::Pair<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitivePair<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitivePair<") +
							TTypeName<TKey>::TypeName() +
							Details::CreateTypeStr(L", ") +
							TTypeName<TValue>::TypeName() +
							Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region String
				template<>
				struct TTypeName<Seed::String>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::String");
					}
				};

				template<>
				struct TTypeName<Seed::SensitiveString>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"Seed::Containers::SensitiveString");
					}
				};
#pragma endregion
#pragma region Tuple
				template<typename ... Types>
				struct TTypeName<Seed::Containers::Tuple<Types...>>
				{
				private:
                    template<typename Head, typename ... Tail>
                    struct TFieldTypeName {
                        constexpr static auto Name() {
                            return TTypeName<Head>::TypeName() +
                                Details::CreateTypeStr(L", ") +
                                TFieldTypeName<Tail...>::Name();
                        }
                    };

                    template<typename Head>
                    struct TFieldTypeName<Head> {
                        constexpr static auto Name() {
                            return TTypeName<Head>::TypeName();
                        }
                    };
					
				public:
					constexpr static auto TypeName()
					{
						if constexpr(sizeof...(Types) ==  0) {
							return
								Details::CreateTypeStr(L"Seed::Containers::Tuple<") +
								Details::CreateTypeStr(L">");
						}else{
							return
								Details::CreateTypeStr(L"Seed::Containers::Tuple<") +
								TFieldTypeName<Types...>::Name() +
								Details::CreateTypeStr(L">");
						}
					}
				};
#pragma endregion
#pragma region GenericTemplate implementation
				template<template <typename ...> typename TTemplate, typename ... TTypes>
				struct TTypeName<TTemplate<TTypes...>>
				{
					using T = TTemplate<TTypes...>;

					constexpr static auto FullTypeName() {
						if constexpr (CTTI::Details::TypeName<T>().IsStartWith(CLASS_TYPE_NAME))
							return CTTI::Details::TypeName<T>().ResizeLeft<CTTI::Details::TypeName<T>().Count - CLASS_TYPE_NAME.Count>();
						else if constexpr (CTTI::Details::TypeName<T>().IsStartWith(STRUCT_TYPE_NAME))
							return CTTI::Details::TypeName<T>().ResizeLeft<CTTI::Details::TypeName<T>().Count - STRUCT_TYPE_NAME.Count>();
						else
							return CTTI::Details::TypeName<T>();
					}

					template<typename THead, typename ... TTypes>
					struct TSubTypeName {
						constexpr static auto Name() {
							return TTypeName<THead>::TypeName() + Details::CreateTypeStr(L", ") + TSubTypeName<TTypes...>::Name();
						}
					};

					template<typename THead>
					struct TSubTypeName<THead> {
						constexpr static auto Name() {
							return TTypeName<THead>::TypeName();
						}
					};

					constexpr static auto TypeName()
					{
						if constexpr (sizeof...(TTypes) == 0)
							return FullTypeName();
						else
							return
								FullTypeName().Resize<FullTypeName().Find('<') + 1>() + TSubTypeName<TTypes...>::Name() + Details::CreateTypeStr(L">");
					}
				};
#pragma endregion
#pragma region Void
				template<>
				struct TTypeName<void>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr(L"void");
					}
				};
#pragma endregion
			}
#pragma region Public API
			//<summary>Декороване ім'я типу для серелізації</summary>
			template<typename T>
			constexpr auto TypeName() {
				return TypeNameSpecializations::TTypeName<T>::TypeName() + Details::CreateTypeStr(L"\0");
			}

			///<summary>Ім'я типу в С++ для компарингу</summary>
			template<typename T>
			constexpr auto CXXTypeName() {
				return CTTI::Details::TypeName<T>() + Details::CreateTypeStr(L"\0");
			}

			static constexpr SizeT TypeStringSize = Details::TypeStringSize;
#pragma endregion
		}
	}
}