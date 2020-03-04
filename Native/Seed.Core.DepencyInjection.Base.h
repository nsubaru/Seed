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
					return ExceptionString(exeptionName)
						+ ExceptionString(L". Ім'я типу: ")
						+ typeName
						+ ExceptionString(L". ")
						+ location.toString()
						+ ExceptionString(L". ");
				}

				virtual ~DIResolveException() {}
			};

			///<summary>Помилка реєстрації сервісу</summary>
			class SeedKernelCall DIRegisterService : public Exception {
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
					return ExceptionString(exeptionName)
						+ ExceptionString(L". Ім'я сервісу: ")
						+ typeName
						+ ExceptionString(L". ")
						+ location.toString()
						+ ExceptionString(L". ");
				}

				virtual ~DIRegisterService() {}
			};

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

			using DynamicObject = Reflection::DynamicObject;

			class DepencyInjector;

			class DepencyInjector final : IDisposable {
			private:
				using TResolverCtor = void* (*)(DepencyInjector & injector);
				using TInterfaceCaster = void* (*)(void*);
			private:
				struct ServiceRecord {
					TResolverCtor resolverCtor;
					TInterfaceCaster interfaceCaster;
					Reflection::ObjectRTTIRecord rttiRecord;
					void* objPtr;

					ServiceRecord(TResolverCtor resolverCtor, TInterfaceCaster interfaceCaster, Reflection::ObjectRTTIRecord && rttiRecord)
						:resolverCtor(resolverCtor), interfaceCaster(interfaceCaster), rttiRecord(MoveRValue(rttiRecord)), objPtr(nullptr) {}

					~ServiceRecord() {
						if (objPtr != nullptr)
						{
							rttiRecord.DtorCallerFunction(objPtr);
							objPtr = nullptr;
						}
					}
				};
			private:
				struct Data {
					Containers::ConcurencyMap<const TypeInfo*, SmartPtr<ServiceRecord>> serviceTableForNativeLevel;
					Containers::ConcurencyDictionary<SmartPtr<ServiceRecord>> serviceTableForManagedLevel;
					Containers::ConcurencyMap<const TypeInfo*, Reflection::DataTypeRecord> dataTypeTableForNativeLevel;
					Containers::ConcurencyDictionary<Reflection::DataTypeRecord> dataTypeTableForManagedLevel;
					Containers::ConcurencyMap<const TypeInfo*, Reflection::ObjectRTTIRecord> objectTypeTableForNativeLevel;
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
				virtual void Dispose() override {};
			public:
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
					forceinline Containers::List<String> GetAllServicesNames() {
						return di.GetAllServicesNames();
					}

					forceinline Reflection::ObjectRTTIRecord GetServiceRecordByName(const String& name) {
						return di.GetServiceRecordByName(name);
					}

					forceinline Containers::List<String> GetAllRegisteredObjects() {
						return di.GetAllRegisteredObjects();
					}

					forceinline Reflection::ObjectRTTIRecord GetObjectRecordByName(const String& name) {
						return di.GetObjectRecordByName(name);
					}

					forceinline Containers::List<String> GetAllRegisteredDataTypes() {
						return di.GetAllRegisteredDataTypes();
					}

					forceinline Reflection::DataTypeRecord GetDataTypesRecordByName(const String& name) {
						return di.GetDataTypesRecordByName(name);
					}

					forceinline void RegisterObjectRTTIRecord(const Reflection::ObjectRTTIRecord& objectRTTIRecord) {
						di.RegisterObjectType(objectRTTIRecord);
					}

					forceinline void RegisterObjectRTTIRecord(Reflection::ObjectRTTIRecord&& objectRTTIRecord) {
						di.RegisterObjectType(MoveRValue(objectRTTIRecord));
					}

					forceinline void RemoveObjectRTTIRecord(const Reflection::ObjectRTTIRecord& objectRTTIRecord) {
						di.RemoveObjectType(objectRTTIRecord);
					}
				private:
					friend class DepencyInjector;
				};
			public:
				DepencyInjector() : data(new Data()) {}

				template<typename TService>
				TService Resolve() {
					if constexpr (TypeTraits::IsSameV<TService, IDI>)
					{
						return IDI(*this);
					}
					else
					{
						if (data->serviceTableForNativeLevel.IsExist(&typeid(TService)))
						{
							auto service = data->serviceTableForNativeLevel[&typeid(TService)];
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

						Throw(DIResolveException(ExceptionPosition, NewCTTI::TypeName<TService>(), typeid(TService)));
					}
				};

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

					if (rttiRecord.GetRTTITypeInfo() != typeid(TService))
						Throw(DIRegisterService(ExceptionPosition, NewCTTI::TypeName<TService>(), typeid(TService)));

					auto serviceRecord = SmartPtr<ServiceRecord>{ new ServiceRecord(
						InjectorCtor,
						nullptr,
						MoveRValue(rttiRecord)
					) };

					this->data->serviceTableForNativeLevel.Insert(&typeid(TService), serviceRecord);
					this->data->serviceTableForManagedLevel.Insert(serviceRecord->rttiRecord.ClassName, serviceRecord);

					serviceRecord->rttiRecord.Methods.ForEach([&] (const Reflection::ObjectRTTIRecord::MethodRTTIRecord& methodRecord)
					{
						methodRecord.TypeInformationGetterFunction().ForEach([&](const Reflection::DataTypeRecord & typeRecord) {
							if (!this->data->dataTypeTableForNativeLevel.IsExist(&typeRecord.GetTypeInfo()))
							{
								if (!this->data->objectTypeTableForNativeLevel.IsExist(&typeRecord.GetTypeInfo()))
								{
									this->data->dataTypeTableForNativeLevel.Insert(&typeRecord.GetTypeInfo(), typeRecord);
									this->data->dataTypeTableForManagedLevel.Insert(typeRecord.TypeName, typeRecord);
								}
							}
						});
					});

					return *this;
				}

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

					if (rttiRecord.GetRTTITypeInfo() != typeid(TService))
						Throw(DIRegisterService(ExceptionPosition, NewCTTI::TypeName<TService>(), typeid(TService)));

					auto serviceRecord = SmartPtr<ServiceRecord>{ new ServiceRecord(
						InjectorCtor,
						InterfaceGetter,
						MoveRValue(rttiRecord)
					) };

					this->data->serviceTableForNativeLevel.Insert(&typeid(TServiceInterface), serviceRecord);
					this->data->serviceTableForManagedLevel.Insert(NewCTTI::TypeName<TServiceInterface>(), serviceRecord);

					serviceRecord->rttiRecord.Methods.ForEach([&] (const Reflection::ObjectRTTIRecord::MethodRTTIRecord& methodRecord)
					{
						methodRecord.TypeInformationGetterFunction().ForEach([&](const Reflection::DataTypeRecord & typeRecord) {
							if (!this->data->dataTypeTableForNativeLevel.IsExist(&typeRecord.GetTypeInfo()))
							{
								this->data->dataTypeTableForNativeLevel.Insert(&typeRecord.GetTypeInfo(), typeRecord);
								this->data->dataTypeTableForManagedLevel.Insert(typeRecord.TypeName, typeRecord);
							}
						});
					});

					return *this;
				}

				DepencyInjector& RegisterObjectType(Reflection::ObjectRTTIRecord&& rttiRecord) {
					
					if (!this->data->objectTypeTableForNativeLevel.IsExist(&rttiRecord.GetRTTITypeInfo()))
					{
						this->data->objectTypeTableForNativeLevel.Insert(&rttiRecord.GetRTTITypeInfo(), rttiRecord);
						this->data->objectTypeTableForManagedLevel.Insert(rttiRecord.ClassName, MoveRValue(rttiRecord));
					}

					return *this;
				}

				DepencyInjector& RegisterObjectType(const Reflection::ObjectRTTIRecord& rttiRecord) {
					
					if (!this->data->objectTypeTableForNativeLevel.IsExist(&rttiRecord.GetRTTITypeInfo()))
					{
						this->data->objectTypeTableForNativeLevel.Insert(&rttiRecord.GetRTTITypeInfo(), rttiRecord);
						this->data->objectTypeTableForManagedLevel.Insert(rttiRecord.ClassName, rttiRecord);
					}

					return *this;
				}

				DepencyInjector& RemoveObjectType(const Reflection::ObjectRTTIRecord& rttiRecord) {
					if (this->data->objectTypeTableForNativeLevel.IsExist(&rttiRecord.GetRTTITypeInfo())) {
						this->data->objectTypeTableForNativeLevel.Delete(&rttiRecord.GetRTTITypeInfo());
						this->data->objectTypeTableForManagedLevel.Remove(rttiRecord.ClassName);
					}

					return *this;
				}

				forceinline Containers::List<String> GetAllServicesNames() {
					Containers::List<String> servicesNames;
					data->serviceTableForManagedLevel.ForEachKey([&] (const String& str) mutable
					{
						servicesNames += str;
					});
					return servicesNames;
				}

				forceinline Reflection::ObjectRTTIRecord GetServiceRecordByName(const String& name) {
					return data->serviceTableForManagedLevel[name]->rttiRecord;
				}

				forceinline Containers::List<String> GetAllRegisteredObjects() {
					Containers::List<String> objectNames;
					data->objectTypeTableForManagedLevel.ForEachKey([&] (const String& str) mutable
					{
						objectNames += str;
					});
					return objectNames;
				}

				forceinline Reflection::ObjectRTTIRecord GetObjectRecordByName(const String& name) {
					return data->objectTypeTableForManagedLevel[name];
				}

				forceinline Containers::List<String> GetAllRegisteredDataTypes() {
					Containers::List<String> dataTypeNames;
					data->dataTypeTableForManagedLevel.ForEachKey([&] (const String& str) mutable
					{
						dataTypeNames += str;
					});
					return dataTypeNames;
				}

				forceinline Reflection::DataTypeRecord GetDataTypesRecordByName(const String& name) {
					return data->dataTypeTableForManagedLevel[name];
				}
			public:

				RefCountClass(DepencyInjector, data);
			};

			using IDI = DepencyInjector::IDI;
		}
	}
}