#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.Tuple.h"
#include "Seed.Core.CTTI.TypeNames.h"

namespace Seed {
	inline namespace Core {
		namespace TypeTraits {
			namespace Types {
				//TODO: Refactore it
#pragma region Data and type classification
				///<summary>“ипи даних</summary>
				enum class EDataClasses : UInt8 {
					Numeric,
					Logic,
					Character,
					Composite,
					Binnary,
					Dynamic,
					Uknown = static_cast<UInt8>(-1)
				};

				///<summary>“ипи Native тип≥в</summary>
				enum class ENativeTypes : UInt8
				{
					Primitive,
					Pod,
					Collection,
					StaticStruct,
					Dynamic,
					Uknown = static_cast<UInt8>(-1)
				};
#pragma endregion
#pragma region Primitives
				template<typename T, typename Enable = bool>
				struct MetaData
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Uknown;
					static constexpr ENativeTypes NativeType = ENativeTypes::Uknown;

					static SizeT GetDynamicTypesCount() { return 0; }
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::IsNumeric<T>, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Numeric;
					static constexpr ENativeTypes NativeType = ENativeTypes::Primitive;

					static SizeT GetDynamicTypesCount() { return 0; }
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::IsBoolean<T>, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Logic;
					static constexpr ENativeTypes NativeType = ENativeTypes::Primitive;

					static SizeT GetDynamicTypesCount() { return 0; }
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::IsCharacter<T>, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Character;
					static constexpr ENativeTypes NativeType = ENativeTypes::Primitive;

					static SizeT GetDynamicTypesCount() { return 0; }
				};
#pragma endregion
#pragma region Collection
				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::Collection::IsArray<T>, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Composite;
					static constexpr ENativeTypes NativeType = ENativeTypes::Collection;
					using ItemsType = typename T::DataType;

					static SizeT GetDynamicTypesCount() { return 0 + MetaData<ItemsType>::GetDynamicTypesCount(); }
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::Collection::IsList<T>, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Composite;
					static constexpr ENativeTypes NativeType = ENativeTypes::Collection;
					using ItemsType = typename T::DataType;

					static SizeT GetDynamicTypesCount() { return 0 + MetaData<ItemsType>::GetDynamicTypesCount(); }
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::Collection::IsString<T>, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Character;
					static constexpr ENativeTypes NativeType = ENativeTypes::Collection;
					using ItemsType = typename T::DataType;

					static SizeT GetDynamicTypesCount() { return 0 + MetaData<ItemsType>::GetDynamicTypesCount(); }
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::Collection::IsPair<T>, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Binnary;
					static constexpr ENativeTypes NativeType = ENativeTypes::StaticStruct;
					using Key = typename T::KeyType;
					using Value = typename T::ValueType;

					static SizeT GetDynamicTypesCount() { return 0 + MetaData<Key>::GetDynamicTypesCount() + MetaData<Value>::GetDynamicTypesCount(); }
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::Collection::IsMap<T>, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Composite;
					static constexpr ENativeTypes NativeType = ENativeTypes::Collection;
					using ItemsType = typename T::DataType;

					static SizeT GetDynamicTypesCount() { return 0 + MetaData<ItemsType>::GetDynamicTypesCount(); }
				};
#pragma endregion
#pragma region User define struct
				template<typename T, typename TypeStringT, SizeT Offset>
				struct Field {
					using FieldType = T;
					static constexpr auto FieldName = CTTI::TypeStringToArray(TypeStringT()).Resize<TypeStringT::Size() - 1>();
					static constexpr SizeT FieldSize = sizeof(T);
					static constexpr SizeT FieldOffset = Offset;

					static SizeT GetDynamicTypesCount() { return 0 + MetaData<FieldType>::GetDynamicTypesCount(); }
				};

				template<typename T, bool AllowSerilize, bool AllowDeserilize, bool Internal, typename ... TFields>
				struct CustomStruct : TypeTraits::LogicTrue
				{
					using Type = T;
					static constexpr auto IsAllowSerilize = AllowSerilize;
					static constexpr auto IsAllowDeserlize = AllowDeserilize;
					static constexpr auto IsInternal = Internal;

					static constexpr SizeT GetFieldsSize = [&]() { SizeT result = 0; for (auto s : { TFields::FieldSize... }) result += s; return result; }();
					static SizeT GetDynamicTypesCount() { return TFields::GetDynamicTypesCount() + ...; }

					using Fields = typename Containers::Tuple<TFields...>;
				};

				template<typename T>
				struct IUserStructMetaData {
					using TypeMetaData = typename T::InternalMetaData;
				};

				struct ISerilizable {
					virtual Byte* GetDataPtr() const = 0;
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<TypeTraits::IsBasedOn<ISerilizable, T>::State, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = sizeof(T);
					static constexpr EDataClasses DataType = EDataClasses::Composite;
					static constexpr ENativeTypes NativeType = ENativeTypes::StaticStruct;
					using ItemsType = typename IUserStructMetaData<T>::TypeMetaData::Fields;

					using AdditionalData = typename IUserStructMetaData<T>::TypeMetaData;

					static SizeT GetDynamicTypesCount() { return AdditionalData::GetDynamicTypesCount(); }
				};
#pragma endregion
#pragma region Dynamic Type
				struct IDynamicMetaData {
					virtual String GetTypeMetaData() = 0;
					virtual SizeT GetTypeSize() = 0;
					virtual String GetTypeName() = 0;
				};

				template<typename T>
				struct MetaData<T, typename TypeTraits::EnableIfV<IsBasedOn<IDynamicMetaData, T>::State, bool>>
				{
					using Type = T;
					static constexpr SizeT Size = 0;
					static constexpr EDataClasses DataType = EDataClasses::Dynamic;
					static constexpr ENativeTypes NativeType = ENativeTypes::Dynamic;
					static SizeT GetDynamicTypesCount() { return 1; }
				};
#pragma endregion
			}
		}
	}
}