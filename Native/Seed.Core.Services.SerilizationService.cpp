// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Services.SerilizationService.h"

using namespace Seed;

String Seed::Core::Services::SerilizationService::GetTypeMetaDataDynObj(const Reflection::DynamicObject& data) {
	auto& typeRegistry = This->idi.GetTypesRegistry();

	if (typeRegistry.Any([&] (const auto& item) InlineLambda
	{
		return item.Key == data.ValueType;
	}))
	{
		auto typeRecord = typeRegistry.First([&] (const auto& item) InlineLambda
		{
			return item.Key == data.ValueType;
		});

		return typeRecord.Value.TypeMetaData;
	}

	Throw(NotFoundException(ExceptionPosition));
}
