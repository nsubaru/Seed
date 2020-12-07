#include "Seed.Core.Testing.Tests.Serilization.TypeMetaDataRecord.h"

#include "Seed.Core.IO.FS.h"

bool Seed::Core::Testing::Test::Serilization::TypeMetadataRecordCreate::Excecute()
{
    auto record = Core::Serilization::GetTypeRecordForType<Core::Containers::List<Core::String>>();
    
    Asserts::AssertEqual(record.TypeName, CTTI::TypeName<Core::Containers::List<Core::String>>(), ExceptionPosition);
    Asserts::AssertEqual(record.TypeClass, CTTI::EType::Collection, ExceptionPosition);
    Asserts::AssertEqual(record.NestedTypes.FirstIt->TypeName, CTTI::TypeName<Core::String>(), ExceptionPosition);
    Asserts::AssertEqual(record.NestedTypes.FirstIt->TypeClass, CTTI::EType::String, ExceptionPosition);
    Asserts::AssertEqual(record.NestedTypes.FirstIt->NestedTypes.FirstIt->SizeOfSize, sizeof(UChar), ExceptionPosition);
    Asserts::AssertEqual(record.NestedTypes.FirstIt->NestedTypes.FirstIt->StaticSize, sizeof(UChar), ExceptionPosition);
    Asserts::AssertEqual(record.NestedTypes.FirstIt->NestedTypes.FirstIt->TypeClass, CTTI::EType::Character, ExceptionPosition);

    return true;
}

bool Seed::Core::Testing::Test::Serilization::TypeMetaDataRecordDecode::Excecute()
{
    auto strMetaData = Core::Serilization::MetaDataGen
        <Core::Containers::Tuple<Core::Containers::Dictionary<Core::IO::FS::FSObjectInformation>, Core::Containers::List<Core::IO::FS::ObjectProvider::VolumeInformation>, SizeT, Boolean>>();

    auto metaDataDecoded = Core::Serilization::TypeMetaDataRecord::DecodeTypeMetaDataRecord(strMetaData);

    Asserts::AssertEqual(String(Core::CTTI::TypeName
        <Core::Containers::Tuple<Core::Containers::Dictionary<Core::IO::FS::FSObjectInformation>, Core::Containers::List<Core::IO::FS::ObjectProvider::VolumeInformation>, SizeT, Boolean>>()),
        metaDataDecoded.TypeName, ExceptionPosition);

    Asserts::AssertEqual(metaDataDecoded.TypeClass, Core::CTTI::EType::Tuple, ExceptionPosition);

    Asserts::AssertEqual(metaDataDecoded.NestedTypes.LastIt->TypeName, Core::CTTI::TypeName<Boolean>(), ExceptionPosition);
    Asserts::AssertEqual(metaDataDecoded.NestedTypes.LastIt->TypeClass, Core::CTTI::EType::Logic, ExceptionPosition);

    return true;
}

bool Seed::Core::Testing::Test::Serilization::TypeMetaDataRecordSerelize::Excecute()
{
    auto registry = Core::Serilization::TypesRegistry{};
    auto record = Core::Serilization::GetTypeRecordForType<Core::Containers::List<Core::String>>();
    auto serelizedData = Core::Serilization::Serilize(record, registry);
    decltype(record) deserelizedRecord;
    Core::Serilization::Deserilize<decltype(record)>(serelizedData, deserelizedRecord, registry);

    Asserts::AssertEqual(record.TypeClass, deserelizedRecord.TypeClass, ExceptionPosition);
    Asserts::AssertEqual(record.TypeName, deserelizedRecord.TypeName, ExceptionPosition);
    Asserts::AssertEqual(record.SizeOfSize, deserelizedRecord.SizeOfSize, ExceptionPosition);
    Asserts::AssertEqual(record.StaticSize, deserelizedRecord.StaticSize, ExceptionPosition);

    return false;
}
