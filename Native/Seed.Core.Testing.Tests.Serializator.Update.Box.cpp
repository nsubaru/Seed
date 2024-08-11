#include "Seed.Core.Testing.Tests.Serializator.Update.Box.h"

#include "Seed.Core.Serialization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Services.h"

using namespace Seed::Core::Testing::Test::Serializator::Update;

bool LazySerialization::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = Lazy<String>(ArrowFunct(String(L"" __FUNCSIG__)));
    auto stream = Core::Serialization::Serialize(data);

    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 0, SourcePosition);

    return true;
}

bool SmartPtrSerialization::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();
    auto rttiSection = Core::RTTI::GetCurrentRttiSection();

    auto data = SmartPtr<String>(new String(L"" __FUNCSIG__));
    auto stream = Core::Serialization::Serialize(data);

    auto updatedStream = stream;
    auto updatedCount = service.StreamInPlaceUpdate(TypeRecord<decltype(data)>(), stream, { rttiSection });

    Asserts::AssertEqual(stream, updatedStream, SourcePosition);
    Asserts::AssertEqual(updatedCount, 0, SourcePosition);

    return true;
}