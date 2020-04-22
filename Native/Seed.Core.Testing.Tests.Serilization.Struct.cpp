#include "Seed.Core.Testing.Tests.Serilization.Struct.h"

bool Seed::Core::Testing::Test::Serilization::StructSerilization::Excecute() {
	struct Data {
		SizeT CountNonCopyRecord;
		Containers::Map<ID, Containers::Tuple<String, Containers::Array<Int32>, Containers::Dictionary<ID>>> Records;

		StructCttiDef(Data, 
			DefStructFieldData(Data, CountNonCopyRecord),
            DefStructFieldData(Data, Records)
        );
	};

	auto data = Data{ 1,
		{{1, {L"text", {1,2,3}, {{"Rec1", 1}, {"Rec2", 2}}}} } };

	constexpr auto v = Data::FieldMetaDataGetter();
	constexpr auto v1 = decltype(v)::IsSerilizable;
	constexpr auto v2 = decltype(v)::Fields::Count;
	constexpr auto v3 = Core::Serilization::CTTIMetaDataGen<Data>();

	toCoreLog(L"Test Data typeClass     : " + String::toString((Int16)Core::CTTI::TypeTraits::CTTITypeMetaData<Data>::TypeClass));
	toCoreLog(L"Test Data IsSerilizable : " + String::toString(v1));
	toCoreLog(L"Test Data fields count  : " + String::toString(v2));
	toCoreLog(L"Test Data type name     : " + String(v3));

	Asserts::AssertTrue([&]
	{
		return String(v3).Lenght > 0;
	});

	//auto schema = Core::Serilization::MetaDataGen<Data>();
	auto blob = Core::Serilization::Serilize(data);
	auto deserilizedData = Data{};
	Core::Serilization::Deserilize<Data>(blob, deserilizedData);

	Asserts::AssertTrue([&] ()
	{
		if (data.CountNonCopyRecord != deserilizedData.CountNonCopyRecord)
			return false;

		auto dataIt = data.Records.FirstIt();
		auto deserilizedIt = deserilizedData.Records.FirstIt();

		for (; dataIt != nullptr && deserilizedIt != nullptr; ++dataIt, ++deserilizedIt)
		{
			if (dataIt->Key != deserilizedIt->Key)
				return false;

			if (dataIt->Value.Count != deserilizedIt->Value.Count)
				return false;

			if (dataIt->Value.GetByIdx<0>() != deserilizedIt->Value.GetByIdx<0>())
				return false;

			if (dataIt->Value.GetByIdx<1>().Count != deserilizedIt->Value.GetByIdx<1>().Count)
				return false;

			for (SizeT i = 0; i < dataIt->Value.GetByIdx<1>().Count; i++)
			{
				if (dataIt->Value.GetByIdx<1>()[i] != deserilizedIt->Value.GetByIdx<1>()[i])
					return false;
			}

			if (dataIt->Value.GetByIdx<2>().Count != deserilizedIt->Value.GetByIdx<2>().Count)
				return false;

			bool isAllItemsAreInDictionaries = true;

			dataIt->Value.GetByIdx<2>().ForEach([&] (const Containers::Pair<String, ID>& item)
			{
				try
				{
					isAllItemsAreInDictionaries = (isAllItemsAreInDictionaries) ?
						(deserilizedIt->Value.GetByIdx<2>()[item.Key] == item.Value) : false;
				}
				catch (NotFoundException&)
				{
					isAllItemsAreInDictionaries = false;
				}
			});

			if (!isAllItemsAreInDictionaries)
				return false;

			if (dataIt->Value.GetByIdx<1>() != deserilizedIt->Value.GetByIdx<1>())
				return false;
		}

		return true;
	});

	return true;// schema.Lenght != 0;
}
