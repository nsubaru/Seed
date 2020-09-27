#include "Seed.Core.Testing.Tests.Serilization.TypeMetaDataRecord.h"

#include "Seed.Core.FS.h"

bool Seed::Core::Testing::Test::Serilization::TypeMetadataRecordCreate::Excecute()
{
    auto record = Core::Serilization::GetTypeRecordForType<Core::Containers::List<Core::String>>();
    
    Asserts::AssertEqual(record.TypeName, CTTI::TypeName<Core::Containers::List<Core::String>>());
    Asserts::AssertEqual(record.TypeClass, CTTI::EType::Collection);
    Asserts::AssertEqual(record.NestedTypes.FirstIt->TypeName, CTTI::TypeName<Core::String>());
    Asserts::AssertEqual(record.NestedTypes.FirstIt->TypeClass, CTTI::EType::String);
    Asserts::AssertEqual(record.NestedTypes.FirstIt->NestedTypes.FirstIt->SizeOfSize, sizeof(UChar));
    Asserts::AssertEqual(record.NestedTypes.FirstIt->NestedTypes.FirstIt->StaticSize, sizeof(UChar));
    Asserts::AssertEqual(record.NestedTypes.FirstIt->NestedTypes.FirstIt->TypeClass, CTTI::EType::Character);

    return true;
}

bool Seed::Core::Testing::Test::Serilization::TypeMetaDataRecordDecode::Excecute()
{
    auto strMetaData = Core::Serilization::MetaDataGen
        <Core::Containers::Tuple<Core::Containers::Dictionary<Core::FS::FSObjectInformation>, Core::Containers::List<Core::FS::VolumeInformation>, SizeT, Boolean>>();

    auto metaDataDecoded = Core::Serilization::TypeMetaDataRecord::DecodeTypeMetaDataRecord(strMetaData);

    Asserts::AssertEqual(String(Core::CTTI::TypeName
        <Core::Containers::Tuple<Core::Containers::Dictionary<Core::FS::FSObjectInformation>, Core::Containers::List<Core::FS::VolumeInformation>, SizeT, Boolean>>()),
        metaDataDecoded.TypeName);

    Asserts::AssertEqual(metaDataDecoded.TypeClass, Core::CTTI::EType::Tuple);

    Asserts::AssertEqual(metaDataDecoded.NestedTypes.LastIt->TypeName, Core::CTTI::TypeName<Boolean>());
    Asserts::AssertEqual(metaDataDecoded.NestedTypes.LastIt->TypeClass, Core::CTTI::EType::Logic);

    return true;
}

bool Seed::Core::Testing::Test::Serilization::TypeMetaDataRecordSerelize::Excecute()
{
    auto registry = Core::Serilization::TypesRegistry{};
    auto record = Core::Serilization::GetTypeRecordForType<Core::Containers::List<Core::String>>();
    auto serelizedData = Core::Serilization::Serilize(record, registry);
    decltype(record) deserelizedRecord;
    Core::Serilization::Deserilize<decltype(record)>(serelizedData, deserelizedRecord, registry);

    Asserts::AssertEqual(record.TypeClass, deserelizedRecord.TypeClass);
    Asserts::AssertEqual(record.TypeName, deserelizedRecord.TypeName);
    Asserts::AssertEqual(record.SizeOfSize, deserelizedRecord.SizeOfSize);
    Asserts::AssertEqual(record.StaticSize, deserelizedRecord.StaticSize);

    return false;
}
