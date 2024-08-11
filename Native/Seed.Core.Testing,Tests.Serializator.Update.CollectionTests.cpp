#include "Seed.Core.Testing,Tests.Serializator.Update.CollectionTests.h"

#include "Seed.Core.Serialization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Services.h"

using namespace Seed::Core::Testing::Test::Serializator::Update;

bool String::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	Core::String value = __FUNCSIG__;
	auto stream = Core::Serialization::Serialize(value);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(value)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool Array::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto array = Containers::Array<Containers::Array<SizeT>>::Generate([] () {
		return Containers::Array<SizeT>::Generate([] (SizeT Index) {
			return Index * 10;
		}, 100);
	}, 100);

	auto stream = Core::Serialization::Serialize(array);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(array)>(), updatedStream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool List::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto list = Containers::List<Containers::List<SizeT>>::Generate([]() InlineLambda{
		return Containers::List<SizeT>::Generate([](SizeT Index) InlineLambda {
			return Index * 10;
		}, 100);
	}, 100);

	auto stream = Core::Serialization::Serialize(list);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(list)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool DynamicQueue::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto queue = Containers::DynamicQueue<Containers::DynamicQueue<SizeT>>::Generate([]() InlineLambda{
		return Containers::DynamicQueue<SizeT>::Generate([](SizeT index) InlineLambda {
			return index * 10;
		}, 100);
	}, 100);

	auto stream = Core::Serialization::Serialize(queue);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(queue)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool FixedArray::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto array = Core::Containers::FixedArray<10, Core::Containers::FixedArray<10, SizeT>>::Generate([](SizeT i) InlineLambda
	{
		return Core::Containers::FixedArray<10, SizeT>::Generate([&](SizeT j) InlineLambda
		{
			return i * 10 + j;
		});
	});

	auto stream = Core::Serialization::Serialize(array);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(array)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool FixedQueue::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto queue = Core::Containers::FixedQueue<10, Core::Containers::FixedQueue<10, Int32>>{
		{1,2,3,4,5},
		{1,2,3,4,5},
		{1,2,3,4,5},
		{1,2,3,4,5}
	};

	auto stream = Core::Serialization::Serialize(queue);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(queue)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool Dictionary::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto dictionary = Core::Containers::Dictionary<Int32>{
		{L"один", 1},
		{L"два", 2},
		{L"три", 3},
		{L"чотири", 4},
		{L"п'ять", 5},
		{L"шість", 6},
		{L"сім", 7},
		{L"вісім", 8},
		{L"дев'ять", 9},
	};

	auto stream = Core::Serialization::Serialize(dictionary);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(dictionary)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool Map::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto map = Core::Containers::Map<Core::String, Int32>{
		{L"один", 1},
		{L"два", 2},
		{L"три", 3},
		{L"чотири", 4},
		{L"п'ять", 5},
		{L"шість", 6},
		{L"сім", 7},
		{L"вісім", 8},
		{L"дев'ять", 9},
	};

	auto stream = Core::Serialization::Serialize(map);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(map)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool PriorityQueue::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto priortyQueue = Containers::Map<Int32, Core::String>{
		{1, L"один"},
		{2, L"два"},
		{3, L"три"},
		{4, L"чотири"},
		{5, L"п'ять"},
		{6, L"шість"},
		{7, L"сім"},
		{8, L"вісім"},
		{9, L"дев'ять"},
		{10, L"десять"}
	};

	auto stream = Core::Serialization::Serialize(priortyQueue);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(priortyQueue)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool DynamicStackTest::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto stack = Core::Containers::DynamicStack<SizeT>::Generate(ArrowFunct(i, i), 10);

	auto stream = Core::Serialization::Serialize(stack);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(stack)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}