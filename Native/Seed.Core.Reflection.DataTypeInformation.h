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
					static constexpr auto SerelizeFrom = [] (const DynamicObject& data) -> Containers::Array<Byte>
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							if (data.GetImplementationTypeInfo() != typeid(T))
							{
								Throw(Reflection::InvlaidDynamicFunctionArgs(ExceptionPosition));
							}
							else
							{
								return Serilization::Serilize(data.GetValue<T>());
							}
						}
						else
						{
							Throw(NonSerelizableType(ExceptionPosition, ExceptionString(CTTI::TypeName<T>())));
						}
					};

					static constexpr auto DeserelizeToDynamic = [] (const Containers::Array<Byte>& rawData) -> DynamicObject
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							T object;
							Serilization::Deserilize(rawData, object);
							return DynamicObject(Containers::Tuple<T>(object));
						}
						else
						{
							Throw(NonSerelizableType(ExceptionPosition, ExceptionString(CTTI::TypeName<T>())));
						}
					};

					static constexpr auto Builder = [] () -> DynamicObject
					{
						if constexpr (Core::TypeTraits::IsConstructible<T>)
						{
							return DynamicObject(Containers::Tuple<T>(T{}));
						}
						else
						{
							Throw(NullPointException(ExceptionPosition));
						}
					};
				};
			public:
				using SerelizeFunctionType = Containers::Array<Byte>(*)(const DynamicObject&);
				using DeSerelizeFunctionType = DynamicObject(*)(const Containers::Array<Byte>&);
				using BuilderFunctionType = DynamicObject(*)();
			private:
				struct Data {
					String typeName;
					String cxxTypeName;
					String typeMetaData;
					Boolean isSereilizable;

					SerelizeFunctionType serelizeFunction;
					DeSerelizeFunctionType deSerelizeFunction;
					BuilderFunctionType builderFunction;

					const TypeInfo& typeInfo;

					RefCountT RefCount;

					Data(
						String&& typeName,
						String&& cxxTypeName,
						String&& typeMetaData,
						Boolean isSereilizable,
						SerelizeFunctionType serelizeFunction,
						DeSerelizeFunctionType deSerelizeFunction,
						BuilderFunctionType builderFunction,
						const TypeInfo& typeInfo
					) :
						typeName(MoveRValue(typeName)),
						cxxTypeName(MoveRValue(cxxTypeName)),
						typeMetaData(MoveRValue(typeMetaData)),
						isSereilizable(isSereilizable),
						serelizeFunction(serelizeFunction),
						deSerelizeFunction(deSerelizeFunction),
						builderFunction(builderFunction),
						typeInfo(typeInfo),
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
					CTTI::TypeName<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>(),
					CTTI::CXXTypeName<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>(),
					Serilization::MetaDataGen<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>(),
					CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>::IsSerilizable,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>::SerelizeFrom,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>::DeserelizeToDynamic,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>::Builder,
					typeid(TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>)
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
					
				const TypeInfo& GetTypeInfo() const {
					return data->typeInfo;
				}

				String Property(GetTypeName) TypeName;
				String Property(GetCxxTypeName) CxxTypeName;
				String Property(GetTypeMetaData) TypeMetaData;
				bool Property(GetIsSerelizableType) IsSerelizableType;
				SerelizeFunctionType Property(GetSerelizeFunction) SerelizeFunction;
				DeSerelizeFunctionType Property(GetDeSerelizeFunction) DeSerelizeFunction;
				BuilderFunctionType Property(GetBuilderFunction) BuilderFunction;

				RefCountClass(DataTypeRecord, data);
			public:
				bool operator==(const DataTypeRecord& other) {
					return this->data->typeInfo == other.data->typeInfo ||
						this->data->cxxTypeName == other.data->cxxTypeName;
				}

				bool operator!=(const DataTypeRecord& other) {
					return this->data->typeInfo != other.data->typeInfo &&
						this->data->cxxTypeName != other.data->cxxTypeName;
				}

				bool operator==(const TypeInfo& other) {
					return this->data->typeInfo == other;
				}

				bool operator!=(const TypeInfo& other) {
					return this->data->typeInfo != other;
				}
			};
		}
	}
}