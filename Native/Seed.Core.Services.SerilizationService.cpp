#include "Seed.Core.Services.SerilizationService.h"

using namespace Seed;

Containers::Blob SerelizeDynObj(const Reflection::DynamicObject& data, const Reflection::DataTypeRecord& typeRecord) {
	return typeRecord.GetSerelizeFunction()(data);
}

Boolean IsSerilizableObjectType(const Reflection::ObjectRTTIRecord& typeRecord, DI::IDI& idi) {
	return typeRecord.Properties.All([&] (const Reflection::ObjectRTTIRecord::PropertyRTTIRecord& typeProperty) -> Boolean
	{
		if (typeProperty.IsSerilizable)
		{
			return true;
		}
		else
		{
			if (idi.IsExistDataTypesRecordByName(typeProperty.PropertieTypeName))
			{
				auto dataRecord = idi.GetDataTypesRecordByName(typeProperty.PropertieTypeName);
				return dataRecord.IsSerelizableType;
			}
			else if (idi.IsExistObjectRecordByName(typeProperty.PropertieTypeName))
			{
				return IsSerilizableObjectType(idi.GetObjectRecordByName(typeProperty.PropertieTypeName), idi);
			}
			else
			{
				return false;
			}
		}
	});
}

Containers::Blob SerilizeObjectType(
	const Reflection::ObjectRTTIRecord& typeRecord,
	const DI::IDI& idi,
	const Reflection::DynamicObject& object);

void SerilizeObjectTypeImpl(
	const Reflection::ObjectRTTIRecord& typeRecord,
	const DI::IDI& idi,
	Reflection::DynamicObject& object,
	const Reflection::ObjectRTTIRecord::PropertyRTTIRecord& typeProperty,
	Containers::VList<Byte>& buffer) {
	if (typeProperty.IsSerilizable)
	{
		buffer += typeProperty.SerilizationFunction(object);
	}
	else
	{
		if (idi.IsExistDataTypesRecordByName(typeProperty.PropertieTypeName))
		{
			auto dataRecord = idi.GetDataTypesRecordByName(typeProperty.PropertieTypeName);
			auto propertyData = typeProperty.CallGetterForObject(object);
			buffer += dataRecord.SerelizeFunction(propertyData);
		}
		else if (idi.IsExistObjectRecordByName(typeProperty.PropertieTypeName))
		{
			auto typeRecord = idi.GetObjectRecordByName(typeProperty.PropertieTypeName);
			auto propertyData = typeProperty.CallGetterForObject(object);
			buffer += SerilizeObjectType(typeRecord, idi, propertyData);
		}
	}
}

Containers::Blob SerilizeObjectType(
	const Reflection::ObjectRTTIRecord& typeRecord, 
	const DI::IDI& idi, 
	const Reflection::DynamicObject& object) {
	return typeRecord.Properties.Aggregate([&] (Containers::VList<Byte>& buffer, const Reflection::ObjectRTTIRecord::PropertyRTTIRecord& typeProperty) -> void
	{
		//We know that getter didn't change object value but getter caller for unification use non const object
		//so we should remove const modificator from object
		SerilizeObjectTypeImpl(typeRecord, idi, const_cast<Reflection::DynamicObject&>(object), typeProperty, buffer);
	}, Containers::VList<Byte>())
		.ToArray();
}

Containers::Blob Seed::Core::Services::SerelizationService::SerelizeDynObj(const Reflection::DynamicObject& data) {
	//Try search type in object records storage by type name
	if (This->idi.IsExistObjectRecordByName(data.GetTypeName()))
	{
		//if found get type record
		auto typeRecord = This->idi.GetObjectRecordByName(data.GetTypeName());
		//check is serilizable if true then serilize properties else throw exception
		if (IsSerilizableObjectType(typeRecord, This->idi)) {
			return SerilizeObjectType(typeRecord, This->idi, data);
		}
		else
		{
			Throw(Reflection::NonSerelizableType(ExceptionPosition, data.GetCxxTypeName()));
		}
	} 
	//else try search type in data types storage by type name
	else if (This->idi.IsExistDataTypesRecordByName(data.GetTypeName()))
	{
		//if found then get type record
		auto typeRecord = This->idi.GetDataTypesRecordByName(data.GetTypeName());
		//check is serializable if true serilize using function in type record else throw exception
		if (typeRecord.IsSerelizableType)
		{
			return typeRecord.SerelizeFunction(data);
		}
		else
		{
			Throw(Reflection::NonSerelizableType(ExceptionPosition, data.GetCxxTypeName()));
		}
	}
	else
	{
		Throw(Reflection::NonSerelizableType(ExceptionPosition, data.GetCxxTypeName()));
	}
}

Reflection::DynamicObject Seed::Core::Services::SerelizationService::DeserelizeDynObj(const Containers::Blob& data) {
	Throw(NotImplementedException(ExceptionPosition));
}

String Seed::Core::Services::SerelizationService::GetTypeMetaDataDynObj(const Reflection::DynamicObject& data) {
	Throw(NotImplementedException(ExceptionPosition));
}
