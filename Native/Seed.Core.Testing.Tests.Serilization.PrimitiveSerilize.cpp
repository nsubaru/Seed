// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Serilization.PrimitiveSerilize.h"

bool Seed::Core::Testing::Test::Serilization::LogicSerilization::Excecute() {
    Boolean Value = true;
    auto stream = Core::Serilization::Serilize(Value);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue);
    return Value == deserelizedValue;
}

bool Seed::Core::Testing::Test::Serilization::NumericSerilization::Excecute() {
    Int32 Value = 1;
    auto stream = Core::Serilization::Serilize(Value);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue);
    return Value == deserelizedValue;
}

bool Seed::Core::Testing::Test::Serilization::FloatPointSerilization::Excecute() {
    Float Value = 1;
    auto stream = Core::Serilization::Serilize(Value);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue);
    return Value == deserelizedValue; //-V550
}

bool Seed::Core::Testing::Test::Serilization::CharacterSerilization::Excecute() {
    Char Value = 1;
    auto stream = Core::Serilization::Serilize(Value);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue);
    return Value == deserelizedValue;
}

bool Seed::Core::Testing::Test::Serilization::CFixedArraySerilization::Excecute() {
    Char Value[] = L"Seed::Core::Testing::Test::Serilization::CFixedArraySerilization::Excecute";
    auto stream = Core::Serilization::Serilize(Value);
    decltype(Value) deserelizedValue;
    Core::Serilization::Deserilize(stream, deserelizedValue);
    return CStr::Compare(Value, deserelizedValue);
}

bool Seed::Core::Testing::Test::Serilization::PrimitiveMetaData::Excecute() {
    auto MetaData = Core::Serilization::MetaDataGen<Int32[10][10][10]>();
    return true;
}
