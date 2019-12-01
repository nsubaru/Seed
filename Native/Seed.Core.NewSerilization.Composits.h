#pragma once
#include "Seed.Core.NewSerilization.Collection.h"

namespace Seed
{
	inline namespace Core
	{
		namespace NewSerilization
		{
			namespace Details
			{
				template<typename T>
				constexpr bool IsSerilizableTuple = NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::Tuple
					&& NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<typename ... TTypes> typename TTuple, typename ... TTypes>
				struct BaseSerlizationHandler<THeap, TTuple<TTypes...>, typename TypeTraits::EnableIfV<IsSerilizableTuple<TTuple<TTypes...>>, void>> {
					using Tuple = TTuple<TTypes...>;

					template<SizeT Index = 0>
					forceinline static void SerilizeTupleItem(const Tuple& data, Byte* rawStream, SizeT& offset){
						if constexpr(Index == sizeof...(TTypes))
							return;
						else{
							using Type = typename Tuple::ItemType<Index>;
							BaseSerlizationHandler<THeap, Type>::SerilizeImpl(data.GetByIdx<Index>(), rawStream, offset);
							SerilizeTupleItem<Index + 1>(data, rawStream, offset);
						}
					}

					template<SizeT Index = 0>
					forceinline static void DeserelizeTupleItem(Tuple& data, const Byte* rawStream, SizeT& offset){
						if constexpr(Index == sizeof...(TTypes))
							return;
						else{
							using Type = typename Tuple::ItemType<Index>;
							BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(data.GetByIdx<Index>(), rawStream, offset);
							DeserelizeTupleItem<Index + 1>(data, rawStream, offset);
						}
					}

					template<SizeT Index = 0>
					forceinline static SizeT DataSizeTuppleItem(const Tuple& data){
						if constexpr(Index == sizeof...(TTypes))
							return 0;
						else{
							using Type = typename Tuple::ItemType<Index>;
							return BaseSerlizationHandler<THeap, Type>::DataSize(data.GetByIdx<Index>())
								+
							DataSizeTuppleItem<Index + 1>(data);
						}
					}

					template<SizeT Index = 0>
					constexpr forceinline static auto TypeMetaDataTuppleItem(){
						if constexpr(Index == sizeof...(TTypes))
							return NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
						else{
							using Type = typename Tuple::ItemType<Index>;
							return BaseSerlizationHandler<THeap, Type>::TypeMetaData()
								+
							TypeMetaDataTuppleItem<Index + 1>();
						}
					}

					forceinline static void SerilizeImpl(const Tuple& data, Byte* rawStream, SizeT& offset) {
						SerilizeTupleItem(data, rawStream, offset);
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const Tuple& data) {
						return DataSizeTuppleItem(data);
					}

					forceinline static constexpr auto TypeHint() {
						return
							NewCTTI::Details::ToCharStr<sizeof...(TTypes)>();
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<Tuple>().Resize<NewCTTI::TypeName<Tuple>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(Tuple)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeMetaDataTuppleItem() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(Tuple& data, const Byte* rawStream, SizeT& offset) {
						DeserelizeTupleItem(data, rawStream, offset);
					}
				};

				template<typename T>
				constexpr bool IsSerilizableStruct = NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::Struct
					&& NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, typename TStruct>
				struct BaseSerlizationHandler<THeap, TStruct, typename TypeTraits::EnableIfV<IsSerilizableStruct<TStruct>, void>> {
					using StructFieldsMetaData = typename NewCTTI::TypeTraits::CTTITypeMetaData<TStruct>::Fields;
					using ISerilizable = NewCTTI::TypeTraits::ISerilizable<TStruct>;

					template<SizeT I = 0>
					forceinline static void SerelizeFieldsImpl(Byte * fiedsPtr, Byte* rawStream, SizeT& offset) {
						if constexpr (I < StructFieldsMetaData::Count)
						
						{
							using FieldMetaData = typename StructFieldsMetaData::ItemType<I>;
							using Type = typename FieldMetaData::Type;

							if constexpr (I + 1 < StructFieldsMetaData::Count){
								BaseSerlizationHandler<THeap, Type>::SerilizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), 
									rawStream, 
									offset);
								SerelizeFieldsImpl<I + 1>(fiedsPtr, rawStream, offset);
							}
							else
							{
								BaseSerlizationHandler<THeap, Type>::SerilizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), 
									rawStream, 
									offset);
							}
						}
						else
						{
							return;
						}
					}

					
					template<SizeT I = 0>
					forceinline static void DeserelizeFieldsImp(Byte * fiedsPtr, const Byte* rawStream, SizeT& offset){
						if constexpr (I < StructFieldsMetaData::Count)
						{
							using FieldMetaData = typename StructFieldsMetaData::ItemType<I>;
							using Type = typename FieldMetaData::Type;

							if constexpr (I + 1 < StructFieldsMetaData::Count){
								BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), 
									rawStream, 
									offset);
								DeserelizeFieldsImp<I + 1>(fiedsPtr, rawStream, offset);
							}
							else{
								BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), 
									rawStream, 
									offset);
							}
						}
						else
						{
							return;
						}
					}

					template<SizeT I = 0>
					forceinline static SizeT DataSizeFieldsImp(Byte * fiedsPtr){
						if constexpr (I < StructFieldsMetaData::Count)
						{
							using FieldMetaData = typename StructFieldsMetaData::ItemType<I>;
							using Type = typename FieldMetaData::Type;

							if constexpr (I + 1 < StructFieldsMetaData::Count)
								return 
									BaseSerlizationHandler<THeap, Type>::DataSize(
										*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset))
									+
									DataSizeFieldsImp<I + 1>(fiedsPtr);
							else
								return
									BaseSerlizationHandler<THeap, Type>::DataSize(
										*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset));
						}
						else
						{
							return 0;
						}
					}

					template<SizeT I = 0>
					constexpr forceinline static auto TypeHintFieldsImp(){
						if constexpr (I < StructFieldsMetaData::Count)
						{
							using FieldMetaData = typename StructFieldsMetaData::ItemType<I>;
							using Type = typename FieldMetaData::Type;

							if constexpr (I + 1 < StructFieldsMetaData::Count)
								return
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
									FieldMetaData::FieldName.Resize<FieldMetaData::FieldName.Count - 1>() +
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
									NewCTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData::FieldOffset)>() +
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
									NewCTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData::FieldSize)>() +
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
									NewCTTI::TypeName<Type>().Resize<NewCTTI::TypeName<Type>().Count - 1>() +
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")())
									+
									TypeHintFieldsImp<I + 1>();
							else
								return
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
									FieldMetaData::FieldName.Resize<FieldMetaData::FieldName.Count - 1>() +
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
									NewCTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData::FieldOffset)>() +
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
									NewCTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData::FieldSize)>() +
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
									NewCTTI::TypeName<Type>().Resize<NewCTTI::TypeName<Type>().Count - 1>() +
									NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
						}
						else
						{
							return NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
						}
					}

					template<SizeT I = 0>
					constexpr forceinline static auto TypeMetaDataFieldsImp(){
						if constexpr (I < StructFieldsMetaData::Count)
						{
							using FieldMetaData = typename StructFieldsMetaData::ItemType<I>;
							using Type = typename FieldMetaData::Type;

							if constexpr (I + 1 < StructFieldsMetaData::Count)
								return BaseSerlizationHandler<THeap, Type>::TypeMetaData()
									+
								TypeMetaDataFieldsImp<I + 1>();
							else
								return BaseSerlizationHandler<THeap, Type>::TypeMetaData();
						}
						else
						{
							return NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
						}
					}

					forceinline static void SerilizeImpl(const TStruct& data, Byte* rawStream, SizeT& offset) {
						const auto& iSerelizeable = static_cast<const ISerilizable&>(data);
						SerelizeFieldsImpl(iSerelizeable.GetDataPtr(), rawStream, offset);
					}

					forceinline static void DeserelizeImpl(TStruct& data, const Byte* rawStream, SizeT& offset) {
						auto& iSerelizeable = static_cast<ISerilizable&>(data);
						DeserelizeFieldsImp(iSerelizeable.GetDataPtr(), rawStream, offset);
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const TStruct& data) {
						const auto& iSerelizeable = static_cast<const ISerilizable&>(data);
						return DataSizeFieldsImp(iSerelizeable.GetDataPtr());
					}

					forceinline static constexpr auto TypeHint() {
						return
							TypeHintFieldsImp();
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<TStruct>().Resize<NewCTTI::TypeName<TStruct>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<TStruct>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TStruct)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeMetaDataFieldsImp() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
				};
			}
		}
	}
}