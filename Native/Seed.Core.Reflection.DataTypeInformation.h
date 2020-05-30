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
								Throw(Reflection::InvlaidDynamicFunctionArgs(__FILE__, "", __LINE__));
							}
							else
							{
								return Serilization::Serilize(data.GetValue<T>());
							}
						}
						else
						{
							Throw(NonSerelizableType(__FILE__, "", __LINE__, ExceptionString(CTTI::TypeName<T>())));
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
							Throw(NonSerelizableType(__FILE__, "", __LINE__, ExceptionString(CTTI::TypeName<T>())));
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
							Throw(NullPointException(__FILE__, "", __LINE__));
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
						RTTI::RTTITypeRecord&& typeInfo
					) :
						typeName(MoveRValue(typeName)),
						cxxTypeName(MoveRValue(cxxTypeName)),
						typeMetaData(MoveRValue(typeMetaData)),
						isSereilizable(isSereilizable),
						serelizeFunction(serelizeFunction),
						deSerelizeFunction(deSerelizeFunction),
						builderFunction(builderFunction),
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
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::DeserelizeToDynamic,
					DataTypeMetaFunctionsBuilder<TypeTraits::RemoveCVRefV<T>>::Builder,
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