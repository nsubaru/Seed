// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.CTTI.ToTuple.h"

#include "Seed.Core.CTTI.h"

bool Seed::Core::Testing::Test::CTTI::ToTuple::Excecute() {
	struct Data {
		Int32 a;
		Int32 b;
		Int32 c;

		StructCttiDef(Data, 
			DefStructFieldData(Data, a),
            DefStructFieldData(Data, b),
            DefStructFieldData(Data, c)
        );
	};

	Asserts::AssertTrue([] ()
	{
		return Core::CTTI::TypeTraits::CTTITypeMetaData<Data>::TypeClass == Core::CTTI::TypeTraits::EType::Struct;
	});

	Asserts::AssertTrue([] ()
	{
		return Core::CTTI::TypeTraits::CTTITypeMetaData<Data>::IsConvertibleToTuple;
	});

	constexpr auto data = Data{ 1,2,3 };

	auto tupleData = Core::CTTI::TypeTraits::CTTITypeMetaData<Data>::ToTuple(data);

	return (tupleData.GetByIdx<0>() + tupleData.GetByIdx<1>() + tupleData.GetByIdx<2>()) == 6;
}
