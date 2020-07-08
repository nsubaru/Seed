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
						location.toString(), ExceptionString(L". ")
					});
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
					Containers::ConcurencyMap<RTTI::RTTITypeRecord, SmartPtr<ServiceRecord>> serviceTableForNativeLevel;
					Containers::ConcurencyDictionary<SmartPtr<ServiceRecord>> serviceTableForManagedLevel;
					Containers::ConcurencyList<SmartPtr<ServiceRecord>> serviceStore;
					Containers::ConcurencyMap<RTTI::RTTITypeRecord, Reflection::DataTypeRecord> dataTypeTableForNativeLevel;
					Containers::ConcurencyDictionary<Reflection::DataTypeRecord> dataTypeTableForManagedLevel;
					Containers::ConcurencyMap<RTTI::RTTITypeRecord, Reflection::ObjectRTTIRecord> objectTypeTableForNativeLevel;
					Containers::ConcurencyDictionary<Reflection::ObjectRTTIRecord> objectTypeTableForManagedLevel;

					RefCountT RefCount;

					Data() :
						RefCount(1),
						serviceTableForNativeLevel(),
						serviceTableForManagedLevel(),
						dataTypeTableForNativeLevel(),
						dataTypeTableForManagedLevel(),
						objectTypeTableForNativeLevel(),
						objectTypeTableForManagedLevel()
					{}
				};
			private:
				Data* data;
			protected:
				virtual void Dispose() override {
					//Видалення сервісів з реєстрів
					data->serviceTableForManagedLevel.RemoveAll();
					data->serviceTableForNativeLevel.RemoveAll();
					//Видалення сервісів
					data->serviceStore.RemoveAll();
					//Видалення записів про типи даних
					data->dataTypeTableForManagedLevel.RemoveAll();
					data->dataTypeTableForNativeLevel.RemoveAll();
					//Видалення записів про типи об'єктів
					data->objectTypeTableForManagedLevel.RemoveAll();
					data->objectTypeTableForNativeLevel.RemoveAll();
				};
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
					forceinline Containers::List<String> GetAllServicesNames() const {
						return di.GetAllServicesNames();
					}

					forceinline Reflection::ObjectRTTIRecord GetServiceRecordByName(const String& name) const {
						return di.GetServiceRecordByName(name);
					}

					forceinline Containers::List<String> GetAllRegisteredObjects() const {
						return di.GetAllRegisteredObjects();
					}

					forceinline Reflection::ObjectRTTIRecord GetObjectRecordByName(const String& name) const {
						return di.GetObjectRecordByName(name);
					}

					forceinline Reflection::ObjectRTTIRecord GetObjectRecordByType(const RTTI::RTTITypeRecord& id) const {
						return di.GetObjectRecordByType(id);
					}

					forceinline Boolean IsExistObjectRecordByName(const String& name) const {
						return di.IsExistObjectRecordByName(name);
					}

					forceinline Boolean IsExistObjectRecordByType(const RTTI::RTTITypeRecord& id) const {
						return di.IsExistObjectRecordByType(id);
					}

					forceinline Containers::List<String> GetAllRegisteredDataTypes() const {
						return di.GetAllRegisteredDataTypes();
					}

					forceinline Reflection::DataTypeRecord GetDataTypesRecordByName(const String& name) const {
						return di.GetDataTypesRecordByName(name);
					}

					forceinline Reflection::DataTypeRecord GetDataTypesRecordByType(const RTTI::RTTITypeRecord& id) const {
						return di.GetDataTypesRecordByType(id);
					}

					forceinline Boolean IsExistDataTypesRecordByName(const String& name) const {
						return di.IsExistDataTypesRecordByName(name);
					}

					forceinline Boolean IsExistDataTypesRecordByType(const RTTI::RTTITypeRecord& type) const {
						return di.IsExistDataTypesRecordByType(type);
					}

					forceinline void RegisterObjectRTTIRecord(const Reflection::ObjectRTTIRecord& objectRTTIRecord) const {
						di.RegisterObjectType(objectRTTIRecord);
					}

					forceinline void RegisterObjectRTTIRecord(Reflection::ObjectRTTIRecord&& objectRTTIRecord) const {
						di.RegisterObjectType(MoveRValue(objectRTTIRecord));
					}

					forceinline void RemoveObjectRTTIRecord(const Reflection::ObjectRTTIRecord& objectRTTIRecord) const {
						di.RemoveObjectType(objectRTTIRecord);
					}
				private:
					friend class DepencyInjector;
				};
			public:
				/// <summary>
				/// Конструктор
				/// </summary>
				DepencyInjector() : data(new Data()) {}

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
							.CallWithArgsFromTuple([] (TCtorArgs&& ... args)
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

					serviceRecord->rttiRecord.Methods.ForEach([&] (const Reflection::ObjectRTTIRecord::MethodRTTIRecord& methodRecord)
					{
						methodRecord.TypeInformationGetterFunction().ForEach([&](const Reflection::DataTypeRecord & typeRecord) {
							if (this->data->dataTypeTableForNativeLevel.IsExist(typeRecord.GetTypeInfo()))
								return;
							if (this->data->objectTypeTableForNativeLevel.IsExist(typeRecord.GetTypeInfo()))
								return;

							this->data->dataTypeTableForNativeLevel.Insert(typeRecord.GetTypeInfo(), typeRecord);
							this->data->dataTypeTableForManagedLevel.Insert(typeRecord.TypeName, typeRecord);
						});
					});

					return *this;
				}

				/// <summary>
				/// Реєстрація сервісу через інтферфейс
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про сервіс</param>
				template<typename TServiceInterface, typename TService, typename ... TCtorArgs>
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
								.CallWithArgsFromTuple([] (TCtorArgs&& ... args)
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

					this->data->serviceTableForNativeLevel.Insert(TypeRecord<TServiceInterface>(), serviceRecord);
					this->data->serviceTableForManagedLevel.Insert(CTTI::TypeName<TServiceInterface>(), serviceRecord);

					serviceRecord->rttiRecord.Methods.ForEach([&] (const Reflection::ObjectRTTIRecord::MethodRTTIRecord& methodRecord)
					{
						methodRecord.TypeInformationGetterFunction().ForEach([&](const Reflection::DataTypeRecord & typeRecord) {
							if (this->data->dataTypeTableForNativeLevel.IsExist(typeRecord.GetTypeInfo()))
								return;
							if (this->data->objectTypeTableForNativeLevel.IsExist(typeRecord.GetTypeInfo()))
								return;

							this->data->dataTypeTableForNativeLevel.Insert(typeRecord.GetTypeInfo(), typeRecord);
							this->data->dataTypeTableForManagedLevel.Insert(typeRecord.TypeName, typeRecord);
						});
					});

					return *this;
				}

				/// <summary>
				/// Дереєстрація сервісу
				/// </summary>
				template<typename TService>
				DepencyInjector& UnRegisterService() {
					if (data->serviceTableForNativeLevel.IsExist(TypeRecord<TService>()))
					{
						auto service = data->serviceTableForNativeLevel[TypeRecord<TService>()];

						auto dependents = Containers::List<decltype(service)>();
						
						dependents += service;
						auto currentDependentsCount = 0Sz;
						do
						{
							currentDependentsCount = dependents.Count;

							auto newDependents = Containers::List<decltype(service)>();
							
							for (auto& service : dependents)
							{
								data->serviceStore.ForEach([&] (const SmartPtr<ServiceRecord>& record)
								{
									record->depencies.ForEach([&] (const RTTI::RTTITypeRecord & info)
									{
										if (info == service->rttiRecord.GetRTTITypeInfo())
											newDependents += record;
									});
								});
							}

							dependents += MoveRValue(newDependents);

						} while (currentDependentsCount != dependents.Count);

						dependents.RemoveAll([&] (SmartPtr<ServiceRecord>&& serviceRecord)
						{
							//Видалення сервісів з реєстрів
							data->serviceTableForManagedLevel.Remove(serviceRecord->rttiRecord.ClassName);
							data->serviceTableForNativeLevel.Delete(serviceRecord->rttiRecord.GetRTTITypeInfo());
							//Видалення сервісів
							data->serviceStore.RemoveIf([&] (const SmartPtr<ServiceRecord>& item)
							{
								return item->rttiRecord.GetRTTITypeInfo() == serviceRecord->rttiRecord.GetRTTITypeInfo();
							});
							//TODO: Remove method type on unregister services
						});
					}
					else
					{
						Throw(DIResolveException(ExceptionPosition, CTTI::TypeName<TService>(), typeid(TService)));
					}

					return *this;
				}

				/// <summary>
				/// Реєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RegisterObjectType(Reflection::ObjectRTTIRecord&& rttiRecord) {
					
					if (!this->data->objectTypeTableForNativeLevel.IsExist(rttiRecord.GetRTTITypeInfo()))
					{
						this->data->objectTypeTableForNativeLevel.Insert(rttiRecord.GetRTTITypeInfo(), rttiRecord);
						this->data->objectTypeTableForManagedLevel.Insert(rttiRecord.ClassName, MoveRValue(rttiRecord));
					}

					return *this;
				}

				/// <summary>
				/// Реєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RegisterObjectType(const Reflection::ObjectRTTIRecord& rttiRecord) {
					
					if (!this->data->objectTypeTableForNativeLevel.IsExist(rttiRecord.GetRTTITypeInfo()))
					{
						this->data->objectTypeTableForNativeLevel.Insert(rttiRecord.GetRTTITypeInfo(), rttiRecord);
						this->data->objectTypeTableForManagedLevel.Insert(rttiRecord.ClassName, rttiRecord);
					}

					return *this;
				}

				/// <summary>
				/// Дереєстрація об'єкта
				/// </summary>
				/// <param name="rttiRecord">Мета інформація про об'єкт</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				DepencyInjector& RemoveObjectType(const Reflection::ObjectRTTIRecord& rttiRecord) {
					if (this->data->objectTypeTableForNativeLevel.IsExist(rttiRecord.GetRTTITypeInfo())) {
						this->data->objectTypeTableForNativeLevel.Delete(rttiRecord.GetRTTITypeInfo());
						this->data->objectTypeTableForManagedLevel.Remove(rttiRecord.ClassName);
					}

					return *this;
				}

				/// <summary>
				/// Вертає список імен всіх сервісів
				/// </summary>
				/// <returns></returns>
				forceinline Containers::List<String> GetAllServicesNames() {
					Containers::List<String> servicesNames;
					data->serviceTableForManagedLevel.ForEachKey([&] (const String& str) mutable
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
				forceinline Reflection::ObjectRTTIRecord GetServiceRecordByName(const String& name) {
					return data->serviceTableForManagedLevel[name]->rttiRecord;
				}

				/// <summary>
				/// Вертає список всіх зареєстрованих об'єктів
				/// </summary>
				/// <returns>Список зареєстрованих об'єкті</returns>
				forceinline Containers::List<String> GetAllRegisteredObjects() {
					Containers::List<String> objectNames;
					data->objectTypeTableForManagedLevel.ForEachKey([&] (const String& str) mutable
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
				forceinline Reflection::ObjectRTTIRecord GetObjectRecordByName(const String& name) {
					return data->objectTypeTableForManagedLevel[name];
				}
				/// <summary>
				/// Вертає мета інформацію про об'єкт за записом про тип
				/// </summary>
				/// <param name="id">Запис про тип</param>
				/// <returns>Мета інформація про об'єкт</returns>
				forceinline Reflection::ObjectRTTIRecord GetObjectRecordByType(const RTTI::RTTITypeRecord& id) {
					return data->objectTypeTableForNativeLevel[id];
				}

				/// <summary>
				/// Перевіряє чи мета інформація про тип за іменем типу
				/// </summary>
				/// <param name="name">Ім'я типу</param>
				/// <returns>true якщо об'єкт типу зареєтрований</returns>
				forceinline Boolean IsExistObjectRecordByName(const String& name) {
					return data->objectTypeTableForManagedLevel.IsExist(name);
				}

				/// <summary>
				/// Перевіряє чи мета інформація про тип за записом про тип
				/// </summary>
				/// <param name="name">Запис про тип</param>
				/// <returns>true якщо об'єкт типу зареєтрований</returns>
				forceinline Boolean IsExistObjectRecordByType(const RTTI::RTTITypeRecord& id) {
					return data->objectTypeTableForNativeLevel.IsExist(id);
				}

				/// <summary>
				/// Вертає список зареєстрованих типів даних
				/// </summary>
				/// <returns>Список імен зареєстрованиї типів даних</returns>
				forceinline Containers::List<String> GetAllRegisteredDataTypes() {
					Containers::List<String> dataTypeNames;
					data->dataTypeTableForManagedLevel.ForEachKey([&] (const String& str) mutable
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
				forceinline Reflection::DataTypeRecord GetDataTypesRecordByName(const String& name) {
					return data->dataTypeTableForManagedLevel[name];
				}

				/// <summary>
				/// Вертає мета інформація про тип даних за записом про тип
				/// </summary>
				/// <param name="name">Запис про типх</param>
				/// <returns>Мета інформація про тип даних</returns>
				forceinline Reflection::DataTypeRecord GetDataTypesRecordByType(const RTTI::RTTITypeRecord& id) {
					return data->dataTypeTableForNativeLevel[id];
				}

				/// <summary>
				/// Перевіряє чи мета інформація про тип даних за іменем типу
				/// </summary>
				/// <param name="name">Ім'я типу</param>
				/// <returns>true якщо тип даних зареєтрований</returns>
				forceinline Boolean IsExistDataTypesRecordByName(const String& name) {
					return data->dataTypeTableForManagedLevel.IsExist(name);
				}

				/// <summary>
				/// Перевіряє чи мета інформація про тип даних за записом про тип
				/// </summary>
				/// <param name="name">Запис про тип</param>
				/// <returns>true якщо тип даних зареєтрований</returns>
				forceinline Boolean IsExistDataTypesRecordByType(const RTTI::RTTITypeRecord& id) {
					return data->dataTypeTableForNativeLevel.IsExist(id);
				}
			public:
				RefCountClass(DepencyInjector, data);
			};

			using IDI = DepencyInjector::IDI;
		}
	}
}