#include "Seed.Core.Testing.Tests.Serializator.Update.CompositsTests.h"

#include "Seed.Core.Serialization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Services.h"

using namespace Seed::Core::Testing::Test::Serializator::Update;

bool EmptyTypeTuple::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto tuple = Seed::Core::Containers::Tuple<>{ };

	auto stream = Core::Serialization::Serialize(tuple);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(tuple)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool SimpleTypeTuple::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto tuple = Seed::Core::Containers::Tuple<Int32, Int64, Char, Boolean>{1,1,'1',true};

	auto stream = Core::Serialization::Serialize(tuple);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(tuple)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool ComplexTypeTuple::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto tuple = Seed::Core::Containers::Tuple<String, Core::Containers::Array<Int32>, Core::Containers::List<Int32>, Boolean>
				{L"test",{1,2,3,4},{1,2,3,4},true};
	auto stream = Core::Serialization::Serialize(tuple);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(tuple)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

bool TupleTypeTuple::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto tuple = Seed::Core::Containers::Tuple<
		Seed::Core::Containers::Tuple<Int32, Int64, Boolean>,
		Seed::Core::Containers::Tuple<Seed::Core::Containers::Array<Int32>>,
		Boolean>{
			{1,1,true},
			{{1,2,3,4}},
			true
	};

	auto stream = Core::Serialization::Serialize(tuple);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(tuple)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}

namespace Seed::Core::Testing::Test::Serializator::Update
{
	class SerializableClass : Seed::Core::IDisposable, public Seed::Core::CTTI::TypeTraits::ISerializable<SerializableClass> {
	private:
		struct Data {
			Seed::Core::RefCountT RefCount;
			Seed::Core::String Name;
			Seed::Core::Containers::Map<Seed::Core::String, Seed::Core::String> Storage;
			Data() : RefCount(1) {}
		};
	private:
		Data* This;
	public:
		DefStructMetaData(Data,
			DefStructFieldData(Data, Name),
			DefStructFieldData(Data, Storage)
		);
	private:
		virtual Seed::Core::Byte* GetDataPtr() const override {
			return reinterpret_cast<Seed::Core::Byte*>(This);
		}
	protected:
		virtual void Dispose() override {}
	public:
		Seed::Core::String& Name() {
			return This->Name;
		}
		Seed::Core::Containers::Map<Seed::Core::String, Seed::Core::String>& Storage() {
			return This->Storage;
		}
	public:
		SerializableClass() : This(new Data) {}
	public:
		RefCountClass(SerializableClass, This)
	};
}

bool CustomStruct::Execute() {
	auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
	auto rttiSection = Core::RTTI::GetCurrentRttiSection();

	auto data = SerializableClass();
	data.Name() = L"Test";
	data.Storage() = {{L"one", L"1"}, {L"two", L"2"}, {L"three", L"3"}};

	auto stream = Core::Serialization::Serialize(data);
	auto updatedStream = stream;
	auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

	Asserts::AssertEqual(stream, updatedStream, SourcePosition);
	Asserts::AssertEqual(updatedCount, 0, SourcePosition);

	return true;
}
