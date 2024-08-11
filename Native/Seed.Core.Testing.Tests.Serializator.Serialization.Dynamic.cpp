#include "Seed.Core.Testing.Tests.Serializator.Serialization.Dynamic.h"

#include "Seed.Core.Serialization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Services.h"

using namespace Seed::Core::Testing::Test::Serializator::Serialization;

bool EmptyDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic();
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);

    return true;
}

bool PrimitiveDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic(10);
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);
    Asserts::AssertEqual(data.As<Int32>(), deserializedData.As<Int32>(), SourcePosition);

    return true;
}

bool StructDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic(Core::IO::FS::FSObjectInformation(
        IO::FS::EFileObjectTypes::File, false, true, false, false, false, true, false, 10Kb, L"text.dat" ));
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);

    Asserts::AssertEqual(data.As<Core::IO::FS::FSObjectInformation>().Flags, deserializedData.As<Core::IO::FS::FSObjectInformation>().Flags, SourcePosition);
    Asserts::AssertEqual(data.As<Core::IO::FS::FSObjectInformation>().Type, deserializedData.As<Core::IO::FS::FSObjectInformation>().Type, SourcePosition);
    Asserts::AssertEqual(data.As<Core::IO::FS::FSObjectInformation>().Size, deserializedData.As<Core::IO::FS::FSObjectInformation>().Size, SourcePosition);
    Asserts::AssertEqual(data.As<Core::IO::FS::FSObjectInformation>().Name, deserializedData.As<Core::IO::FS::FSObjectInformation>().Name, SourcePosition);

    return true;
}

bool StringDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic(L"Test"S);
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);
    Asserts::AssertEqual(data.As<String>(), deserializedData.As<String>(), SourcePosition);

    return true;
}

bool CollectionDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic(Core::Containers::Array<SizeT>::Generate(ArrowFunct(const &i, i), 10));
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);
    Asserts::AssertEqual(data.As<Core::Containers::Array<SizeT>>(), deserializedData.As<Core::Containers::Array<SizeT>>(), SourcePosition);

    return true;
}

bool TupleDynamic::Execute() {
    using TTupleType = Core::Containers::Tuple<Int32, Float, Boolean, String>;
    
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic(TTupleType(1, 1, false, L"Text"));
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);

    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<0>(), deserializedData.As<TTupleType>().GetByIdx<0>(), SourcePosition);
    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<1>(), deserializedData.As<TTupleType>().GetByIdx<1>(), SourcePosition);
    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<2>(), deserializedData.As<TTupleType>().GetByIdx<2>(), SourcePosition);
    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<3>(), deserializedData.As<TTupleType>().GetByIdx<3>(), SourcePosition);

    return true;
}

bool DynamicInDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic(Core::RTTI::Dynamic(L"Text"S), {});
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);
    Asserts::AssertEqual(data.As<Core::RTTI::Dynamic>().Type, deserializedData.As<Core::RTTI::Dynamic>().Type, SourcePosition);
    Asserts::AssertEqual(data.As<Core::RTTI::Dynamic>().As<String>(), deserializedData.As<Core::RTTI::Dynamic>().As<String>(), SourcePosition);

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

    auto data = Core::RTTI::Dynamic(StructWithDynamic{ .Value1 = 10, .Value2 = __FUNCSIG__, .Value3 = RTTI::Dynamic(String(__FUNCSIG__))});
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);

    Asserts::AssertEqual(data.As<StructWithDynamic>().Value1, deserializedData.As<StructWithDynamic>().Value1, SourcePosition);
    Asserts::AssertEqual(data.As<StructWithDynamic>().Value2, deserializedData.As<StructWithDynamic>().Value2, SourcePosition);

    Asserts::AssertEqual(data.As<StructWithDynamic>().Value3.Type, deserializedData.As<StructWithDynamic>().Value3.Type, SourcePosition);
    Asserts::AssertEqual(data.As<StructWithDynamic>().Value3.As<String>(), deserializedData.As<StructWithDynamic>().Value3.As<String>(), SourcePosition);

    return true;
}

bool CollectionWithDynamicInDynamic::Execute() {
    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic(Core::Containers::Array<Core::RTTI::Dynamic>::Generate(ArrowFunct(const& i, toString(i)), 10));
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);

    auto dataArray = data.As<Core::Containers::Array<Core::RTTI::Dynamic>>();
    auto deserializedDataArray = deserializedData.As<Core::Containers::Array<Core::RTTI::Dynamic>>();

    Asserts::AssertEqual(dataArray.Count, deserializedDataArray.Count, SourcePosition);

    for (SizeT i = 0; i < dataArray.Count; i++)
    {
        Asserts::AssertEqual(dataArray[i].Type, deserializedDataArray[i].Type, SourcePosition);
        Asserts::AssertEqual(dataArray[i].As<String>(), deserializedDataArray[i].As<String>(), SourcePosition);
    }

    return true;
}

bool TupleWithDynamicInDynamic::Execute() {
    using TTupleType = Core::Containers::Tuple<Int32, Float, Boolean, Core::RTTI::Dynamic>;

    auto service = Core::Services::GetGlobalIOC().Resolve<Services::SerializationService>();

    auto data = Core::RTTI::Dynamic(TTupleType(1, 1, false, L"Text"S));
    auto stream = service.Serialize(data);
    auto deserializedData = service.DeserializeDynObj(stream);

    Asserts::AssertEqual(data.Type, deserializedData.Type, SourcePosition);

    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<0>(), deserializedData.As<TTupleType>().GetByIdx<0>(), SourcePosition);
    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<1>(), deserializedData.As<TTupleType>().GetByIdx<1>(), SourcePosition);
    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<2>(), deserializedData.As<TTupleType>().GetByIdx<2>(), SourcePosition);

    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<3>().Type, deserializedData.As<TTupleType>().GetByIdx<3>().Type, SourcePosition);
    Asserts::AssertEqual(data.As<TTupleType>().GetByIdx<3>().As<String>(), deserializedData.As<TTupleType>().GetByIdx<3>().As<String>(), SourcePosition);

    return true;
}