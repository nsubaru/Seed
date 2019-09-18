#pragma once
#include "Seed.Core.Containers.SmartPtr.h"
#include "Seed.Core.CTTI.TypeTraits.h"
#include "Seed.Core.CTTI.TypeNames.h"

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			struct TypeMetaData {
				SizeT FullRecordSize;
				SizeT DataSize;
				TypeTraits::Types::ENativeTypes NativeType;
				TypeTraits::Types::EDataClasses DataClass;
				UInt8 IsSerilizable : 1;
				UInt8 IsDeserilizable : 1;
				UInt8 IsDynamic : 1;
				char Name[1];
			};

			template<typename T>
			static __forceinline List<SmartPtr<TypeMetaData>> GetTypeMetaData() {
				struct InternalHelper {
					static SmartPtr<TypeMetaData> BuildTypeMetaData() {
						constexpr SizeT MetDataSize = sizeof(TypeMetaData) + SimplTypeName<T>().Count;
						TypeMetaData* rawTypeMetaData = (TypeMetaData*)new Byte[MetDataSize];
						using CttiMetaData = TypeTraits::Types::MetaData<T>;
						rawTypeMetaData->NativeType = CttiMetaData::NativeType;
						rawTypeMetaData->DataClass = CttiMetaData::DataType;
						rawTypeMetaData->IsSerilizable = 0x1;
						rawTypeMetaData->IsDeserilizable = 0x1;
						rawTypeMetaData->IsDynamic = 0x0;
						rawTypeMetaData->DataSize = sizeof(T);
						rawTypeMetaData->FullRecordSize = MetDataSize;
						Memory::Copy(SimplTypeName<T>().Raw(), rawTypeMetaData->Name, SimplTypeName<T>().Count);
						return SmartPtr<TypeMetaData>(MoveRValue(rawTypeMetaData));
					}
				};

				List<SmartPtr<TypeMetaData>> ret;

				if constexpr (TypeTraits::IsSimpleType<T>) {
					ret += InternalHelper::BuildTypeMetaData();
					return ret;
				}
				else if constexpr (TypeTraits::IsPair<T>) {
					ret += InternalHelper::BuildTypeMetaData();
					ret += GetTypeMetaData<TypeTraits::PairTypes<T>::Key>();
					ret += GetTypeMetaData<TypeTraits::PairTypes<T>::Value>();
					return ret;
				}
				else if constexpr (TypeTraits::IsContainer<T>) {
					ret += InternalHelper::BuildTypeMetaData();
					ret += GetTypeMetaData<TypeTraits::ContainerSubType<T>>();
					return ret;
				}
				else
				{
					Throw(NotImplemented(ExceptionPosition));
				}
			}
		}
	}
}