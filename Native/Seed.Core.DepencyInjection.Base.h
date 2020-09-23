#pragma once
#include "Seed.Core.RTTI.h"

#include "Seed.Core.Containers.h"

#include "Seed.Core.Reflection.h"

namespace Seed
{
	inline namespace Core
	{
		namespace DI
		{
			///<summary>Не можливо розвязати залежності типів для побудови класу</summary>
			class DIResolveException : public Exception {
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
				DIResolveException(const DIResolveException& orig) : Exception(orig), type(orig.type), typeName(orig.typeName) {}
				DIResolveException(DIResolveException&& from) : Exception(MoveRValue(from)), type(from.type), typeName(MoveRValue(from.typeName)) {}
				DIResolveException& operator=(const DIResolveException& from) { this->~DIResolveException(); this->DIResolveException::DIResolveException(from); return *this; }
				DIResolveException& operator=(DIResolveException&& from) { this->~DIResolveException(); this->DIResolveException::DIResolveException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param Name = 'Type'>Тип який DI не зумів знайти</param>
				DIResolveException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ExceptionString&& typeName, const TypeInfo& type)
					:Exception(File, FunctName, Line, L"Не можливо розвязати залежності типів для побудови класу"), type(type), typeName(MoveRValue(typeName)) {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Ім'я типу: "), typeName, ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new DIResolveException(*this));
				}

				virtual ~DIResolveException() {}
			};

			///<summary>Помилка реєстрації сервісу</summary>
			class DIRegisterService : public Exception {
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
				DIRegisterService(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ExceptionString&& typeName, const TypeInfo& type)
					:Exception(File, FunctName, Line, L"Помилка реєстрації сервісу"), type(type), typeName(MoveRValue(typeName)) {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Ім'я сервісу: "), typeName, ExceptionString(L". "),
						location.toString(), ExceptionString(L". ")
					});
				}

				virtual ~DIRegisterService() {}
			};

			using NonSerelizableType = Reflection::NonSerelizableType;

			using DynamicObject = Reflection::DynamicObject;

			class DepencyInjector;

			/// <summary>
			/// Класс постачальника залежностей
			/// </summary>
			class DepencyInjector final : IDisposable {
			private:
				using TResolverCtor = void* (*)(DepencyInjector & injector);
				using TServiceDtor = void(*)(void*);
				using TInterfaceCaster = void* (*)(void*);
			private:
				struct ServiceRecord {
					TResolverCtor resolverCtor;
					TInterfaceCaster interfaceCaster;
					TServiceDtor serviceDtor;
					Reflection::ObjectRTTIRecord rttiRecord;
					Containers::List<RTTI::RTTITypeRecord> depencies;
					void* objPtr;
					

					ServiceRecord(
						TResolverCtor resolverCtor, 
						TServiceDtor serviceDtor,
						TInterfaceCaster interfaceCaster, 
						Reflection::ObjectRTTIRecord && rttiRecord,
						Containers::List<RTTI::RTTITypeRecord>&& depencies
						) 
					:
						resolverCtor(resolverCtor), 
						serviceDtor(serviceDtor),
						interfaceCaster(interfaceCaster), 
						rttiRecord(MoveRValue(rttiRecord)), 
						depencies(MoveRValue(depencies)),
						objPtr(nullptr)
					{}

					~ServiceRecord() {
						if (objPtr != nullptr)
						{
							serviceDtor(objPtr);
							objPtr = nullptr;
						}
					}
				};
			private:
				struct Data {
					Containers::ConcurencyMap<RTTI::RTTITypeRecord, SmartPtr<ServiceRecord>>		serviceTableForNativeLevel;
					Containers::ConcurencyDictionary<SmartPtr<ServiceRecord>>						serviceTableForManagedLevel;
					Containers::ConcurencyList<SmartPtr<ServiceRecord>>								serviceStore;
					Containers::ConcurencyMap<RTTI::RTTITypeRecord, Reflection::DataTypeRecord>		dataTypeTableForNativeLevel;
					Containers::ConcurencyDictionary<Reflection::DataTypeRecord>					dataTypeTableForManagedLevel;
					Containers::ConcurencyMap<RTTI::RTTITypeRecord, Reflection::ObjectRTTIRecord>	objectTypeTableForNativeLevel;
					Containers::ConcurencyDictionary<Reflection::ObjectRTTIRecord>					objectTypeTableForManagedLevel;

