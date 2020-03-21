#pragma once
#include "Seed.Core.CTTI.Base.h"
#include "Seed.Core.Containers.h"
#include "Seed.Core.CTTI.TypeName.Details.h"

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			namespace TypeNameSpecializations {
				static constexpr auto CLASS_TYPE_NAME = Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("class ")());
				static constexpr auto CONST_MODIFICATOR_NAME = Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("const ")());
				static constexpr auto VOLATILE_MODIFICATOR_NAME = Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("volatile ")());

				template<typename T>
				struct TTypeName {
					constexpr static auto TypeName() {
						if constexpr (CTTI::Details::TypeName<T>().IsStartWith(CLASS_TYPE_NAME))
							return CTTI::Details::TypeName<T>().ResizeLeft<CTTI::Details::TypeName<T>().Count - CLASS_TYPE_NAME.Count>();
						else if constexpr (Core::TypeTraits::IsConstV<T>&& Core::TypeTraits::IsVolatileV<T>)
						{
							using U = Core::TypeTraits::RemoveCV<T>;
							if constexpr (CTTI::Details::TypeName<U>().IsStartWith(CLASS_TYPE_NAME))
							{
								return CONST_MODIFICATOR_NAME + VOLATILE_MODIFICATOR_NAME +
									CTTI::Details::TypeName<T>().ResizeLeft<CTTI::Details::TypeName<T>().Count - CLASS_TYPE_NAME.Count>();
							}

							return CTTI::Details::TypeName<T>();
						}
						else if constexpr (Core::TypeTraits::IsConstV<T>)
						{
							using U = Core::TypeTraits::RemoveCV<T>;
							if constexpr (CTTI::Details::TypeName<U>().IsStartWith(CLASS_TYPE_NAME))
							{
								return CONST_MODIFICATOR_NAME +
									CTTI::Details::TypeName<T>().ResizeLeft<CTTI::Details::TypeName<T>().Count - CLASS_TYPE_NAME.Count>();
							}

							return CTTI::Details::TypeName<T>();
						}
						else if constexpr (Core::TypeTraits::IsVolatileV<T>)
						{
							using U = Core::TypeTraits::RemoveCV<T>;
							if constexpr (CTTI::Details::TypeName<U>().IsStartWith(CLASS_TYPE_NAME))
							{
								return VOLATILE_MODIFICATOR_NAME +
									CTTI::Details::TypeName<T>().ResizeLeft<CTTI::Details::TypeName<T>().Count - CLASS_TYPE_NAME.Count>();
							}

							return CTTI::Details::TypeName<T>();
						}
						else
						{
							return CTTI::Details::TypeName<T>();
						}
					}
				};
#pragma region Specializations for array and reference
				template<typename T>
				struct TTypeName<T*>
				{
					constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() + 
							Details::TypeStringToArray(GenNGTypeString("*")());
					}
				};

				template<typename T>
				struct TTypeName<T&>
				{
					constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() + 
							Details::TypeStringToArray(GenNGTypeString("&")());
					}
				};

				template<typename T>
				struct TTypeName<T&&>
				{
					constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() + 
							Details::TypeStringToArray(GenNGTypeString("&&")());
					}
				};

				template<typename T, SizeT TSize>
				struct TTypeName<T[TSize]>
				{
					constexpr static auto TypeName()
					{
						return TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("[")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString("]")());
					}
				};
#pragma endregion
#pragma region Primitives
#pragma region Numeric
#pragma region Signed Int
				template<>
				struct TTypeName<Seed::Int8> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::Int8")());
					}
				};

				template<>
				struct TTypeName<Seed::Int16> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::Int16")());
					}
				};

				template<>
				struct TTypeName<Seed::Int32> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::Int16")());
					}
				};

				template<>
				struct TTypeName<Seed::Int64> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::Int16")());
					}
				};
#pragma endregion
#pragma region Unsigned Int
				template<>
				struct TTypeName<Seed::UInt8> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::UInt8")());
					}
				};

				template<>
				struct TTypeName<Seed::UInt16> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::UInt16")());
					}
				};

				template<>
				struct TTypeName<Seed::UInt32> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::UInt16")());
					}
				};

				template<>
				struct TTypeName<Seed::UInt64> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::UInt64")());
					}
				};
#pragma endregion
#pragma region Float and Double
				template<>
				struct TTypeName<Seed::Float> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::Float")());
					}
				};

				template<>
				struct TTypeName<Seed::Double> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::Double")());
					}
				};
#pragma endregion
#pragma endregion
#pragma region Character
				template<>
				struct TTypeName<Seed::UChar> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::UChar")());
					}
				};
