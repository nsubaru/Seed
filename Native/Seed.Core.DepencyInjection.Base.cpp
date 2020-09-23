// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Seed.Core.DepencyInjection.Base.h"

#include "Seed.Core.Time.h"

void Seed::Core::DI::DepencyInjector::RegisterServiceTypes(const SmartPtr<ServiceRecord>& service) {
	service->rttiRecord.Methods.ForEach([&] (const Reflection::ObjectRTTIRecord::MethodRTTIRecord& methodRecord) InlineLambda
	{
		methodRecord.TypeInformationGetterFunction().ForEach([&](const Reflection::DataTypeRecord & typeRecord) InlineLambda 
		{
			if (this->data->objectTypeRecordRefs.IsExist(typeRecord.GetTypeInfo()))
			{
				this->data->objectTypeRecordRefs[typeRecord.GetTypeInfo()] += 1;
				return;
			}

			if (this->data->dataTypeRecordRefs.IsExist(typeRecord.GetTypeInfo()))
			{
				this->data->dataTypeRecordRefs[typeRecord.GetTypeInfo()] += 1;
				return;
			}

			this->data->dataTypeRecordRefs.Insert({ typeRecord.GetTypeInfo(), 1 });
			this->data->dataTypeTableForNativeLevel.Insert(typeRecord.GetTypeInfo(), typeRecord);
			this->data->dataTypeTableForManagedLevel.Insert(typeRecord.TypeName, typeRecord);
		});
	});
}

void Seed::Core::DI::DepencyInjector::UnRegisterServiceByTypeRecord(const RTTI::RTTITypeRecord& serviceRecord) {
	if (data->serviceTableForNativeLevel.IsExist(serviceRecord))
	{
		auto& service = data->serviceTableForNativeLevel[serviceRecord];

		auto dependents = Containers::List<RTTI::RTTITypeRecord>();
							
		data->serviceStore.ForEach([&] (const SmartPtr<ServiceRecord>& record) InlineLambda
		{
			record->depencies.ForEach([&] (const RTTI::RTTITypeRecord & info) InlineLambda
			{
				if (info == service->rttiRecord.GetRTTITypeInfo())
					dependents += service->rttiRecord.GetRTTITypeInfo();
			});
		});

		dependents.RemoveAll([&] (RTTI::RTTITypeRecord&& item) InlineLambda
		{
			UnRegisterServiceByTypeRecord(item);
		});

		UnregisterServiceTypes(service);

		data->serviceStore.RemoveIf([&] (const auto& item)
		{
			return item->rttiRecord.GetRTTITypeInfo() == service->rttiRecord.GetRTTITypeInfo();
		});

		data->serviceTableForManagedLevel.Remove(service->rttiRecord.ClassName);
		data->serviceTableForNativeLevel.Delete(service->rttiRecord.GetRTTITypeInfo());
	}
	else
	{
		Throw(NotFoundException(ExceptionPosition));
	}
}

void Seed::Core::DI::DepencyInjector::UnregisterServiceTypes(const SmartPtr<ServiceRecord>& service) {
	service->rttiRecord.Methods.ForEach([&] (const Reflection::ObjectRTTIRecord::MethodRTTIRecord& methodRecord) InlineLambda
	{
		methodRecord.TypeInformationGetterFunction().ForEach([&] (const Reflection::DataTypeRecord& typeRecord) InlineLambda
		{
			if (data->dataTypeRecordRefs.IsExist(typeRecord.GetTypeInfo())) {
				if ((--data->dataTypeRecordRefs[typeRecord.GetTypeInfo()]) == 0)
				{
					data->dataTypeTableForNativeLevel.Delete(typeRecord.GetTypeInfo());
					data->dataTypeTableForManagedLevel.Remove(typeRecord.TypeName);

					data->dataTypeRecordRefs.Delete(typeRecord.GetTypeInfo());

					return;
				}
			}
			else if (data->objectTypeRecordRefs.IsExist(typeRecord.GetTypeInfo())) {
				if ((--data->objectTypeRecordRefs[typeRecord.GetTypeInfo()]) == 0)
				{
					data->objectTypeTableForNativeLevel.Delete(typeRecord.GetTypeInfo());
						data->objectTypeTableForManagedLevel.Remove(typeRecord.TypeName);

						data->objectTypeRecordRefs.Delete(typeRecord.GetTypeInfo());

						return;
				}
			}
		});
	});
}

void Seed::Core::DI::DepencyInjector::Dispose() {
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
	//Видалення записів типів для серелізатора
	data->dataTypesListForSerilization.RemoveAll();
};

Seed::Core::DI::DepencyInjector::DepencyInjector() : data(new Data()) {
#define RegisterTypeDef(x) RegisterType<x>(); RegisterType<Containers::Tuple<x>>();
	RegisterTypeDef(Int8);
	RegisterTypeDef(Int16);
	RegisterTypeDef(Int32);
	RegisterTypeDef(Int64);

	RegisterTypeDef(UInt8);
	RegisterTypeDef(UInt16);
	RegisterTypeDef(UInt32);
	RegisterTypeDef(UInt64);

	RegisterTypeDef(UChar);

	RegisterTypeDef(Double);
	RegisterTypeDef(Float);
	RegisterTypeDef(Boolean);
	RegisterTypeDef(String);

	RegisterTypeDef(Time::Date);
	RegisterTypeDef(Time::DateSpan);
	RegisterTypeDef(Time::TimeSpan);
	RegisterTypeDef(Time::DateTime);
	RegisterTypeDef(Time::TimeRange);

	RegisterTypeDef(Containers::Blob);
	RegisterTypeDef(Containers::Dynamic);
	RegisterTypeDef(Reflection::DynamicObject);
#undef RegisterTypeDef
}
