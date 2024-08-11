#include "Seed.Core.Testing.Tests.Serializator.Update.Dynamic.h"

#include "Seed.Core.Serialization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Services.h"

using namespace Seed::Core::Testing::Test::Serializator::Update;

bool EmptyDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic();
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 1, SourcePosition);

    return true;
}

bool PrimitiveDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(10);
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 1, SourcePosition);

    return true;
}

bool StructDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(Core::IO::FS::FSObjectInformation(
        IO::FS::EFileObjectTypes::File, false, true, false, false, false, true, false, 10Kb, L"text.dat"));
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 1, SourcePosition);

    return true;
}

bool StringDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(L"Test"S);
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 1, SourcePosition);

    return true;
}

bool CollectionDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(Core::Containers::Array<SizeT>::Generate(ArrowFunct(const& i, i), 10));
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 1, SourcePosition);

    return true;
}

bool TupleDynamic::Execute() {
    using TTupleType = Core::Containers::Tuple<Int32, Float, Boolean, String>;

    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(TTupleType(1, 1, false, L"Text"));
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 1, SourcePosition);

    return true;
}

bool DynamicInDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(Core::RTTI::Dynamic(L"Text"S), {});
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 2, SourcePosition);

    return true;
}

bool DynamicStructFieldInDynamic::Execute() {
    struct StructWithDynamic {
        Core::Int32 Value1;
        Core::String Value2;
        Core::RTTI::Dynamic Value3;

        StructCttiDef(StructWithDynamic,
                      DefStructFieldData(StructWithDynamic, Value1),
                      DefStructFieldData(StructWithDynamic, Value2),
                      DefStructFieldData(StructWithDynamic, Value3));
    };

    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(StructWithDynamic{ .Value1 = 10, .Value2 = __FUNCSIG__, .Value3 = RTTI::Dynamic(String(__FUNCSIG__)) });
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 2, SourcePosition);

    return true;
}

bool CollectionWithDynamicInDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(Core::Containers::Array<Core::RTTI::Dynamic>::Generate(ArrowFunct(const& i, toString(i)), 10));
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);

    auto dataArray = data.As<Core::Containers::Array<Core::RTTI::Dynamic>>();
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 11, SourcePosition);

    return true;
}

bool TupleWithDynamicInDynamic::Execute() {
    using TTupleType = Core::Containers::Tuple<Int32, Float, Boolean, Core::RTTI::Dynamic>;

    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Core::RTTI::Dynamic(TTupleType(1, 1, false, L"Text"S));
    auto stream = service.Serialize(data);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 2, SourcePosition);

    return true;
}