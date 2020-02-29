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
			///<summary>Не можливо серелізувати або десерелізувати даний тип</summary>
			class SeedKernelCall NonSerelizableType : public Exception {
			private:
				/// <summary>
				/// Тип який DI не зумів знайти
				/// </summary>
				const TypeInfo& type;
				/// <summary>
				/// Ім'я типу який не знайдено
				/// </summary>
				ExceptionString typeName;
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param Name = 'Type'>Тип який DI не зумів знайти</param>
				NonSerelizableType(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ExceptionString&& typeName, const TypeInfo& type)
					:Exception(File, FunctName, Line, L"Неможливо викликати динамічну функцію з даним набором аргументів"), type(type), typeName(MoveRValue(typeName)) {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString(exeptionName)
						+ ExceptionString(L". Ім'я типу: ")
						+ typeName
						+ ExceptionString(L". ")
						+ location.toString()
						+ ExceptionString(L". ");
				}

				virtual ~NonSerelizableType() {}
			};

			class DataTypeRecord : IDisposable, NewSerilization::ISerilizable<DataTypeRecord> {
			private:
				template<typename T>
				struct DataTypeMetaFunctionsBuilder {
					static constexpr auto SerelizeFrom = [] (const DynamicObject& data) -> Containers::Array<Byte>
					{
						if constexpr (NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							if (data.GetImplementationTypeInfo() != typeid(T))
							{
								Throw(Reflection::InvlaidDynamicFunctionArgs(ExceptionPosition));
							}
							else
							{
								return NewSerilization::Serilize(data.GetValue<T>());
							}
						}
						else
						{
							Throw(NonSerelizableType(ExceptionPosition, ExceptionString(NewCTTI::TypeName<T>()), typeid(T)));
						}
					};

					static constexpr auto DeserelizeToDynamic = [] (const Containers::Array<Byte>& rawData) -> DynamicObject
					{
						if constexpr (NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							T object;
							NewSerilization::Deserilize(rawData, object);
							return DynamicObject(Containers::Tuple<T>(object));
						}
						else
						{
							Throw(NonSerelizableType(ExceptionPosition, ExceptionString(NewCTTI::TypeName<T>()), typeid(T)));
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
					NewCTTI::TypeName<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>(),
					NewCTTI::CXXTypeName<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>(),
					NewSerilization::MetaDataGen<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>(),
					NewCTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCV<TypeTraits::RemoveRefV<T>>>::IsSerilizable,
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