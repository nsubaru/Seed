// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Serilization.PrimitiveSerilize.h"
#include "Seed.Core.Serilization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Reflection.h"

bool Seed::Core::Testing::Test::Serilization::LogicSerilization::Excecute() {
    auto registry = Core::Serilization::TypesRegistry();

    Boolean Value = true;
    auto stream = Core::Serilization::Serilize(Value, registry);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue, registry);
    return Value == deserelizedValue;
}

bool Seed::Core::Testing::Test::Serilization::NumericSerilization::Excecute() {
    auto registry = Core::Serilization::TypesRegistry();

    Int32 Value = 1;
    auto stream = Core::Serilization::Serilize(Value, registry);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue, registry);
    return Value == deserelizedValue;
}

bool Seed::Core::Testing::Test::Serilization::FloatPointSerilization::Excecute() {
    auto registry = Core::Serilization::TypesRegistry();

    Float Value = 1;
    auto stream = Core::Serilization::Serilize(Value, registry);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue, registry);
    return Value == deserelizedValue; //-V550 //-V2550
}

bool Seed::Core::Testing::Test::Serilization::CharacterSerilization::Excecute() {
    auto registry = Core::Serilization::TypesRegistry();

    Char Value = 1;
    auto stream = Core::Serilization::Serilize(Value, registry);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue, registry);
    return Value == deserelizedValue;
}

bool Seed::Core::Testing::Test::Serilization::CFixedArraySerilization::Excecute() {
    auto registry = Core::Serilization::TypesRegistry();

    Char Value[] = L"Seed::Core::Testing::Test::Serilization::CFixedArraySerilization::Excecute";
    auto stream = Core::Serilization::Serilize(Value, registry);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue, registry);
    return CStr::Compare(Value, deserelizedValue);
}

bool Seed::Core::Testing::Test::Serilization::PrimitiveMetaData::Excecute() {
    auto registry = Core::Serilization::TypesRegistry();

    auto MetaData = Core::Serilization::MetaDataGen<Int32[10][10][10]>();
    return true;
}

bool Seed::Core::Testing::Test::Serilization::DataSizeTSerilization::Excecute()
{
    auto registry = Core::Serilization::TypesRegistry();

    auto value = 10Gb;

    auto stream = Core::Serilization::Serilize(value, registry);
    decltype(value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue, registry);

    Asserts::AssertEqual(value, deserelizedValue);

    return true;
}

bool Seed::Core::Testing::Test::Serilization::DataSizeTMetaData::Excecute()
{
    Asserts::AssertEqual(CTTI::TypeTraits::CTTITypeMetaData<Memory::DataSizeT>::TypeClass, CTTI::EType::Numeric);
    Asserts::AssertTrue(CTTI::TypeTraits::CTTITypeMetaData<Memory::DataSizeT>::IsSerilizable);

    return true;
}
