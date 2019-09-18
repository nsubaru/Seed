#pragma once
#include "Seed.Core.CTTI.TypeTraits.h"

namespace Seed {
	inline namespace Core {

		namespace Serlization {
			template<typename THeap>
			class BaseMemoryStream : Seed::IDisposable, Memory::Allocator<THeap> {
			private:
				struct Data : Memory::Allocator<THeap> {
					BaseSlicesList<THeap, Byte> buffer;
					Seed::RefCountT RefCount;
					Data() : RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {}
			private:
				template<typename T, typename Tuple, SizeT I = 0>
				static BaseMemoryStream & FieldsMetaData(const T & Struct, BaseMemoryStream& stream) {
					if constexpr (I == Tuple::Count())
						return stream;
					else
					{
						if constexpr (I + 1 == Tuple::Count()) {
							using FieldMetaData = typename Tuple::Item<I>;
							using FieldType = typename FieldMetaData::FieldType;
							auto ptr = reinterpret_cast<const Byte*>(&Struct) + FieldMetaData::FieldOffset; 
							return SerilizeImpl(reinterpret_cast<const FieldMetaData::FieldType&>(*ptr), stream);
						}
						else {
							using FieldMetaData = typename Tuple::Item<I>;
							using FieldType = typename FieldMetaData::FieldType;
							auto ptr = reinterpret_cast<const Byte*>(&Struct) + FieldMetaData::FieldOffset;
							return FieldsMetaData<T, Tuple, I + 1>(Struct, SerilizeImpl(reinterpret_cast<const FieldMetaData::FieldType&>(*ptr), stream));
						}
					}
				}

				template<typename T>
				static BaseMemoryStream& SerilizeImpl(const T& Value, BaseMemoryStream& stream) {
					if constexpr (
						TypeTraits::Types::MetaData<T>::NativeType == TypeTraits::Types::ENativeTypes::Primitive ||
						TypeTraits::Types::MetaData<T>::NativeType == TypeTraits::Types::ENativeTypes::Pod
						) {
						stream.This->buffer.AddNewSlice(Blob(reinterpret_cast<const Byte*>(&Value), sizeof(T)));
						return stream;
					}
					else if constexpr (
						TypeTraits::Types::MetaData<T>::NativeType == TypeTraits::Types::ENativeTypes::Collection &&
						TypeTraits::Types::MetaData<T>::DataType == TypeTraits::Types::EDataClasses::Character
						)
					{
						stream.This->buffer.AddNewSlice((Blob)Value);
						return stream;
					}
					else if constexpr (
						TypeTraits::Types::MetaData<T>::NativeType == TypeTraits::Types::ENativeTypes::StaticStruct &&
						TypeTraits::Types::MetaData<T>::DataType == TypeTraits::Types::EDataClasses::Binnary
						) {
						SerilizeImpl(Value.Key, stream);
						SerilizeImpl(Value.Value, stream);
						return stream;
					}
					else if constexpr (
						TypeTraits::Types::MetaData<T>::NativeType == TypeTraits::Types::ENativeTypes::Collection
						) {
						SerilizeImpl(Value.Count, stream);
						Value.ForEach([&](const TypeTraits::Types::MetaData<T>::ItemsType& item) { SerilizeImpl(item, stream); });
						return stream;
					}
					else if constexpr (
						TypeTraits::Types::MetaData<T>::NativeType == TypeTraits::Types::ENativeTypes::StaticStruct
						) {
						using DataType = typename TypeTraits::Types::MetaData<T>::Type;
						return FieldsMetaData<DataType, typename TypeTraits::Types::MetaData<T>::AdditionalData::Fields>
							((DataType&)*(DataType*)((TypeTraits::Types::ISerilizable&)Value).GetDataPtr(), stream);
					}
					else if constexpr (
						TypeTraits::Types::MetaData<T>::NativeType == TypeTraits::Types::ENativeTypes::Dynamic
						) {
						Throw(NotImplemented(ExceptionPosition));
					}
					else {
						Throw(InvalidArgumentException(ExceptionPosition, L"Value", L"Type must be serilizable"));
					}
				}
			public:
				BaseMemoryStream() : This(new Data) {}

			public:
				template<typename T>
				BaseMemoryStream & Serilize(const T& Value) {
					return SerilizeImpl(Value, *this);
				}
			public:
				RefCountClass(BaseMemoryStream, This);
			};

			///<summary>Потік даних в пам'яті
			///<para>
			/// Для кожного елемента відбувається підрахунок силок,
			/// що дозволяє корректно його знищити навіть
			/// якщо десь використовується ітератор на елемент
			///</para>
			///</summary>
			///<param name = "T">Тип значеннь</param>
			namespace {
				using MemoryStream = BaseMemoryStream<Seed::Core::Memory::Heap>;
			}

			///<summary>Потік даних в пам'яті з захищеним алокатором
			///<para>
			/// Для кожного елемента відбувається підрахунок силок,
			/// що дозволяє корректно його знищити навіть
			/// якщо десь використовується ітератор на елемент
			///</para>
			///</summary>
			///<param name = "T">Тип значеннь</param>
			namespace {
				using SensitiveMemoryStream = BaseMemoryStream<Seed::Core::Memory::SensitiveDataHeap>;
			}
		}
	}
}