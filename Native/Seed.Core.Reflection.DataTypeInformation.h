#pragma once
#include "Seed.Core.Reflection.Base.h"
#include "Seed.Core.Containers.h"
#include "Seed.Core.Exceptions.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Reflection
		{
			using NonSerelizableType = Serilization::NonSerelizableType;

			class DataTypeRecord : IDisposable, Serilization::ISerilizable<DataTypeRecord> {
			private:
				template<typename T>
				struct DataTypeMetaFunctionsBuilder {
					static constexpr auto SerelizeFrom = [] (
						const DynamicObject& data, 
						Byte* rawStream, 
						SizeT& offset, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> void
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple))
						{
							if (data.GetImplementationTypeInfo() != typeid(T))
							{
								Throw(Reflection::InvlaidDynamicFunctionArgs(__FILE__, "SerelizeFrom", __LINE__));
							}
							else
							{
								const T& tuple = data.GetTuple<T>();
								return Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::SerilizeImpl
											(tuple, rawStream, offset, registry);
							}
						}
						else
						{
							Throw(NonSerelizableType(__FILE__, "", __LINE__, ExceptionString(CTTI::TypeName<T>())));
						}
					};

					static constexpr auto SerelizeFromDynamic = [] (
						const Containers::Dynamic& data, 
						Byte* rawStream, 
						SizeT& offset, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> void
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple))
						{
							if (data.GetVariableType() != typeid(T))
							{
								Throw(Reflection::InvlaidDynamicFunctionArgs(__FILE__, "SerelizeFromDynamic", __LINE__));
							}
							else
							{
								const T& tuple = data.GetVariablesTuple<T>();
								return Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::SerilizeImpl
										(tuple, rawStream, offset, registry);
							}
						}
						else
						{
							Throw(NonSerelizableType(__FILE__, "", __LINE__, ExceptionString(CTTI::TypeName<T>())));
						}
					};

					static constexpr auto DeserelizeTo = [] (
						DynamicObject& data, 
						const Byte* rawStream, 
						SizeT& offset, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> void
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple))
						{
							T object = T();
							Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::DeserelizeImpl(object, rawStream, offset, registry);
							data = DynamicObject(MoveRValue(object));
						}
						else
						{
							Throw(NonSerelizableType(__FILE__, "DeserelizeTo", __LINE__, ExceptionString(CTTI::TypeName<T>())));
						}
					};

					static constexpr auto DeserelizeToDynamic = [] (
						Containers::Dynamic& data,
						const Byte* rawStream, 
						SizeT& offset, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> void
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple))
						{
							T object = T();
							Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::DeserelizeImpl(object, rawStream, offset, registry);
							data = Containers::Dynamic(MoveRValue(object));
						}
						else
						{
							Throw(NonSerelizableType(__FILE__, "DeserelizeToDynamic", __LINE__, ExceptionString(CTTI::TypeName<T>())));
						}
					};

					static constexpr auto Builder = [] () -> DynamicObject
					{
						if constexpr (Core::TypeTraits::IsConstructible<T>)
						{
							if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable &&
								(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple))
							{
								return DynamicObject(T{});
							}
							else
							{
								Throw(NullPointException(__FILE__, "Builder", __LINE__));
							}

						}
						else
						{
							Throw(NullPointException(__FILE__, "Builder", __LINE__));
						}
					};

					static constexpr auto BuilderDynamic = [] () -> Containers::Dynamic
					{
						if constexpr (Core::TypeTraits::IsConstructible<T>)
						{
							if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable &&
								(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple))
							{
								return Containers::Dynamic(T{});
							}
							else
							{
								Throw(NullPointException(__FILE__, "Builder", __LINE__));
							}
						}
						else
						{
							Throw(NullPointException(__FILE__, "BuilderDynamic", __LINE__));
						}
					};

					static constexpr auto DataSize = [] (
						const DynamicObject& data, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> SizeT
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple))
						{
							return Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::DataSize
								(data.GetTuple<T>(), registry);
						}
						else
						{
							return 0;
						}
					};

					static constexpr auto DataSizeDynamic = [] (
						const Containers::Dynamic& data, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> SizeT
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Tuple))
						{
							return Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::DataSize
								(data.GetVariablesTuple<T>(), registry);
						}
						else
						{
							return 0;
						}
					};
				};
			public:
				using SerelizeFunctionType = void(*)(
					const DynamicObject&, 
					Byte*, 
					SizeT&, 
					UnusedData const Serilization::TypesRegistry&);

				using DeSerelizeFunctionType = void(*)(
					DynamicObject&, 
					const Byte*, 
					SizeT&, 
					UnusedData const Serilization::TypesRegistry&);

				using BuilderFunctionType = DynamicObject(*)();

				using SerelizeToDynamicFunctionType = void(*)(
					const Containers::Dynamic&,
					Byte*, 
					SizeT&, 
					UnusedData const Serilization::TypesRegistry&);

				using DeSerelizeToDynamicFunctionType = void(*)(
					Containers::Dynamic&,
					const Byte*, 
					SizeT&, 
					UnusedData const Serilization::TypesRegistry&);

				using BuilderToDynamicFunctionType = Containers::Dynamic(*)();

				using DataSizeFunctionType = SizeT(*)(const DynamicObject&, UnusedData const Serilization::TypesRegistry&);
				using DataSizeDynamicFunctionType = SizeT(*)(const Containers::Dynamic&, UnusedData const Serilization::TypesRegistry&);
			private:
				struct Data {
					String typeName;
					String cxxTypeName;
					String typeMetaData;
					Boolean isSereilizable;

					SerelizeFunctionType serelizeFunction;
					DeSerelizeFunctionType deSerelizeFunction;
					BuilderFunctionType builderFunction;

					SerelizeToDynamicFunctionType serelizeToDynamicFunction;
					DeSerelizeToDynamicFunctionType deSerelizeToDynamicFunction;
					BuilderToDynamicFunctionType builderToDynamicFunction;

					DataSizeFunctionType dataSizeFunctionType;
					DataSizeDynamicFunctionType dataSizeDynamicFunctionType;

					RTTI::RTTITypeRecord typeInfo;

					RefCountT RefCount;

					Data(
						String&& typeName,
						String&& cxxTypeName,
						String&& typeMetaData,
						Boolean isSereilizable,

						SerelizeFunctionType serelizeFunction,
						DeSerelizeFunctionType deSerelizeFunction,
						BuilderFunctionType builderFunction,

						SerelizeToDynamicFunctionType serelizeToDynamicFunction,
						DeSerelizeToDynamicFunctionType deSerelizeToDynamicFunction,
						BuilderToDynamicFunctionType builderToDynamicFunction,

						DataSizeFunctionType dataSizeFunctionType,
						DataSizeDynamicFunctionType dataSizeDynamicFunctionType,

						RTTI::RTTITypeRecord&& typeInfo
					) :
						typeName(MoveRValue(typeName)),
						cxxTypeName(MoveRValue(cxxTypeName)),
						typeMetaData(MoveRValue(typeMetaData)),
						isSereilizable(isSereilizable),

						serelizeFunction(serelizeFunction),
						deSerelizeFunction(deSerelizeFunction),
						builderFunction(builderFunction),

						serelizeToDynamicFunction(serelizeToDynamicFunction),
						deSerelizeToDynamicFunction(deSerelizeToDynamicFunction),
						builderToDynamicFunction(builderToDynamicFunction),

						dataSizeFunctionType(dataSizeFunctionType),
						dataSizeDynamicFunctionType(dataSizeDynamicFunctionType),

						typeInfo(MoveRValue(typeInfo)),
						RefCount(1)
					{}
				};
			private:
				Data* data;
			protected:
				virtual void Dispose() override {}
				virtual Byte* GetDataPtr() const override {
					return reinterpret_cast<Byte*>(data);
				}
			public:
				DefStructMetaData(Data,
					DefStructFieldData(Data, typeName),
					DefStructFieldData(Data, cxxTypeName),
					DefStructFieldData(Data, typeMetaData),
					DefStructFieldData(Data, isSereilizable)
				);
			public:
				template<typename T>
				DataTypeRecord(TypeTraits::TypeTag<T>) : data(new Data(
					CTTI::TypeName<TypeTraits::RemoveCVRefV<T>>(),
					CTTI::CXXTypeName<TypeTraits::RemoveCVRefV<T>>(),
					Serilization::MetaDataGen<TypeTraits::RemoveCVRefV<T>>(),
					CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<T>>::IsSerilizable,

					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::SerelizeFrom,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::DeserelizeTo,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::Builder,

					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::SerelizeFromDynamic,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::DeserelizeToDynamic,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::BuilderDynamic,

					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::DataSize,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::DataSizeDynamic,

					RTTI::RTTITypeRecord(TypeTraits::TypeTag<TypeTraits::RemoveCVRefV<T>>{})
				))
				{}

				const String& GetTypeName() const {
					return data->typeName;
				}

				const String& GetCxxTypeName() const {
					return data->cxxTypeName;
				}

				const String& GetTypeMetaData() const {
					return data->typeMetaData;
				}

				bool GetIsSerelizableType() const {
					return data->isSereilizable;
				}

				SerelizeFunctionType GetSerelizeFunction() const {
					return data->serelizeFunction;
				}

				DeSerelizeFunctionType GetDeSerelizeFunction() const {
					return data->deSerelizeFunction;
				}

				BuilderFunctionType GetBuilderFunction() const {
					return data->builderFunction;
				}

				SerelizeToDynamicFunctionType GetSerelizeToDynamicFunction() const {
					return data->serelizeToDynamicFunction;
				}

				DeSerelizeToDynamicFunctionType GetDeSerelizeToDynamicFunction() const {
					return data->deSerelizeToDynamicFunction;
				}

				BuilderToDynamicFunctionType GetBuilderToDynamicFunction() const {
					return data->builderToDynamicFunction;
				}

				DataSizeFunctionType GetDataSizeFunction() const {
					return data->dataSizeFunctionType;
				}

				DataSizeDynamicFunctionType GetDataSizeDynamicFunction() const {
					return data->dataSizeDynamicFunctionType;
				}
					
				const RTTI::RTTITypeRecord& GetTypeInfo() const {
					return data->typeInfo;
				}

				String Property(GetTypeName) TypeName;
				String Property(GetCxxTypeName) CxxTypeName;
				String Property(GetTypeMetaData) TypeMetaData;
				bool Property(GetIsSerelizableType) IsSerelizableType;

				SerelizeFunctionType Property(GetSerelizeFunction) SerelizeFunction;
				DeSerelizeFunctionType Property(GetDeSerelizeFunction) DeSerelizeFunction;
				BuilderFunctionType Property(GetBuilderFunction) BuilderFunction;

				SerelizeToDynamicFunctionType Property(GetSerelizeToDynamicFunction) SerelizeToDynamicFunction;
				DeSerelizeToDynamicFunctionType Property(GetDeSerelizeToDynamicFunction) DeSerelizeToDynamicFunction;
				BuilderToDynamicFunctionType Property(GetBuilderToDynamicFunction) BuilderToDynamicFunction;

				DataSizeFunctionType Property(GetDataSizeFunction) DataSizeFunction;
				DataSizeDynamicFunctionType Property(GetDataSizeDynamicFunction) DataSizeDynamicFunction;

				RefCountClass(DataTypeRecord, data);
			public:
				bool operator==(const DataTypeRecord& other) const {
					return this->data->typeInfo == other.data->typeInfo ||
						this->data->cxxTypeName == other.data->cxxTypeName;
				}

				bool operator!=(const DataTypeRecord& other) const {
					return this->data->typeInfo != other.data->typeInfo &&
						this->data->cxxTypeName != other.data->cxxTypeName;
				}

				bool operator==(const TypeInfo& other) const {
					return this->data->typeInfo.TypeId() == other;
				}

				bool operator!=(const TypeInfo& other) const {
					return this->data->typeInfo.TypeId() != other;
				}

				bool operator==(const RTTI::RTTITypeRecord & other) const {
					return this->data->typeInfo == other;
				}

				bool operator!=(const RTTI::RTTITypeRecord & other) const {
					return this->data->typeInfo != other;
				}

				auto operator<=>(const RTTI::RTTITypeRecord& other) const {
					return this->data->typeInfo <=> other;
				}

				friend auto operator<=>(const RTTI::RTTITypeRecord& other, const DataTypeRecord& item) {
					return other <=> item.data->typeInfo;
				}

				friend auto operator<=>(const TypeInfo& other, const DataTypeRecord& item) {
					return other.HashCode <=> item.data->typeInfo.TypeId().HashCode;
				}
			};
		}
	}
}