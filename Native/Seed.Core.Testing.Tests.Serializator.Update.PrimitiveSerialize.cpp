#include "Seed.Core.Testing.Tests.Serializator.Update.PrimitiveSerialize.h"

#include "Seed.Core.Serialization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Services.h"

using namespace Seed::Core::Testing::Test::Serializator::Update;

bool LogicSerialization::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    Boolean value = true;
    auto stream = Core::Serialization::Serialize(value);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(value)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 0, SourcePosition);

    return true;
}

bool NumericSerialization::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    Int32 value = 1;
    auto stream = Core::Serialization::Serialize(value);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(value)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 0, SourcePosition);

    return true;
}

bool FloatPointSerialization::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    Float value = 1;
    auto stream = Core::Serialization::Serialize(value);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(value)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 0, SourcePosition);

    return true;
}

bool CharacterSerialization::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    Char value = 1;
    auto stream = Core::Serialization::Serialize(value);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(value)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 0, SourcePosition);

    return true;
}

bool CFixedArraySerialization::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    Char value[] = L"Seed::Core::Testing::Test::Serialization::CFixedArraySerialization::Execute";
    auto stream = Core::Serialization::Serialize(value);
    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(value)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 0, SourcePosition);

    return true;
}