#pragma endregion
#pragma region Boolean
				template<>
				struct TTypeName<Seed::Boolean> {
					constexpr static auto TypeName() {
						return Details::TypeStringToArray(GenNGTypeString("Seed::Boolean")());
					}
				};
#pragma endregion
#pragma endregion
#pragma region Collections
#pragma region Array
				template<typename T>
				struct TTypeName<Seed::Containers::Array<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::Array<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveArray<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveArray<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyArray<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyArray<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveArray<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitiveArray<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma region List
				template<typename T>
				struct TTypeName<Seed::Containers::List<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::List<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveList<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveList<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyList<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyList<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveList<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitiveList<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma region DynamicQueue
				template<typename T>
				struct TTypeName<Seed::Containers::DynamicQueue<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::DynamicQueue<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDynamicQueue<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveDynamicQueue<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDynamicQueue<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyDynamicQueue<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDynamicQueue<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitiveDynamicQueue<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma region DynamicStack
				template<typename T>
				struct TTypeName<Seed::Containers::DynamicStack<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::DynamicStack<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDynamicStack<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveDynamicStack<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDynamicStack<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyDynamicStack<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDynamicStack<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitiveDynamicStack<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma endregion
#pragma region Fixed Size Collection
#pragma region FixedAray
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::FixedArray<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::FixedArray<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedArray<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyFixedArray<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedArray<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveFixedArray<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedArray<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitiveFixedArray<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma region FixedQueue
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::FixedQueue<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::FixedQueue<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedQueue<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyFixedQueue<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedQueue<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveFixedQueue<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedQueue<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitiveFixedQueue<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma region FixedStack
				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::FixedStack<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::FixedStack<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencyFixedStack<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyFixedStack<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveFixedStack<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveFixedStack<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveFixedStack<TSize, T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitiveFixedStack<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
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
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::BitMapT<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<SizeT TSize, typename T>
				struct TTypeName<Seed::Containers::SensitiveBitMapT<TSize, T>>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveBitMapT<")()) +
							CTTI::Details::ToCharStr<TSize>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma endregion
#pragma region Associable Collections
#pragma region Dictionary
				template<typename T>
				struct TTypeName<Seed::Containers::Dictionary<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::Dictionary<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencyDictionary<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::ConcurencyContainers::ConcurencyDictionary<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::SensitiveDictionary<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveDictionary<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename T>
				struct TTypeName<Seed::Containers::ConcurencySensitiveDictionary<T>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::ConcurencyContainers::ConcurencySensitiveDictionary<")()) +
							TTypeName<T>::TypeName().Resize<TTypeName<T>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma region Map
				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::Map<TKey, TValue>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::Map<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencyMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyMap<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitiveMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitiveMap<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencySensitiveMap<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitiveMap<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
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
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::PriorityQueue<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencyPriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencyPriorityQueue<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitivePriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitivePriorityQueue<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::ConcurencySensitivePriorityQueue<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::ConcurencySensitivePriorityQueue<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};
#pragma endregion
#pragma endregion
#pragma region Pair
				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::Pair<TKey, TValue>> {
					constexpr static auto TypeName() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::Pair<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
					}
				};

				template<typename TKey, typename TValue>
				struct TTypeName<Seed::Containers::SensitivePair<TKey, TValue>>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::SensitivePair<")()) +
							TTypeName<TKey>::TypeName().Resize<TTypeName<TKey>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
							TTypeName<TValue>::TypeName().Resize<TTypeName<TValue>::TypeName().Count - 1>() +
							CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
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
							CTTI::Details::TypeStringToArray(GenNGTypeString("Seed::String")());
					}
				};

				template<>
				struct TTypeName<Seed::SensitiveString>
				{
					constexpr static auto TypeName()
					{
						return
							CTTI::Details::TypeStringToArray(GenNGTypeString("Seed::Containers::SensitiveString")());
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
                                CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString(", ")()) +
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
								CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::Tuple<")()) +
								CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
						}else{
							return
								CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Seed::Containers::Tuple<")()) +
								TFieldTypeName<Types...>::Name() +
								CTTI::Details::TypeStringToArray(GenNGTypeString(">")());
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
							CTTI::Details::TypeStringToArray(GenNGTypeString("void")());
					}
				};
#pragma endregion
			}
#pragma region Public API
			//<summary>Декороване ім'я типу для серелізації</summary>
			template<typename T>
			constexpr auto TypeName() {
				return TypeNameSpecializations::TTypeName<T>::TypeName();
			}

			///<summary>Ім'я типу в С++ для компарингу</summary>
			template<typename T>
			constexpr auto CXXTypeName() {
				return CTTI::Details::TypeName<T>();
			}
#pragma endregion
		}
	}
}