					Serilization::TypesRegistry														dataTypesListForSerilization;

					Containers::ConcurencyMap<RTTI::RTTITypeRecord, SizeT>							dataTypeRecordRefs;
					Containers::ConcurencyMap<RTTI::RTTITypeRecord, SizeT>							objectTypeRecordRefs;

					RefCountT																		RefCount;

					Data() :
						RefCount(1),
						serviceTableForNativeLevel(),
						serviceTableForManagedLevel(),
						dataTypeTableForNativeLevel(),
						dataTypeTableForManagedLevel(),
						objectTypeTableForNativeLevel(),
						objectTypeTableForManagedLevel(),
						dataTypesListForSerilization(),
						dataTypeRecordRefs(),
						objectTypeRecordRefs()
					{}
				};
			private:
				Data* data;
			private:
				template<typename T>
				ForceInline void RegisterType() {
					auto typeRecord = Reflection::DataTypeRecord(Seed::Core::TypeTraits::TypeTag<T>{});

					data->dataTypeTableForNativeLevel.Insert(TypeRecord<T>(), typeRecord);
					data->dataTypeTableForManagedLevel.Insert(CTTI::TypeName<T>(), typeRecord);
					data->dataTypesListForSerilization.AddLast({ TypeRecord<T>(), typeRecord});
					data->dataTypeRecordRefs.Insert(TypeRecord<T>(), 1);
				}

				void RegisterServiceTypes(const SmartPtr<ServiceRecord>& service);

				void UnRegisterServiceByTypeRecord(const RTTI::RTTITypeRecord& serviceRecord);

				void UnregisterServiceTypes(const SmartPtr<ServiceRecord>& service);
			protected:
				virtual void Dispose() override;
			public:
				/// <summary>
				/// Клас сервісу постачальника залежностей
				/// </summary>
				class IDI final {
				private:
					DepencyInjector& di;
				private:
					IDI(DepencyInjector& di) : di(di) {}
				public:
					IDI(const IDI& idi) : di(idi.di) {}
					IDI(IDI&& idi) : di(idi.di) {}
					IDI& operator=(const IDI& idi) {
						this->~IDI();
						this->IDI::IDI(idi);
						return *this;
					}
					IDI& operator=(IDI&& idi) {
						this->~IDI();
						this->IDI::IDI(MoveRValue(idi));
						return *this;
					}
					~IDI() {}
				public:
#pragma region Getters
					ForceInline Containers::List<String> GetAllServicesNames() const {
						return di.GetAllServicesNames();
					}

					ForceInline Reflection::ObjectRTTIRecord GetServiceRecordByName(const String& name) const {
						return di.GetServiceRecordByName(name);
					}

					ForceInline Containers::List<String> GetAllRegisteredObjects() const {
						return di.GetAllRegisteredObjects();
					}

					ForceInline Reflection::ObjectRTTIRecord GetObjectRecordByName(const String& name) const {
						return di.GetObjectRecordByName(name);
					}

					ForceInline Reflection::ObjectRTTIRecord GetObjectRecordByType(const RTTI::RTTITypeRecord& id) const {
						return di.GetObjectRecordByType(id);
					}

					ForceInline Boolean IsExistObjectRecordByName(const String& name) const {
						return di.IsExistObjectRecordByName(name);
					}

					ForceInline Boolean IsExistObjectRecordByType(const RTTI::RTTITypeRecord& id) const {
						return di.IsExistObjectRecordByType(id);
					}

					ForceInline Containers::List<String> GetAllRegisteredDataTypes() const {
						return di.GetAllRegisteredDataTypes();
					}

					ForceInline Reflection::DataTypeRecord GetDataTypesRecordByName(const String& name) const {
						return di.GetDataTypesRecordByName(name);
					}

					ForceInline Reflection::DataTypeRecord GetDataTypesRecordByType(const RTTI::RTTITypeRecord& id) const {
						return di.GetDataTypesRecordByType(id);
					}

					ForceInline Boolean IsExistDataTypesRecordByName(const String& name) const {
						return di.IsExistDataTypesRecordByName(name);
					}

					ForceInline Boolean IsExistDataTypesRecordByType(const RTTI::RTTITypeRecord& type) const {
						return di.IsExistDataTypesRecordByType(type);
					}

