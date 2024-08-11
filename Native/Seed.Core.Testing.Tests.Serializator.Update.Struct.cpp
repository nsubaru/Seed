#include "Seed.Core.Testing.Tests.Serializator.Update.Struct.h"

#include "Seed.Core.Serialization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Services.h"

using namespace Seed::Core::Testing::Test::Serializator::Update;

bool Struct::Execute() {
	struct Data {
		SizeT CountNonCopyRecord;
		Core::Containers::Map<ID, Core::Containers::Tuple<String, Core::Containers::Array<Int32>, Core::Containers::Dictionary<ID>>> Records;

		StructCttiDef(Data,
			DefStructFieldData(Data, CountNonCopyRecord),
			DefStructFieldData(Data, Records)
		)
	};

	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto data = Data{ 1,
		{{1, {L"text", {1,2,3}, {{"Rec1", 1}, {"Rec2", 2}}}} } };

	Asserts::AssertTrue([&]() InlineLambda
	{
		return Core::String(Core::Serialization::CTTIMetaDataGen<Data>()).Length > 0;
	}, SourcePosition);

	auto stream = Core::Serialization::Serialize(data);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}
