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

			template<typename THeap>
			using BaseTypeMetaDataRecord = Serilization::BaseTypeMetaDataRecord<THeap>;

			using TypeMetaDataRecord = Serilization::TypeMetaDataRecord;
			using SensitiveTypeMetaDataRecord = Serilization::SensitiveTypeMetaDataRecord;

			/// <summary>
			/// Запис про тип даних(який підтримує серелізацію)
			/// </summary>
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
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable || 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Tuple))
						{
							if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Tuple) {
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
							else {
								if (data.GetItemsCount() != 1 || data.GetImplementationItem1TypeInfo() != typeid(T))
								{
									Throw(Reflection::InvlaidDynamicFunctionArgs(__FILE__, "SerelizeFrom", __LINE__));
								}
								else
								{
									const T& item = data.GetValue<T>().GetByIdx<0>();

									return Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::SerilizeImpl
										(item, rawStream, offset, registry);
								}
							}
						}
						else
						{
							Throw(NonSerelizableType(__FILE__, "SerelizeFrom", __LINE__, ExceptionString(CTTI::TypeName<T>())));
						}

						Throw(NonSerelizableType(__FILE__, "SerelizeFrom", __LINE__, ExceptionString(CTTI::TypeName<T>())));
					};

					static constexpr auto SerelizeFromDynamic = [] (
						const Containers::Dynamic& data, 
						Byte* rawStream, 
						SizeT& offset, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> void
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Tuple))
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
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable || CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Tuple) {
							if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && Core::TypeTraits::IsConstructible<T>)
							{
								T object = T();

								Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::DeserelizeImpl(object, rawStream, offset, registry);
								data = DynamicObject(MoveRValue(object));
							}
							else
							{
								Throw(NonSerelizableType(__FILE__, "DeserelizeTo", __LINE__, ExceptionString(CTTI::TypeName<T>())));
							}
						}
						else {
							if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && Core::TypeTraits::IsConstructible<T>) {
								T object = T();

								Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::DeserelizeImpl(object, rawStream, offset, registry);
								data = DynamicObject(MoveRValue(Containers::Tuple<T>(MoveRValue(object))));
							}
							else
							{
								Throw(NonSerelizableType(__FILE__, "DeserelizeTo", __LINE__, ExceptionString(CTTI::TypeName<T>())));
							}
						}
					};

					static constexpr auto DeserelizeToDynamic = [] (
						Containers::Dynamic& data,
						const Byte* rawStream, 
						SizeT& offset, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> void
					{
						if constexpr (Core::TypeTraits::IsConstructible<T> &&
									  CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Tuple))
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
								(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass != CTTI::EType::Tuple))
							{
								return DynamicObject(Containers::Tuple<T>());
							} 
							else if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
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
								(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Tuple))
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
							(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass != CTTI::EType::Tuple))
						{
							const T& val = data.GetValue<T>().GetByIdx<0>();
							return Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::DataSize
								(val, registry);
						} 
						else if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable)
						{
							return Serilization::Details::BaseSerlizationHandler<Memory::Heap, T>::DataSize
								(data.GetTuple<T>(), registry);
						}
						else
						{
							return 0ui64;
						}
					};

					static constexpr auto DataSizeDynamic = [] (
						const Containers::Dynamic& data, 
						UnusedData const Serilization::TypesRegistry& registry
						) -> SizeT
					{
						if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable && 
									 (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Tuple))
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
					String TypeName;
					String CxxTypeName;
					String TypeMetaData;
					Boolean IsSereilizable;

					TypeMetaDataRecord TypeShema;

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

						TypeMetaDataRecord&& typeMetaDataRecord,

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
						TypeName(MoveRValue(typeName)),
						CxxTypeName(MoveRValue(cxxTypeName)),
						TypeMetaData(MoveRValue(typeMetaData)),
						IsSereilizable(isSereilizable),

						TypeShema(MoveRValue(typeMetaDataRecord)),

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
					DefStructFieldData(Data, TypeName),
					DefStructFieldData(Data, CxxTypeName),
					DefStructFieldData(Data, TypeMetaData),
					DefStructFieldData(Data, IsSereilizable),
					DefStructFieldData(Data, TypeShema)
				);
			public:
				/// <summary>
				/// Конструктор з тегу типу, тип мусить підтримувати серелізацію
				/// </summary>
				template<typename T> //requires CTTI::Concepts::IsCttiSerelizable<T>
				DataTypeRecord(TypeTraits::TypeTag<T>) : data(new Data(
					CTTI::TypeName<TypeTraits::RemoveCVRefV<T>>(),
					CTTI::CXXTypeName<TypeTraits::RemoveCVRefV<T>>(),
					Serilization::MetaDataGen<TypeTraits::RemoveCVRefV<T>>(),
					CTTI::TypeTraits::CTTITypeMetaData<TypeTraits::RemoveCVRefV<T>>::IsSerilizable,

					Serilization::GetTypeRecordForType<TypeTraits::RemoveCVRefV<T>>(),

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

				/// <summary>
				/// Ім'я типу
				/// </summary>
				/// <returns>Вертає текстове представлення імені типу</returns>
				const String& GetTypeName() const {
					return data->TypeName;
				}

				/// <summary>
				/// Cxx ім'я типу
				/// </summary>
				/// <returns>Вертає текстове представлення декорованого імені типу</returns>
				const String& GetCxxTypeName() const {
					return data->CxxTypeName;
				}

				/// <summary>
				/// Схема типу
				/// </summary>
				/// <returns>Вертає текстове представлення схеми типу</returns>
				const String& GetTypeMetaData() const {
					return data->TypeMetaData;
				}

				/// <summary>
				/// Вказує чи тип серелізується
				/// </summary>
				/// <returns>Логічне значення</returns>
				bool GetIsSerelizableType() const {
					return data->IsSereilizable;
				}

				/// <summary>
				/// Схема типу
				/// </summary>
				/// <returns>Об'єкт схеми типу</returns>
				const TypeMetaDataRecord& GetTypeShema() const {
					return data->TypeShema;
				}

				/// <summary>
				/// Функція серелізації з Seed.Reflection.DynamicObject
				/// </summary>
				/// <returns>Покажчик на фунцію</returns>
				SerelizeFunctionType GetSerelizeFunction() const {
					return data->serelizeFunction;
				}

				/// <summary>
				/// Функція десерелізації з Seed::Core::Reflection::DynamicObject
				/// </summary>
				/// <returns>Покажчик на фунцію</returns>
				DeSerelizeFunctionType GetDeSerelizeFunction() const {
					return data->deSerelizeFunction;
				}

				/// <summary>
				/// Функція створення об'єкта як Seed::Core::Reflection::DynamicObject
				/// </summary>
				/// <returns>Покажчик на фунцію</returns>
				BuilderFunctionType GetBuilderFunction() const {
					return data->builderFunction;
				}

				/// <summary>
				/// Функція серелізації з Seed::Core::Containers::Dynamic
				/// </summary>
				/// <returns>Покажчик на фунцію</returns>
				SerelizeToDynamicFunctionType GetSerelizeToDynamicFunction() const {
					return data->serelizeToDynamicFunction;
				}

				/// <summary>
				/// Функція десерелізації з Seed::Core::Containers::Dynamic
				/// </summary>
				/// <returns>Покажчик на фунцію</returns>
				DeSerelizeToDynamicFunctionType GetDeSerelizeToDynamicFunction() const {
					return data->deSerelizeToDynamicFunction;
				}

				/// <summary>
				/// Функція створення об'єкта як Seed::Core::Containers::Dynamic
				/// </summary>
				/// <returns>Покажчик на фунцію</returns>
				BuilderToDynamicFunctionType GetBuilderToDynamicFunction() const {
					return data->builderToDynamicFunction;
				}

				/// <summary>
				/// Функція яка вказує розмір даних після серелізації з об'єкта Seed::Core::Reflection::DynamicObject
				/// </summary>
				/// <returns>Покажчик на фунцію</returns>
				DataSizeFunctionType GetDataSizeFunction() const {
					return data->dataSizeFunctionType;
				}

				/// <summary>
				/// Функція яка вказує розмір даних після серелізації з об'єкта Seed::Core::Containers::Dynamic
				/// </summary>
				/// <returns>Покажчик на фунцію</returns>
				DataSizeDynamicFunctionType GetDataSizeDynamicFunction() const {
					return data->dataSizeDynamicFunctionType;
				}
				
				/// <summary>
				/// Інформація про тип
				/// </summary>
				/// <returns>RTTITypeRecord</returns>
				const RTTI::RTTITypeRecord& GetTypeInfo() const {
					return data->typeInfo;
				}

			public:
				/// <summary>
				/// Ім'я типу
				/// </summary>
				String Property(GetTypeName) TypeName;

				/// <summary>
				/// Cxx ім'я типу
				/// </summary>
				String Property(GetCxxTypeName) CxxTypeName;

				/// <summary>
				/// Схема типу
				/// </summary>
				String Property(GetTypeMetaData) TypeMetaData;

				/// <summary>
				/// Вказує чи тип серелізується
				/// </summary>
				Boolean Property(GetIsSerelizableType) IsSerelizableType;

				/// <summary>
				/// Вказує чи тип серелізується
				/// </summary>
				TypeMetaDataRecord Property(GetTypeShema) TypeShema;

				/// <summary>
				/// Інформація про тип
				/// </summary>
				RTTI::RTTITypeRecord Property(GetTypeInfo) TypeRecord;

				/// <summary>
				/// Функція серелізації з Seed::Core::Reflection::DynamicObject
				/// </summary>
				SerelizeFunctionType Property(GetSerelizeFunction) SerelizeFunction;

				/// <summary>
				/// Функція десерелізації з Seed::Core::Reflection::DynamicObject
				/// </summary>
				DeSerelizeFunctionType Property(GetDeSerelizeFunction) DeSerelizeFunction;

				/// <summary>
				/// Функція створення об'єкта як Seed::Core::Reflection::DynamicObject
				/// </summary>
				BuilderFunctionType Property(GetBuilderFunction) BuilderFunction;


				/// <summary>
				/// Функція серелізації з Seed::Core::Containers::Dynamic
				/// </summary>
				SerelizeToDynamicFunctionType Property(GetSerelizeToDynamicFunction) SerelizeToDynamicFunction;

				/// <summary>
				/// Функція десерелізації з Seed::Core::Reflection::DynamicObject
				/// </summary>
				DeSerelizeToDynamicFunctionType Property(GetDeSerelizeToDynamicFunction) DeSerelizeToDynamicFunction;

				/// <summary>
				/// Функція створення об'єкта як Seed::Core::Containers::Dynamic
				/// </summary>
				BuilderToDynamicFunctionType Property(GetBuilderToDynamicFunction) BuilderToDynamicFunction;

				/// <summary>
				/// Функція яка вказує розмір даних після серелізації з об'єкта Seed::Core::Reflection::DynamicObject
				/// </summary>
				DataSizeFunctionType Property(GetDataSizeFunction) DataSizeFunction;

				/// <summary>
				/// Функція яка вказує розмір даних після серелізації з об'єкта Seed::Core::Containers::Dynamic
				/// </summary>
				DataSizeDynamicFunctionType Property(GetDataSizeDynamicFunction) DataSizeDynamicFunction;

				RefCountClass(DataTypeRecord, data);
			public:
				/// <summary>
				/// Оператор рівності з іншим об'єктом <see cref="DataTypeRecord"/>
				/// </summary>
				/// <param name="other">Лівий операнд</param>
				/// <returns>Результат порівняння</returns>
				bool operator==(const DataTypeRecord& other) const {
					return
						this->data->typeInfo == other.data->typeInfo;
				}

				/// <summary>
				/// Оператор рівності з іншим об'єктом <see cref="DataTypeRecord"/>
				/// </summary>
				/// <param name="other">Лівий операнд</param>
				/// <returns>Результат порівняння</returns>
				bool operator!=(const DataTypeRecord& other) const {
					return this->data->typeInfo != other.data->typeInfo;
				}

				bool operator==(const TypeInfo& other) const {
					return this->data->typeInfo == other;
				}

				bool operator!=(const TypeInfo& other) const {
					return this->data->typeInfo != other;
				}

				auto operator<=>(const TypeInfo& other) const {
					return this->data->typeInfo <=> other;
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
					return other <=> item.data->typeInfo;
				}
			};
		}
	}
}