					ForceInline void RegisterObjectRTTIRecord(const Reflection::ObjectRTTIRecord& objectRTTIRecord) const {
						di.RegisterObjectType(objectRTTIRecord);
					}

					ForceInline void RegisterObjectRTTIRecord(Reflection::ObjectRTTIRecord&& objectRTTIRecord) const {
						di.RegisterObjectType(MoveRValue(objectRTTIRecord));
					}

					ForceInline void RemoveObjectRTTIRecord(const Reflection::ObjectRTTIRecord& objectRTTIRecord) const {
						di.RemoveObjectType(objectRTTIRecord);
					}

					ForceInline void RegisterDataTypeRecord(const Reflection::DataTypeRecord& objectRTTIRecord) const {
						di.RegisterDataType(objectRTTIRecord);
					}

					ForceInline void RegisterDataTypeRecord(Reflection::DataTypeRecord&& objectRTTIRecord) const {
						di.RegisterDataType(MoveRValue(objectRTTIRecord));
					}

					ForceInline void RemoveDataTypeRecord(const Reflection::DataTypeRecord& objectRTTIRecord) const {
						di.RemoveDataType(objectRTTIRecord);
					}

					ForceInline void RemoveDataTypeRecord(Reflection::DataTypeRecord&& objectRTTIRecord) const {
						di.RemoveDataType(objectRTTIRecord);
					}

					ForceInline const Serilization::TypesRegistry& GetTypesRegistry() {
						return di.data->dataTypesListForSerilization;
					}
#pragma endregion
				public:
					/// <summary>
					/// Вертає сервіс за типом
					/// </summary>
					template<typename TService>
					TService Resolve() {
						return di.Resolve<TService>();
					}

					/// <summary>
					/// Реєстрація сервісу
					/// </summary>
					/// <param name="rttiRecord">Мета інформація про сервіс</param>
					template<typename TService, typename ... TCtorArgs>
					requires Concepts::IsConstructibleWith<TService, TCtorArgs...>
					IDI& Register(Reflection::ObjectRTTIRecord&& rttiRecord) {
						di.Register<TService, TCtorArgs...>(MoveRValue(rttiRecord));
						return *this;
					}

					/// <summary>
					/// Реєстрація сервісу через інтферфейс
					/// </summary>
					/// <param name="rttiRecord">Мета інформація про сервіс</param>
					template<typename TServiceInterface, typename TService, typename ... TCtorArgs>
					requires Concepts::IsConstructibleWith<TService, TCtorArgs...>
					IDI& RegisterInterface(Reflection::ObjectRTTIRecord&& rttiRecord) {
						di.RegisterInterface<TServiceInterface, TService, TCtorArgs...>(MoveRValue(rttiRecord));
						return *this;
					}
				private:
					friend class DepencyInjector;
				};
			public:
				/// <summary>
				/// Конструктор
				/// </summary>
				DepencyInjector();

				/// <summary>
				/// Вертає сервіс за типом
				/// </summary>
				template<typename TService>
				TService Resolve() {
					if constexpr (TypeTraits::IsSameV<TService, IDI>)
					{
						return IDI(*this);
					}
					else
					{
						auto type = TypeRecord<TService>();
						if (data->serviceTableForNativeLevel.IsExist(type))
						{
							auto service = data->serviceTableForNativeLevel[type];
							if (service->objPtr == nullptr)
							{
								service->objPtr =
									service->resolverCtor(*this);
							}

							if (service->interfaceCaster != nullptr)
							{
								return *reinterpret_cast<TService*>(service
									->interfaceCaster(service->objPtr));
							}
							else
							{
								return *reinterpret_cast<TService*>(service->objPtr);
							}
						}

						Throw(DIResolveException(ExceptionPosition, CTTI::TypeName<TService>(), typeid(TService)));
					}
				};

