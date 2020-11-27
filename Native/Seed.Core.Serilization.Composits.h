#pragma once
#include "Seed.Core.Serilization.Collection.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Serilization
		{
			namespace Details
			{
				template<typename T>
				constexpr bool IsSerilizableTuple = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Tuple
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<typename ... TTypes> typename TTuple, typename ... TTypes>
				struct BaseSerlizationHandler<THeap, TTuple<TTypes...>, typename TypeTraits::EnableIfV<IsSerilizableTuple<TTuple<TTypes...>>, void>> {
					using Tuple = TTuple<TTypes...>;

					template<SizeT Index = 0>
					ForceInline static void SerilizeTupleItem(const Tuple& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry){
						if constexpr(Index == sizeof...(TTypes))
							return;
						else{
							using Type = typename Tuple::template ItemType<Index>;
							BaseSerlizationHandler<THeap, Type>::SerilizeImpl(data.GetByIdxAsRef<Index>(), rawStream, offset, registry);
							SerilizeTupleItem<Index + 1>(data, rawStream, offset, registry);
						}
					}

					template<SizeT Index = 0>
					ForceInline static void DeserelizeTupleItem(Tuple& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry){
						if constexpr(Index == sizeof...(TTypes))
							return;
						else{
							using Type = typename Tuple::template ItemType<Index>;
							BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(data.GetByIdxAsRef<Index>(), rawStream, offset, registry);
							DeserelizeTupleItem<Index + 1>(data, rawStream, offset, registry);
						}
					}

					template<SizeT Index = 0>
					ForceInline static SizeT DataSizeTuppleItem(const Tuple& data, UnusedData const TypesRegistry& registry){
						if constexpr(Index == sizeof...(TTypes))
							return 0;
						else{
							using Type = typename Tuple::template ItemType<Index>;
							return BaseSerlizationHandler<THeap, Type>::DataSize(data.GetByIdxAsRef<Index>(), registry)
								+
							DataSizeTuppleItem<Index + 1>(data, registry);
						}
					}

					template<SizeT Index = 0>
					constexpr ForceInline static auto TypeMetaDataTuppleItem(){
						if constexpr (Index == sizeof...(TTypes))
						{
							return CTTI::Details::CreateTypeStr(L"");
						}
						else
						{
							using Type = typename Tuple::template ItemType<Index>;

							return BaseSerlizationHandler<THeap, Type>::TypeMetaData()
								+
							TypeMetaDataTuppleItem<Index + 1>();
						}
					}

					ForceInline static void SerilizeImpl(const Tuple& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						SerilizeTupleItem(data, rawStream, offset, registry);
					}

					ForceInline static constexpr Boolean IsMemCopy() {
						return false;
					}

					ForceInline static constexpr SizeT DataSize(const Tuple& data, UnusedData const TypesRegistry& registry) {
						return DataSizeTuppleItem(data, registry);
					}

					ForceInline static constexpr auto TypeHint() {
						return
							CTTI::Details::ToCharStr<sizeof...(TTypes)>();
					}

					ForceInline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::CreateTypeStr(L"0");
					}

					ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<Tuple>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(Tuple)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeMetaDataTuppleItem() +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					ForceInline static void DeserelizeImpl(Tuple& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						DeserelizeTupleItem(data, rawStream, offset, registry);
					}

					template<SizeT Index = 0>
					ForceInline static Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>> GetFieldsTypeMetaDataRecord() {
						if constexpr (Index < sizeof...(TTypes))
						{
							using Type = typename Tuple::template ItemType<Index>;

							if constexpr (Index + 1 < sizeof...(TTypes))
								return
									Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>>{
										BaseSerlizationHandler<THeap, Type>::GetTypeMetaDataRecord()
									}
									+=
										GetFieldsTypeMetaDataRecord<Index + 1>();
							else
								return
									Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>>{
										BaseSerlizationHandler<THeap, Type>::GetTypeMetaDataRecord()
									};
						}
						else
						{
							return Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>>{};
						}
					}

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return BaseTypeMetaDataRecord<THeap>(
							CTTI::TypeName<Tuple>(),
							CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeClass,
							sizeof(Tuple),
							0,
							TypeHint(),
							GetFieldsTypeMetaDataRecord()
						);
					}
				};

				template<typename T>
				constexpr bool IsSerilizableObject = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Object
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, typename TObject>
				struct BaseSerlizationHandler<THeap, TObject, typename TypeTraits::EnableIfV<IsSerilizableObject<TObject>, void>> {
					static constexpr auto StructFieldsMetaData = CTTI::TypeTraits::CTTITypeMetaData<TObject>::Fields;
					using ISerilizable = CTTI::TypeTraits::ISerilizable<TObject>;

					template<SizeT I = 0>
					ForceInline static void SerelizeFieldsImpl(Byte * fiedsPtr, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						if constexpr (I < StructFieldsMetaData.Count)
						
						{
							auto FieldMetaData = StructFieldsMetaData.GetByIdx<I>();
							using Type = decltype(FieldMetaData)::template Type;

							if constexpr (I + 1 < StructFieldsMetaData.Count){
								BaseSerlizationHandler<THeap, Type>::SerilizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData.FieldOffset), 
									rawStream, 
									offset, registry);
								SerelizeFieldsImpl<I + 1>(fiedsPtr, rawStream, offset, registry);
							}
							else
							{
								BaseSerlizationHandler<THeap, Type>::SerilizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData.FieldOffset), 
									rawStream, 
									offset, registry);
							}
						}
						else
						{
							return;
						}
					}

					
					template<SizeT I = 0>
					ForceInline static void DeserelizeFieldsImp(Byte * fiedsPtr, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry){
						if constexpr (I < StructFieldsMetaData.Count)
						{
							auto FieldMetaData = StructFieldsMetaData.GetByIdx<I>();
							using Type = decltype(FieldMetaData)::template Type;

							if constexpr (I + 1 < StructFieldsMetaData.Count){
								BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData.FieldOffset), 
									rawStream, 
									offset, registry);
								DeserelizeFieldsImp<I + 1>(fiedsPtr, rawStream, offset, registry);
							}
							else{
								BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData.FieldOffset), 
									rawStream, 
									offset, registry);
							}
						}
						else
						{
							return;
						}
					}

					template<SizeT I = 0>
					ForceInline static SizeT DataSizeFieldsImp(Byte * fiedsPtr, UnusedData const TypesRegistry& registry){
						if constexpr (I < StructFieldsMetaData.Count)
						{
							auto FieldMetaData = StructFieldsMetaData.GetByIdx<I>();
							using Type = decltype(FieldMetaData)::template Type;

							if constexpr (I + 1 < StructFieldsMetaData.Count)
								return 
									BaseSerlizationHandler<THeap, Type>::DataSize(
										*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData.FieldOffset), registry)
									+
									DataSizeFieldsImp<I + 1>(fiedsPtr, registry);
							else
								return
									BaseSerlizationHandler<THeap, Type>::DataSize(
										*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData.FieldOffset), registry);
						}
						else
						{
							return 0;
						}
					}

					template<SizeT I = 0>
					constexpr ForceInline static auto TypeHintFieldsImp(){
						if constexpr (I < StructFieldsMetaData.Count)
						{
							auto FieldMetaData = StructFieldsMetaData.GetByIdx<I>();
							using Type = decltype(FieldMetaData)::template Type;

							if constexpr (I + 1 < StructFieldsMetaData.Count)
								return
									CTTI::Details::CreateTypeStr(L"[") +
									FieldMetaData.FieldName +
									CTTI::Details::CreateTypeStr(L"-") +
									CTTI::TypeName<Type>() +
									CTTI::Details::CreateTypeStr(L"]")
									+
									TypeHintFieldsImp<I + 1>();
							else
								return
									CTTI::Details::CreateTypeStr(L"[") +
									FieldMetaData.FieldName +
									CTTI::Details::CreateTypeStr(L"-") +
									CTTI::TypeName<Type>() +
									CTTI::Details::CreateTypeStr(L"]");
						}
						else
						{
							return CTTI::Details::CreateTypeStr(L"");
						}
					}

					template<SizeT I = 0>
					constexpr ForceInline static auto TypeMetaDataFieldsImp(){
						if constexpr (I < StructFieldsMetaData.Count)
						{
							auto FieldMetaData = StructFieldsMetaData.GetByIdx<I>();
							using Type = decltype(FieldMetaData)::template Type;

							if constexpr (I + 1 < StructFieldsMetaData.Count)
								return BaseSerlizationHandler<THeap, Type>::TypeMetaData()
									+
								TypeMetaDataFieldsImp<I + 1>();
							else
								return BaseSerlizationHandler<THeap, Type>::TypeMetaData();
						}
						else
						{
							return CTTI::Details::CreateTypeStr(L"");
						}
					}

					ForceInline static void SerilizeImpl(const TObject& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						const auto& iSerelizeable = static_cast<const ISerilizable&>(data);
						SerelizeFieldsImpl(iSerelizeable.GetDataPtr(), rawStream, offset, registry);
					}

					ForceInline static void DeserelizeImpl(TObject& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						auto& iSerelizeable = static_cast<ISerilizable&>(data);
						DeserelizeFieldsImp(iSerelizeable.GetDataPtr(), rawStream, offset, registry);
					}

					ForceInline static constexpr Boolean IsMemCopy() {
						return false;
					}

					ForceInline static constexpr SizeT DataSize(const TObject& data, UnusedData const TypesRegistry& registry) {
						const auto& iSerelizeable = static_cast<const ISerilizable&>(data);
						return DataSizeFieldsImp(iSerelizeable.GetDataPtr(), registry);
					}

					ForceInline static constexpr auto TypeHint() {
						return
							TypeHintFieldsImp();
					}

					ForceInline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::CreateTypeStr(L"0");
					}

					ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<TObject>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TObject>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(TObject)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeMetaDataFieldsImp() +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

					template<SizeT I = 0>
					ForceInline static Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>> GetFieldsTypeMetaDataRecord() {
						if constexpr (I < StructFieldsMetaData.Count)
						{
							auto FieldMetaData = StructFieldsMetaData.GetByIdx<I>();
							using Type = decltype(FieldMetaData)::template Type;

							if constexpr (I + 1 < StructFieldsMetaData.Count)
								return
									Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>>{
										BaseSerlizationHandler<THeap, Type>::GetTypeMetaDataRecord()
									}
									+=
									GetFieldsTypeMetaDataRecord<I + 1>();
							else
								return
									Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>>{
										BaseSerlizationHandler<THeap, Type>::GetTypeMetaDataRecord()
									};
						}
						else
						{
							return Containers::BaseList<THeap, BaseTypeMetaDataRecord<THeap>>{};
						}
					}

					ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
						return BaseTypeMetaDataRecord<THeap>(
							CTTI::TypeName<TObject>(),
							CTTI::TypeTraits::CTTITypeMetaData<TObject>::TypeClass,
							sizeof(TObject),
							0,
							TypeHint(),
							GetFieldsTypeMetaDataRecord()
						);
					}
				};
			}
		}
	}
}