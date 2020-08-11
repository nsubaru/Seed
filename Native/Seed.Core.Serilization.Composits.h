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
				constexpr bool IsSerilizableTuple = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, template<typename ... TTypes> typename TTuple, typename ... TTypes>
				struct BaseSerlizationHandler<THeap, TTuple<TTypes...>, typename TypeTraits::EnableIfV<IsSerilizableTuple<TTuple<TTypes...>>, void>> {
					using Tuple = TTuple<TTypes...>;

					template<SizeT Index = 0>
					forceinline static void SerilizeTupleItem(const Tuple& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry){
						if constexpr(Index == sizeof...(TTypes))
							return;
						else{
							using Type = typename Tuple::ItemType<Index>;
							BaseSerlizationHandler<THeap, Type>::SerilizeImpl(data.GetByIdx<Index>(), rawStream, offset, registry);
							SerilizeTupleItem<Index + 1>(data, rawStream, offset, registry);
						}
					}

					template<SizeT Index = 0>
					forceinline static void DeserelizeTupleItem(Tuple& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry){
						if constexpr(Index == sizeof...(TTypes))
							return;
						else{
							using Type = typename Tuple::ItemType<Index>;
							BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(data.GetByIdx<Index>(), rawStream, offset, registry);
							DeserelizeTupleItem<Index + 1>(data, rawStream, offset, registry);
						}
					}

					template<SizeT Index = 0>
					forceinline static SizeT DataSizeTuppleItem(const Tuple& data, UnusedData const TypesRegistry& registry){
						if constexpr(Index == sizeof...(TTypes))
							return 0;
						else{
							using Type = typename Tuple::ItemType<Index>;
							return BaseSerlizationHandler<THeap, Type>::DataSize(data.GetByIdx<Index>(), registry)
								+
							DataSizeTuppleItem<Index + 1>(data, registry);
						}
					}

					template<SizeT Index = 0>
					constexpr forceinline static auto TypeMetaDataTuppleItem(){
						if constexpr(Index == sizeof...(TTypes))
							return CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
						else{
							using Type = typename Tuple::ItemType<Index>;
							return BaseSerlizationHandler<THeap, Type>::TypeMetaData()
								+
							TypeMetaDataTuppleItem<Index + 1>();
						}
					}

					forceinline static void SerilizeImpl(const Tuple& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						SerilizeTupleItem(data, rawStream, offset, registry);
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const Tuple& data, UnusedData const TypesRegistry& registry) {
						return DataSizeTuppleItem(data, registry);
					}

					forceinline static constexpr auto TypeHint() {
						return
							CTTI::Details::ToCharStr<sizeof...(TTypes)>();
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<Tuple>().Resize<CTTI::TypeName<Tuple>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<Tuple>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(Tuple)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeMetaDataTuppleItem() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }

					forceinline static void DeserelizeImpl(Tuple& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						DeserelizeTupleItem(data, rawStream, offset, registry);
					}
				};

				template<typename T>
				constexpr bool IsSerilizableObject = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Object
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, typename TObject>
				struct BaseSerlizationHandler<THeap, TObject, typename TypeTraits::EnableIfV<IsSerilizableObject<TObject>, void>> {
					using StructFieldsMetaData = typename CTTI::TypeTraits::CTTITypeMetaData<TObject>::Fields;
					using ISerilizable = CTTI::TypeTraits::ISerilizable<TObject>;

					template<SizeT I = 0>
					forceinline static void SerelizeFieldsImpl(Byte * fiedsPtr, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						if constexpr (I < StructFieldsMetaData::Count)
						
						{
							using FieldMetaData = typename StructFieldsMetaData::ItemType<I>;
							using Type = typename FieldMetaData::Type;

							if constexpr (I + 1 < StructFieldsMetaData::Count){
								BaseSerlizationHandler<THeap, Type>::SerilizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), 
									rawStream, 
									offset, registry);
								SerelizeFieldsImpl<I + 1>(fiedsPtr, rawStream, offset, registry);
							}
							else
							{
								BaseSerlizationHandler<THeap, Type>::SerilizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), 
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
					forceinline static void DeserelizeFieldsImp(Byte * fiedsPtr, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry){
						if constexpr (I < StructFieldsMetaData::Count)
						{
							using FieldMetaData = typename StructFieldsMetaData::ItemType<I>;
							using Type = typename FieldMetaData::Type;

							if constexpr (I + 1 < StructFieldsMetaData::Count){
								BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), 
									rawStream, 
									offset, registry);
								DeserelizeFieldsImp<I + 1>(fiedsPtr, rawStream, offset, registry);
							}
							else{
								BaseSerlizationHandler<THeap, Type>::DeserelizeImpl(
									*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), 
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
					forceinline static SizeT DataSizeFieldsImp(Byte * fiedsPtr, UnusedData const TypesRegistry& registry){
						if constexpr (I < StructFieldsMetaData::Count)
						{
							using FieldMetaData = typename StructFieldsMetaData::ItemType<I>;
							using Type = typename FieldMetaData::Type;

							if constexpr (I + 1 < StructFieldsMetaData::Count)
								return 
									BaseSerlizationHandler<THeap, Type>::DataSize(
										*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), registry)
									+
									DataSizeFieldsImp<I + 1>(fiedsPtr, registry);
							else
								return
									BaseSerlizationHandler<THeap, Type>::DataSize(
										*reinterpret_cast<Type*>(fiedsPtr + FieldMetaData::FieldOffset), registry);
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
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("[")()) +
									FieldMetaData::FieldName.Resize<FieldMetaData::FieldName.Count - 1>() +
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("-")()) +
									CTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData::FieldOffset)>() +
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("-")()) +
									CTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData::FieldSize)>() +
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("-")()) +
									CTTI::TypeName<Type>().Resize<CTTI::TypeName<Type>().Count - 1>() +
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("]")())
									+
									TypeHintFieldsImp<I + 1>();
							else
								return
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("[")()) +
									FieldMetaData::FieldName.Resize<FieldMetaData::FieldName.Count - 1>() +
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("-")()) +
									CTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData::FieldOffset)>() +
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("-")()) +
									CTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData::FieldSize)>() +
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("-")()) +
									CTTI::TypeName<Type>().Resize<CTTI::TypeName<Type>().Count - 1>() +
									CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("]")());
						}
						else
						{
							return CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
						}
					}

					template<SizeT I = 0>
					constexpr forceinline static auto TypeMetaDataFieldsImp(){
						if constexpr (I < StructFieldsMetaData::Count)
						{
							using FieldMetaData = StructFieldsMetaData::ItemType<I>;
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
							return CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
						}
					}

					forceinline static void SerilizeImpl(const TObject& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						const auto& iSerelizeable = static_cast<const ISerilizable&>(data);
						SerelizeFieldsImpl(iSerelizeable.GetDataPtr(), rawStream, offset, registry);
					}

					forceinline static void DeserelizeImpl(TObject& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						auto& iSerelizeable = static_cast<ISerilizable&>(data);
						DeserelizeFieldsImp(iSerelizeable.GetDataPtr(), rawStream, offset, registry);
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const TObject& data, UnusedData const TypesRegistry& registry) {
						const auto& iSerelizeable = static_cast<const ISerilizable&>(data);
						return DataSizeFieldsImp(iSerelizeable.GetDataPtr(), registry);
					}

					forceinline static constexpr auto TypeHint() {
						return
							TypeHintFieldsImp();
					}

					forceinline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("0")());
					}

					forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<TObject>().Resize<CTTI::TypeName<TObject>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TObject>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TObject)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeMetaDataFieldsImp() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
				};
			}
		}
	}
}