				/// <summary>
				/// Реєстрація сервісу
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про сервіс</param>
				template<typename TService, typename ... TCtorArgs>
				requires Concepts::IsConstructibleWith<TService, TCtorArgs...>
				DepencyInjector& Register(Reflection::ObjectRTTIRecord&& rttiRecord) {
					static_assert(TypeTraits::IsConstructible<TService, TCtorArgs...>, "Cannot construct service with that types");

					static constexpr auto InjectorCtor = [] (DepencyInjector& injector) -> void*
					{
						if constexpr (Reflection::Details::IsFunctionWithoutArgs<TCtorArgs...>)
						{
							return new TService();
						}
						else
						{
							return Containers::Tuple<TCtorArgs...>{injector.Resolve<TCtorArgs>() ... }
								.CallWithArgsFromTuple([] (TCtorArgs&& ... args) InlineLambda
								{
									return new TService(args...);
								});
						}
					};

					static constexpr auto ServiceDtor = [] (void* pServiceObj)->void
					{
						delete (TService*) pServiceObj;
					};

					if (rttiRecord.GetRTTITypeInfo().TypeId() != typeid(TService))
						Throw(DIRegisterService(ExceptionPosition, CTTI::TypeName<TService>(), typeid(TService)));

					auto serviceRecord = SmartPtr<ServiceRecord>{ new ServiceRecord(
						InjectorCtor,
						ServiceDtor,
						nullptr,
						MoveRValue(rttiRecord),
						{ RTTI::RTTITypeRecord(TypeTraits::TypeTag<TCtorArgs>{}) ... }
					) };

					this->data->serviceStore += serviceRecord;

					this->data->serviceTableForNativeLevel.Insert(TypeRecord<TService>(), serviceRecord);
					this->data->serviceTableForManagedLevel.Insert(serviceRecord->rttiRecord.ClassName, serviceRecord);

					RegisterServiceTypes(serviceRecord);

					return *this;
				}

				/// <summary>
				/// Реєстрація сервісу через інтферфейс
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про сервіс</param>
				template<typename TServiceInterface, typename TService, typename ... TCtorArgs>
				requires Concepts::IsConstructibleWith<TService, TCtorArgs...>
				DepencyInjector& RegisterInterface(Reflection::ObjectRTTIRecord&& rttiRecord) {
					static_assert(TypeTraits::IsConstructible<TService, TCtorArgs...>, "Cannot construct service with that types");
					static_assert(TypeTraits::IsBasedOnV<TServiceInterface, TService>, "Cannot register service with invalid interface");

					static constexpr auto InjectorCtor = [] (DepencyInjector& injector) -> void*
					{
						if constexpr (Reflection::Details::IsFunctionWithoutArgs<TCtorArgs...>)
						{
							return new TService();
						}
						else
						{
							return Containers::Tuple<TCtorArgs...>{injector.Resolve<TCtorArgs>() ... }
								.CallWithArgsFromTuple([] (TCtorArgs&& ... args) InlineLambda
								{
									return new TService(args...);
								});
						}
					};

					static constexpr auto InterfaceGetter = [] (void* obj) -> void*
					{
						return static_cast<TServiceInterface*>(reinterpret_cast<TService*>(obj));
					}

					if (rttiRecord.GetRTTITypeInfo().TypeId() != typeid(TService))
						Throw(DIRegisterService(ExceptionPosition, CTTI::TypeName<TService>(), typeid(TService)));

					auto serviceRecord = SmartPtr<ServiceRecord>{ new ServiceRecord(
						InjectorCtor,
						InterfaceGetter,
						MoveRValue(rttiRecord),
						{ RTTI::RTTITypeRecord(TypeTraits::TypeTag<TCtorArgs>{}) ... }
					) };

					RegisterServiceTypes(serviceRecord);

					return *this;
				}

				/// <summary>
				/// Дереєстрація сервісу
				/// </summary>
				template<typename TService>
				DepencyInjector& UnRegisterService() {
					UnRegisterServiceByTypeRecord(TypeRecord<TService>());
					return *this;
				}

				/// <summary>
				/// Реєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RegisterObjectType(Reflection::ObjectRTTIRecord&& rttiRecord) {
					
					if (this->data->objectTypeRecordRefs.IsExist(rttiRecord.GetRTTITypeInfo()))
					{
						this->data->objectTypeRecordRefs[rttiRecord.GetRTTITypeInfo()] += 1;
					}
					else
					{
						this->data->objectTypeTableForNativeLevel.Insert(rttiRecord.GetRTTITypeInfo(), rttiRecord);
						this->data->objectTypeTableForManagedLevel.Insert(rttiRecord.ClassName, rttiRecord);
						this->data->objectTypeRecordRefs.Insert({ rttiRecord.GetRTTITypeInfo(), 1 });
					}

					return *this;
				}

				/// <summary>
				/// Реєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RegisterObjectType(const Reflection::ObjectRTTIRecord& rttiRecord) {
					
