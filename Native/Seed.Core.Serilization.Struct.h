#pragma once
#include "Seed.Core.Serilization.Composits.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Serilization
		{
			namespace Details
			{
				template<typename T>
				constexpr bool IsSerilizableStruct = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Struct
					&& CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

				template<typename THeap, typename TStruct>
				struct BaseSerlizationHandler<THeap, TStruct, typename TypeTraits::EnableIfV<IsSerilizableStruct<TStruct>, void>> {
					static constexpr auto StructFieldsMetaData = CTTI::TypeTraits::CTTITypeMetaData<TStruct>::Fields;

					template<SizeT I = 0>
					ForceInline static void SerelizeFieldsImpl(Byte * fiedsPtr, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						if constexpr (I < StructFieldsMetaData.Count)
						{
							auto FieldMetaData = StructFieldsMetaData.GetByIdx<I>();
							using Type = decltype(FieldMetaData)::Type;
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
							using Type = decltype(FieldMetaData)::Type;

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
							using Type = decltype(FieldMetaData)::Type;

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
							using Type = decltype(FieldMetaData)::Type;

							if constexpr (I + 1 < StructFieldsMetaData.Count)
								return
									CTTI::Details::CreateTypeStr("[") +
									FieldMetaData.FieldName.Resize<FieldMetaData.FieldName.Count - 1>() +
									CTTI::Details::CreateTypeStr("-") +
									CTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData.FieldOffset)>() +
									CTTI::Details::CreateTypeStr("-") +
									CTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData.FieldSize)>() +
									CTTI::Details::CreateTypeStr("-") +
									CTTI::TypeName<Type>().Resize<CTTI::TypeName<Type>().Count - 1>() +
									CTTI::Details::CreateTypeStr("]")
									+
									TypeHintFieldsImp<I + 1>();
							else
								return
									CTTI::Details::CreateTypeStr("[") +
									FieldMetaData.FieldName.Resize<FieldMetaData.FieldName.Count - 1>() +
									CTTI::Details::CreateTypeStr("-") +
									CTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData.FieldOffset)>() +
									CTTI::Details::CreateTypeStr("-") +
									CTTI::Details::ToCharStr<static_cast<SizeT>(FieldMetaData.FieldSize)>() +
									CTTI::Details::CreateTypeStr("-") +
									CTTI::TypeName<Type>().Resize<CTTI::TypeName<Type>().Count - 1>() +
									CTTI::Details::CreateTypeStr("]");
						}
						else
						{
							return CTTI::Details::CreateTypeStr("");
						}
					}

					template<SizeT I = 0>
					constexpr ForceInline static auto TypeMetaDataFieldsImp(){
						if constexpr (I < StructFieldsMetaData.Count)
						{
							auto FieldMetaData = StructFieldsMetaData.GetByIdx<I>();
							using Type = decltype(FieldMetaData)::Type;

							if constexpr (I + 1 < StructFieldsMetaData.Count)
								return BaseSerlizationHandler<THeap, Type>::TypeMetaData()
									+
								TypeMetaDataFieldsImp<I + 1>();
							else
								return BaseSerlizationHandler<THeap, Type>::TypeMetaData();
						}
						else
						{
							return CTTI::Details::CreateTypeStr("");
						}
					}

					ForceInline static void SerilizeImpl(const TStruct& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						SerelizeFieldsImpl((Byte*)&data, rawStream, offset, registry);
					}

					ForceInline static void DeserelizeImpl(TStruct& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
						DeserelizeFieldsImp((Byte*)&data, rawStream, offset, registry);
					}

					ForceInline static constexpr Boolean IsMemCopy() {
						return false;
					}

					ForceInline static constexpr SizeT DataSize(const TStruct& data, UnusedData const TypesRegistry& registry) {
						return DataSizeFieldsImp((Byte*)&data, registry);
					}

					ForceInline static constexpr auto TypeHint() {
						return
							TypeHintFieldsImp();
					}

					ForceInline static constexpr auto ConstexprSize() {
						return
							CTTI::Details::CreateTypeStr("0");
					}

					ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<TStruct>().Resize<CTTI::TypeName<TStruct>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TStruct>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(TStruct)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeMetaDataFieldsImp() +
                            CTTI::Details::CreateTypeStr("}");
                    }
				};
			}
		}
	}
}