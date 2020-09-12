#pragma once
#include "Seed.Core.CTTI.Base.h"
#include "Seed.Core.Containers.h"
#include "Seed.Core.CTTI.TypeName.Details.h"

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			namespace TypeNameSpecializations {
				static constexpr auto CLASS_TYPE_NAME = Details::CreateTypeStr("class ");
				static constexpr auto STRUCT_TYPE_NAME = Details::CreateTypeStr("struct ");

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
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() + 
							Details::CreateTypeStr("*");
					}
				};

				template<typename T>
				struct TTypeName<const T>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>();;
					}
				};

				template<typename T>
				struct TTypeName<volatile T>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>();;
					}
				};

				template<typename T>
				struct TTypeName<const volatile T>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>();;
					}
				};

				template<typename T>
				struct TTypeName<T&>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() + 
							Details::CreateTypeStr("&");
					}
				};

				template<typename T>
				struct TTypeName<T&&>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() + 
							Details::CreateTypeStr("&&");
					}
				};

				template<typename T, SizeT TSize>
				struct TTypeName<T[TSize]>
				{
					ForceInline constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr("[") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr("]");
					}
				};
#pragma endregion
#pragma region Primitives
#pragma region Numeric
#pragma region Signed Int
				template<>
				struct TTypeName<Seed::Int8> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::Int8");
					}
				};

				template<>
				struct TTypeName<Seed::Int16> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::Int16");
					}
				};

				template<>
				struct TTypeName<Seed::Int32> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::Int32");
					}
				};

				template<>
				struct TTypeName<Seed::Int64> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::Int64");
					}
				};
#pragma endregion
#pragma region Unsigned Int
				template<>
				struct TTypeName<Seed::UInt8> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::UInt8");
					}
				};

				template<>
				struct TTypeName<Seed::UInt16> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::UInt16");
					}
				};

				template<>
				struct TTypeName<Seed::UInt32> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::UInt32");
					}
				};

				template<>
				struct TTypeName<Seed::UInt64> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::UInt64");
					}
				};
#pragma endregion
#pragma region Float and Double
				template<>
				struct TTypeName<Seed::Float> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::Float");
					}
				};

				template<>
				struct TTypeName<Seed::Double> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::Double");
					}
				};
#pragma endregion
#pragma endregion
#pragma region Character
				template<>
				struct TTypeName<Seed::UChar> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::UChar");
					}
				};
#pragma endregion
#pragma region Boolean
				template<>
				struct TTypeName<Seed::Boolean> {
					ForceInline static constexpr auto TypeName() {
						return Details::CreateTypeStr("Seed::Boolean");
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
							CTTI::Details::CreateTypeStr("Seed::Containers::Array<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveArray<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							CTTI::Details::CreateTypeStr("Seed::Containers::SensitiveArray<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyArray<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyArray<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveArray<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitiveArray<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};
#pragma endregion
#pragma region List
				template<typename T>
				struct TTypeName<Seed::Containers::List<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::List<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveList<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveList<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyList<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyList<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveList<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitiveList<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};
#pragma endregion
#pragma region DynamicQueue
				template<typename T>
				struct TTypeName<Seed::Containers::DynamicQueue<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::DynamicQueue<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDynamicQueue<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveDynamicQueue<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDynamicQueue<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyDynamicQueue<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDynamicQueue<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitiveDynamicQueue<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};
#pragma endregion
#pragma region DynamicStack
				template<typename T>
				struct TTypeName<Seed::Containers::DynamicStack<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::DynamicStack<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDynamicStack<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveDynamicStack<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDynamicStack<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyDynamicStack<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDynamicStack<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitiveDynamicStack<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
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
							Details::CreateTypeStr("Seed::Containers::FixedArray<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedArray<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyFixedArray<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedArray<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveFixedArray<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedArray<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitiveFixedArray<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};
#pragma endregion
#pragma region FixedQueue
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::FixedQueue<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::FixedQueue<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedQueue<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyFixedQueue<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedQueue<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveFixedQueue<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedQueue<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitiveFixedQueue<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};
#pragma endregion
#pragma region FixedStack
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::FixedStack<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::FixedStack<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedStack<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyFixedStack<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedStack<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveFixedStack<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedStack<TSize, T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitiveFixedStack<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
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
							Details::CreateTypeStr("Seed::Containers::BitMapT<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveBitMapT<TSize, T>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveBitMapT<") +
							CTTI::Details::ToCharStr<TSize>() +
							Details::CreateTypeStr(", ") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
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
							Details::CreateTypeStr("Seed::Containers::Dictionary<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDictionary<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::ConcurencyContainers::ConcurencyDictionary<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDictionary<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveDictionary<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDictionary<T>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::ConcurencyContainers::ConcurencySensitiveDictionary<") +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};
#pragma endregion
#pragma region Map
				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::Map<TKey, TValue>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::Map<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencyMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyMap<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitiveMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveMap<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencySensitiveMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitiveMap<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
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
							Details::CreateTypeStr("Seed::Containers::PriorityQueue<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencyPriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencyPriorityQueue<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitivePriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::SensitivePriorityQueue<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencySensitivePriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::ConcurencySensitivePriorityQueue<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};
#pragma endregion
#pragma endregion
#pragma region Pair
				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::Pair<TKey, TValue>> {
					ForceInline static constexpr auto TypeName() {
						return
							Details::CreateTypeStr("Seed::Containers::Pair<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitivePair<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::SensitivePair<") +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							Details::CreateTypeStr(", ") +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							Details::CreateTypeStr(">");
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
							Details::CreateTypeStr("Seed::String");
					}
				};

				template<>
				struct TTypeName<Seed::SensitiveString>
				{
					constexpr static auto TypeName()
					{
						return
							Details::CreateTypeStr("Seed::Containers::SensitiveString");
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
                            return TTypeName<Head>::TypeName().Resize<TTypeName<Head>::TypeName().Count - 1>() +
                                Details::CreateTypeStr(", ") +
                                TFieldTypeName<Tail...>::Name();
                        }
                    };

                    template<typename Head>
                    struct TFieldTypeName<Head> {
                        constexpr static auto Name() {
                            return TTypeName<Head>::TypeName().Resize<TTypeName<Head>::TypeName().Count - 1>();
                        }
                    };
					
				public:
					constexpr static auto TypeName()
					{
						if constexpr(sizeof...(Types) ==  0) {
							return
								Details::CreateTypeStr("Seed::Containers::Tuple<") +
								Details::CreateTypeStr(">");
						}else{
							return
								Details::CreateTypeStr("Seed::Containers::Tuple<") +
								TFieldTypeName<Types...>::Name() +
								Details::CreateTypeStr(">");
						}
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
							Details::CreateTypeStr("void");
					}
				};
#pragma endregion
			}
#pragma region Public API
			//<summary>Декороване ім'я типу для серелізації</summary>
			template<typename T>
			constexpr auto TypeName() {
				return TypeNameSpecializations::TTypeName<T>::TypeName() + Details::CreateTypeStr("\0");
			}

			///<summary>Ім'я типу в С++ для компарингу</summary>
			template<typename T>
			constexpr auto CXXTypeName() {
				return CTTI::Details::TypeName<T>() + Details::CreateTypeStr("\0");
			}

			static constexpr SizeT TypeStringSize = Details::TypeStringSize;
#pragma endregion
		}
	}
}