					if (this->data->objectTypeRecordRefs.IsExist(rttiRecord.GetRTTITypeInfo()))
					{
						this->data->objectTypeRecordRefs[rttiRecord.GetRTTITypeInfo()] += 1;
					}
					else
					{
						this->data->objectTypeTableForNativeLevel.Insert(rttiRecord.GetRTTITypeInfo(), rttiRecord);
						this->data->objectTypeTableForManagedLevel.Insert(rttiRecord.ClassName, rttiRecord);
						this->data->objectTypeRecordRefs.Insert({ rttiRecord.GetRTTITypeInfo(), 1 });
					}

					return *this;
				}

				/// <summary>
				/// Дереєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RemoveObjectType(const Reflection::ObjectRTTIRecord& rttiRecord) {
					if (this->data->objectTypeRecordRefs.IsExist(rttiRecord.GetRTTITypeInfo()))
					{
						if ((--this->data->objectTypeRecordRefs[rttiRecord.GetRTTITypeInfo()]) == 0)
						{
							this->data->objectTypeTableForNativeLevel.Delete(rttiRecord.GetRTTITypeInfo());
							this->data->objectTypeTableForManagedLevel.Remove(rttiRecord.ClassName);
							this->data->objectTypeRecordRefs.Delete(rttiRecord.GetRTTITypeInfo());
						}
					}

					return *this;
				}

				/// <summary>
				/// Вертає список імен всіх сервісів
				/// </summary>
				/// <returns></returns>
				ForceInline Containers::List<String> GetAllServicesNames() {
					Containers::List<String> servicesNames;
					data->serviceTableForManagedLevel.ForEachKey([&] (const String& str)  mutable InlineLambda
					{
						servicesNames += str;
					});
					return servicesNames;
				}

				/// <summary>
				/// Вертає метаінформацію про сервіс по його імені
				/// </summary>
				/// <param name="name"></param>
				/// <returns></returns>
				ForceInline Reflection::ObjectRTTIRecord GetServiceRecordByName(const String& name) {
					return data->serviceTableForManagedLevel[name]->rttiRecord;
				}

				/// <summary>
				/// Вертає список всіх зареєстрованих об'єктів
				/// </summary>
				/// <returns>Список зареєстрованих об'єкті</returns>
				ForceInline Containers::List<String> GetAllRegisteredObjects() {
					Containers::List<String> objectNames;
					data->objectTypeTableForManagedLevel.ForEachKey([&] (const String& str) mutable InlineLambda
					{
						objectNames += str;
					});
					return objectNames;
				}
				/// <summary>
				/// Вертає мета інформацію про об'єкт за іменем типу
				/// </summary>
				/// <param name="name">Ім'я типу</param>
				/// <returns>Мета інформація про об'єкт</returns>
				ForceInline Reflection::ObjectRTTIRecord GetObjectRecordByName(const String& name) {
					return data->objectTypeTableForManagedLevel[name];
				}
				/// <summary>
				/// Вертає мета інформацію про об'єкт за записом про тип
				/// </summary>
				/// <param name="id">Запис про тип</param>
				/// <returns>Мета інформація про об'єкт</returns>
				ForceInline Reflection::ObjectRTTIRecord GetObjectRecordByType(const RTTI::RTTITypeRecord& id) {
					return data->objectTypeTableForNativeLevel[id];
				}

				/// <summary>
				/// Перевіряє чи мета інформація про тип за іменем типу
				/// </summary>
				/// <param name="name">Ім'я типу</param>
				/// <returns>true якщо об'єкт типу зареєтрований</returns>
				ForceInline Boolean IsExistObjectRecordByName(const String& name) {
					return data->objectTypeTableForManagedLevel.IsExist(name);
				}

				/// <summary>
				/// Перевіряє чи мета інформація про тип за записом про тип
				/// </summary>
				/// <param name="name">Запис про тип</param>
				/// <returns>true якщо об'єкт типу зареєтрований</returns>
				ForceInline Boolean IsExistObjectRecordByType(const RTTI::RTTITypeRecord& id) {
					return data->objectTypeTableForNativeLevel.IsExist(id);
				}

				/// <summary>
				/// Вертає список зареєстрованих типів даних
				/// </summary>
				/// <returns>Список імен зареєстрованиї типів даних</returns>
				ForceInline Containers::List<String> GetAllRegisteredDataTypes() {
					Containers::List<String> dataTypeNames;
					data->dataTypeTableForManagedLevel.ForEachKey([&] (const String& str) mutable InlineLambda
					{
						dataTypeNames += str;
					});
					return dataTypeNames;
				}

				/// <summary>
				/// Вертає мета інформація про тип даних за його іменем
				/// </summary>
				/// <param name="name">Ім'я типу даних</param>
				/// <returns>Мета інформація про тип даних</returns>
				ForceInline Reflection::DataTypeRecord GetDataTypesRecordByName(const String& name) {
					return data->dataTypeTableForManagedLevel[name];
				}

				/// <summary>
				/// Вертає мета інформація про тип даних за записом про тип
				/// </summary>
				/// <param name="name">Запис про типх</param>
				/// <returns>Мета інформація про тип даних</returns>
				ForceInline Reflection::DataTypeRecord GetDataTypesRecordByType(const RTTI::RTTITypeRecord& id) {
					return data->dataTypeTableForNativeLevel[id];
				}

				/// <summary>
				/// Перевіряє чи мета інформація про тип даних за іменем типу
				/// </summary>
				/// <param name="name">Ім'я типу</param>
				/// <returns>true якщо тип даних зареєтрований</returns>
				ForceInline Boolean IsExistDataTypesRecordByName(const String& name) {
					return data->dataTypeTableForManagedLevel.IsExist(name);
				}

				/// <summary>
				/// Перевіряє чи мета інформація про тип даних за записом про тип
				/// </summary>
				/// <param name="name">Запис про тип</param>
				/// <returns>true якщо тип даних зареєтрований</returns>
				ForceInline Boolean IsExistDataTypesRecordByType(const RTTI::RTTITypeRecord& id) {
					return data->dataTypeTableForNativeLevel.IsExist(id);
				}

				/// <summary>
				/// Реєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RegisterDataType(Reflection::DataTypeRecord&& rttiRecord) {
					
					if (this->data->dataTypeRecordRefs.IsExist(rttiRecord.TypeRecord))
					{
						this->data->dataTypeRecordRefs[rttiRecord.TypeRecord] += 1;
					}
					else
					{
						this->data->dataTypeTableForNativeLevel.Insert(rttiRecord.TypeRecord, rttiRecord);
						this->data->dataTypeTableForManagedLevel.Insert(rttiRecord.TypeName, rttiRecord);
						this->data->dataTypeRecordRefs.Insert({ rttiRecord.TypeRecord, 1 });
						this->data->dataTypesListForSerilization.AddLast({ rttiRecord.TypeRecord, rttiRecord });
					}

					return *this;
				}

				/// <summary>
				/// Реєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RegisterDataType(const Reflection::DataTypeRecord& rttiRecord) {
					
					if (this->data->dataTypeRecordRefs.IsExist(rttiRecord.TypeRecord))
					{
						this->data->dataTypeRecordRefs[rttiRecord.TypeRecord] += 1;
					}
					else
					{
						this->data->dataTypeTableForNativeLevel.Insert(rttiRecord.TypeRecord, rttiRecord);
						this->data->dataTypeTableForManagedLevel.Insert(rttiRecord.TypeName, rttiRecord);
						this->data->dataTypeRecordRefs.Insert({ rttiRecord.TypeRecord, 1 });
						this->data->dataTypesListForSerilization.AddLast({ rttiRecord.TypeRecord, rttiRecord });
					}

					return *this;
				}

				/// <summary>
				/// Дереєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RemoveDataType(const Reflection::DataTypeRecord& rttiRecord) {
					if (this->data->dataTypeRecordRefs.IsExist(rttiRecord.TypeRecord))
					{
						if ((--this->data->dataTypeRecordRefs[rttiRecord.TypeRecord]) == 0)
						{
							this->data->dataTypeTableForNativeLevel.Delete(rttiRecord.TypeRecord);
							this->data->dataTypeTableForManagedLevel.Remove(rttiRecord.TypeName);
							this->data->dataTypeRecordRefs.Delete(rttiRecord.TypeRecord);
							this->data->dataTypesListForSerilization.RemoveIf([&](const auto& item){ return item.Key == rttiRecord.TypeRecord; });
						}
					}

					return *this;
				}
			public:
				RefCountClass(DepencyInjector, data);
			};

			using IDI = DepencyInjector::IDI;
		}
	}
}