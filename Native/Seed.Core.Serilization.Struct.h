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
					using StructFieldsMetaData = typename CTTI::TypeTraits::CTTITypeMetaData<TStruct>::Fields;

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
							return CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
						}
					}

					forceinline static void SerilizeImpl(const TStruct& data, Byte* rawStream, SizeT& offset) {
						SerelizeFieldsImpl((Byte*)&data, rawStream, offset);
					}

					forceinline static void DeserelizeImpl(TStruct& data, const Byte* rawStream, SizeT& offset) {
						DeserelizeFieldsImp((Byte*)&data, rawStream, offset);
					}

					forceinline static constexpr Boolean IsMemCopy() {
						return false;
					}

					forceinline static constexpr SizeT DataSize(const TStruct& data) {
						return DataSizeFieldsImp((Byte*)&data);
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
                            CTTI::TypeName<TStruct>().Resize<CTTI::TypeName<TStruct>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TStruct>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TStruct)>() +
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