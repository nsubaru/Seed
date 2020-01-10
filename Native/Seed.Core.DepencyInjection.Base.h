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
				DepencyInjector() : data(new Data()) {}

				template<typename TService>
				TService Resolve() {
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
						methodRecord.ArgsTypeInformationGetterFunction().ForEach([&](const Reflection::DataTypeRecord & typeRecord) {
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
						methodRecord.ArgsTypeInformationGetterFunction().ForEach([&](const Reflection::DataTypeRecord & typeRecord) {
							if (!this->data->dataTypeTableForNativeLevel.IsExist(&typeRecord.GetTypeInfo()))
							{
								this->data->dataTypeTableForNativeLevel.Insert(&typeRecord.GetTypeInfo(), typeRecord);
								this->data->dataTypeTableForManagedLevel.Insert(typeRecord.TypeName, typeRecord);
							}
						});
					});

					return *this;
				}

				template<typename T>
				DepencyInjector& RegisterObjectType(Reflection::ObjectRTTIRecord&& rttiRecord) {
					if (!this->data->dataTypeTableForNativeLevel.IsExist(&typeid(T)))
					{
						this->data->objectTypeTableForNativeLevel.Insert(&typeid(T), rttiRecord);
						this->data->objectTypeTableForManagedLevel.Insert(rttiRecord.ClassName, rttiRecord);
					}

					return *this;
				}
			public:

				RefCountClass(DepencyInjector, data);
			};
		}
	}
}