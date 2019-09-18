#pragma once
#include "Seed.Core.CTTI.h"
#include "Seed.Core.Stage1.h"

namespace Seed {
	inline namespace Core {
		namespace Serlization {
//#define offsetof(s,m) ((::size_t)&reinterpret_cast<char const volatile&>((((s*)0)->m)))
//
//			enum class ETypeClass {
//				Numeric,
//				Character,
//				String,
//				Container,
//				Pair,
//				AssociativeContainer,
//				PodStruct,
//				Struct,
//				Unknown,
//				UnknownContainer
//			};
//
//			
//
//			template<typename T, typename Enable = bool>
//			struct TypeMetaData
//			{
//				using Type = T;
//				static constexpr bool IsDynamic = true;
//				static constexpr ETypeClass TypeClass = ETypeClass::Unknown;
//				static constexpr SizeT Size = sizeof(T);
//			};
//
//			template<typename T>
//			struct TypeMetaData<T, typename TypeTraits::EnableIfV<TypeTraits::IsNumeric<T>, bool>>
//			{
//				using Type = T;
//				static constexpr bool IsDynamic = false;
//				static constexpr ETypeClass TypeClass = ETypeClass::Numeric;
//				static constexpr SizeT Size = sizeof(T);
//				static constexpr auto TypeName = CTTI::SimplTypeName<Type>();
//			};
//
//			template<typename T>
//			struct TypeMetaData<T, typename TypeTraits::EnableIfV<TypeTraits::IsCharacter<T>, bool>>
//			{
//				using Type = T;
//				using SubType = void;
//				static constexpr bool IsDynamic = false;
//				static constexpr ETypeClass TypeClass = ETypeClass::Character;
//				static constexpr SizeT Size = sizeof(T);
//				static constexpr auto TypeName = CTTI::SimplTypeName<Type>();
//			};
//
//			template<typename T>
//			struct IsContainer : TypeTraits::LogicFalse {};
//
//			template<template <typename THeap, typename T> typename TContainer, typename THeap, typename T>
//			struct IsContainer<TContainer<THeap, T>> : TypeTraits::LogicTrue
//			{
//				using Container = TContainer<THeap, T>;
//				using Type = T;
//				static constexpr ETypeClass ContainerClass = ETypeClass::UnknownContainer;
//			};
//
//			template<typename THeap>
//			struct IsContainer<BaseString<THeap>> : TypeTraits::LogicTrue
//			{
//				using Container = BaseString<THeap>;
//				using Type = UChar;
//				static constexpr ETypeClass ContainerClass = ETypeClass::String;
//				static constexpr auto ContainerTypeName = CTTI::SimplTypeName<Container>();
//			};
//
//			template<typename THeap, typename T>
//			struct IsContainer<BaseList<THeap, T>> : TypeTraits::LogicTrue
//			{
//				using Container = BaseList<THeap, T>;
//				using Type = T;
//				static constexpr ETypeClass ContainerClass = ETypeClass::Container;
//				static constexpr auto ContainerTypeName = CTTI::SimplTypeName<Container>();
//			};
//
//			template<typename THeap, typename TKey, typename TValue>
//			struct IsContainer<BaseMap<THeap, TKey, TValue>> : TypeTraits::LogicTrue
//			{
//				using Container = BaseList;
//				using Type = Pair<TKey, TValue>;
//				static constexpr ETypeClass ContainerClass = ETypeClass::AssociativeContainer;
//				static constexpr auto ContainerTypeName = CTTI::SimplTypeName<Container>();
//			};
//
//			template<typename T>
//			struct IsContainer<Array<T>> : TypeTraits::LogicTrue
//			{
//				using Container = Array;
//				using Type = T;
//				static constexpr ETypeClass ContainerClass = ETypeClass::Container;
//				static constexpr auto ContainerTypeName = CTTI::SimplTypeName<Container>();
//			};
//
//			template<typename TKey, typename TValue>
//			struct TypeMetaData<Pair<TKey, TValue>, bool> {
//				using Type = Pair<TKey, TValue>;
//				using Key = TKey;
//				using Value = TValue;
//				static constexpr bool IsDynamic = false;
//				static constexpr ETypeClass TypeClass = ETypeClass::Pair;
//				static constexpr SizeT Size = sizeof(Type);
//				static constexpr auto TypeName = CTTI::SimplTypeName<Type>();
//
//				using KeyTypeMetaData = TypeMetaData<Key>;
//				using ValueTypeMetaData = TypeMetaData<Value>;
//			};
//
//			template<typename T>
//			struct TypeMetaData<T, typename TypeTraits::EnableIfV<IsContainer<T>::State, bool>>
//			{
//				using Type = typename IsContainer<T>::Container;
//				using SubType = typename IsContainer<T>::Type;
//				static constexpr bool IsDynamic = true;
//				static constexpr ETypeClass TypeClass = IsContainer<T>::ContainerClass;
//				static constexpr SizeT Size = sizeof(T);
//				static constexpr auto TypeName = CTTI::SimplTypeName<T>();
//
//				using SubTypeMetaData = TypeMetaData<SubType>;
//			};
//
//			template<typename T>
//			struct TypeMetaData<T, typename TypeTraits::EnableIfV<TypeTraits::IsPodV<T>, bool>>
//			{
//				using Type = T;
//				static constexpr bool IsDynamic = false;
//				static constexpr ETypeClass TypeClass = ETypeClass::PodStruct;
//				static constexpr SizeT Size = sizeof(T);
//			};
//
//			struct Data {
//				String Name;
//				Map<String, String> Storage;
//			};
//
//			template<typename T, typename TypeStringT, SizeT Offset>
//			struct Field {
//				using FieldType = T;
//				static constexpr auto FieldName = CTTI::TypeStringToArray(TypeStringT()).Resize<TypeStringT::Size() - 1>();
//				static constexpr SizeT FieldSize = sizeof(T);
//				static constexpr SizeT FieldOffset = Offset;
//				static constexpr auto FieldTypeName = CTTI::SimplTypeName<T>().Resize<CTTI::SimplTypeName<T>().Count - 1>();
//				static constexpr bool IsSimple = TypeTraits::IsNumeric<T> || TypeTraits::IsCharacter<T> || TypeTraits::IsBoolean<T>;
//				using MetaData = TypeMetaData<T>;
//			};
//
#define StructFieldData(Struct, Name) \
	Seed::Core::TypeTraits::Types::Field<decltype(Struct::Name), GenTypeString(#Name) , StructOffsetOfMember(Struct, Name)>
//
//			template<typename TRecord>
//			constexpr auto RecordMetaData() {
//				if constexpr(TRecord::TypeClass == ETypeClass::String) {
//					return CTTI::CompileStringView("{").AsArray<1>() +
//						TField::FieldName +
//						CTTI::CompileStringView("|").AsArray<1>() +
//						CTTI::ToCharStr<TField::FieldSize>() +
//						CTTI::CompileStringView("|").AsArray<1>() +
//						TField::FieldTypeName +
//						CTTI::CompileStringView("}").AsArray<1>();
//				}
//			}
//
//			template<typename TField>
//			constexpr auto FieldMetaData() {
//				return
//					CTTI::CompileStringView("{").AsArray<1>() +
//					TField::FieldName +
//					CTTI::CompileStringView("|").AsArray<1>() +
//					CTTI::ToCharStr<TField::FieldSize>() +
//					CTTI::CompileStringView("|").AsArray<1>() +
//					TField::FieldTypeName +
//					CTTI::CompileStringView("}").AsArray<1>();
//			}
//
//			template<typename Tuple, SizeT RevI = 0>
//			constexpr auto FieldsMetaData() {
//				if constexpr (RevI == Tuple::Count())
//					return CTTI::ConstexprArray<char, 1>();
//				else
//				{
//					if constexpr  (RevI + 1 == Tuple::Count()) {
//						using T = typename Tuple::Item<RevI>;
//						return
//							FieldMetaData<T>() +
//							FieldsMetaData<Tuple, RevI + 1>();
//					}
//					else {
//						using T = typename Tuple::Item<RevI>;
//						return
//							FieldMetaData<T>() +
//							CTTI::CompileStringView("|").AsArray<1>() +
//							FieldsMetaData<Tuple, RevI + 1>();
//					}
//						//CTTI::TypeName<typename T::FieldType>().Resize<CTTI::TypeName<typename T::FieldType>().Count - 1>() + CTTI::CompileStringView("|").AsArray<1>() + FieldsMetaData<Tuple, RevI + 1>();
//				}
//			}
//
//			template<typename T, bool AllowSerilize, bool AllowDeserilize, bool Internal, typename ... TFields>
//			struct CustomStruct : TypeTraits::LogicTrue
//			{
//
//			public:
//				using Type = T;
//				static constexpr auto Name = CTTI::SimplTypeName<T>().Resize<CTTI::SimplTypeName<T>().Count - 1>();
//				static constexpr auto Size = sizeof(T);
//				static constexpr auto IsAllowSerilize = AllowSerilize;
//				static constexpr auto IsAllowDeserlize = AllowDeserilize;
//				static constexpr auto IsInternal = Internal;
//
//				static constexpr SizeT GetFieldsSize = [&]() {
//					SizeT result = 0;
//					for (auto s : { TFields::FieldSize... }) result += s;
//					return result;
//				}();
//
//				using Fields = typename Tuple<TFields...>;
//
//				static constexpr auto SubTypeMetaData = FieldsMetaData<Fields>();
//			};
//
//			template<typename T>
//			struct IsCustomStructImplT : LogicFalse {};
//
//			template<typename T, bool AllowSerilize, bool AllowDeserilize, bool Internal, typename ... TFields>
//			struct IsCustomStructImplT<CustomStruct<T, AllowSerilize, AllowDeserilize, Internal, TFields...>> : TypeTraits::LogicTrue {};
//
//			template<typename T>
//			struct IsCustomStructT : IsCustomStructImplT<T> {};
//
//			template<typename T>
//			struct TypeMetaData<T, typename TypeTraits::EnableIfV<IsCustomStructT<T>::State, bool>>
//			{
//				using Type = typename T::Type;
//				static constexpr bool IsDynamic = false;
//				static constexpr ETypeClass TypeClass = ETypeClass::Struct;
//				static constexpr SizeT Size = sizeof(T::Type);
//				using TypeDescriptor = T;
//			};
//
//			template<typename T>
//			class IStaticSerilizable {
//			protected:
//				using MetaData = typename T::InternalMetaData;
//				friend constexpr auto GetTypeDescription<IStaticSerilizable<T>, true>();
//			public:
//				virtual ~IStaticSerilizable() = 0 {}
//			};
//
//			template<typename T>
//			class IDynamicInterfaceSerilizable {
//			protected:
//				using MetaData = typename T::InternalDynamicInterfaceMetaData;
//				friend constexpr auto GetTypeDescription<IStaticSerilizable<T>, true>();
//			public:
//				virtual ~IDynamicSerilizable() = 0 {}
//			};
//
//			class DynamicSerilizable : Seed::IDisposable {
//			private:
//				struct Data {
//					Seed::RefCountT RefCount;
//					String Name;
//					SmartPtr<List<String>> Data;
//				};
//			private:
//				Data* This;
//			protected:
//				virtual void Dispose() override;
//			public:
//				RefCountClass(DynamicSerilizable, This);
//			public:
//				friend class IDynamicInterfaceSerilizable<DynamicSerilizable>;
//			};

			class SerilizableClass : Seed::IDisposable, TypeTraits::Types::ISerilizable {
			private:
				struct Data {
					Seed::RefCountT RefCount;
					String Name;
					Map<String, String> Storage;
					Data() : RefCount(1) {}
				};
			private:
				Data* This;
			public:
				using InternalMetaData = TypeTraits::Types::CustomStruct<Data, true, true, false,
					StructFieldData(Data, Name),
					StructFieldData(Data, Storage)>;
			private:
				virtual Byte* GetDataPtr() const override { return reinterpret_cast<Byte*>(This); }
			protected:
				virtual void Dispose() override {}
			public:
				String Name() & { return This->Name; }
				Map<String, String> Storage() & { return This->Storage; }
			public:
				SerilizableClass() : This(new Data) {}
			public:
				RefCountClass(SerilizableClass, This);
			};
			
			/*template<typename T, typename B>
			constexpr auto GetTypeDescription() {
				Throw(Exceptions::InvalidTypeException(ExceptionPosition, typeid(T)));
			}

			template<typename T, TypeTraits::EnableIfV<IDynamicInterfaceSerilizable<T>::MetaData::State, bool> = true>
			constexpr auto GetTypeDescription() {
				using TypeMetaData = IStaticSerilizable<T>::MetaData;

				constexpr auto v = TypeMetaData::GetFieldsSize;
				
				return
					TypeMetaData::Name +
					CTTI::CompileStringView("|").AsArray<1>() +
					((TypeMetaData::IsAllowDeserlize) ? CTTI::CompileStringView("+").AsArray<1>() : CTTI::CompileStringView("-").AsArray<1>()) +
					CTTI::CompileStringView("|").AsArray<1>() +
					((TypeMetaData::IsAllowSerilize) ? CTTI::CompileStringView("+").AsArray<1>() : CTTI::CompileStringView("-").AsArray<1>()) +
					CTTI::CompileStringView("|").AsArray<1>() +
					((TypeMetaData::IsInternal) ? CTTI::CompileStringView("+").AsArray<1>() : CTTI::CompileStringView("-").AsArray<1>()) +
					CTTI::CompileStringView("|").AsArray<1>() +
					CTTI::ToCharStr<TypeMetaData::GetFieldsSize>() +
					CTTI::CompileStringView("|").AsArray<1>() +
					TypeMetaData::SubTypeMetaData;
			}

			template<typename T, TypeTraits::EnableIfV<IStaticSerilizable<T>::MetaData::State, bool> = true>
			constexpr auto GetTypeDescription() {
				using TypeMetaData = IStaticSerilizable<T>::MetaData;

			}

			constexpr auto v12 = GetTypeDescription<SerilizableClass>();*/
		}
	}
}