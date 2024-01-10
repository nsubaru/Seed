// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Seed.Core.Functionality.Result.h"

#include "Seed.Core.Testing.Tests.Functionality.Optional.h"

bool Seed::Core::Testing::Test::Functionality::Optional::SetValueL::Execute()
{
    auto obj = Core::Functionality::Optional<String>();

    auto text = String(L"Test");

    obj.SetValue(text);

    Asserts::AssertTrue(obj.HasValue, SourcePosition);
    Asserts::AssertEqual(obj.Value, L"Test", SourcePosition);

    return true;
}

bool Seed::Core::Testing::Test::Functionality::Optional::SetValueR::Execute()
{
    auto obj = Core::Functionality::Optional<String>();

    auto text = String(L"Test");

    obj.SetValue(MoveRValue(text));

    Asserts::AssertTrue(obj.HasValue, SourcePosition);
    Asserts::AssertEqual(obj.Value, L"Test", SourcePosition);

    return true;
}

bool Seed::Core::Testing::Test::Functionality::Optional::SetValueL_WhenValueExist::Execute()
{
    auto obj = Core::Functionality::Optional<String>(L"Test1");

    auto text = String(L"Test");

    obj.SetValue(text);

    Asserts::AssertTrue(obj.HasValue, SourcePosition);
    Asserts::AssertEqual(obj.Value, L"Test", SourcePosition);

    return true;
}

bool Seed::Core::Testing::Test::Functionality::Optional::SetValueR_WhenValueExist::Execute()
{
    auto obj = Core::Functionality::Optional<String>(L"Test1");

    auto text = String(L"Test");

    obj.SetValue(MoveRValue(text));

    Asserts::AssertTrue(obj.HasValue, SourcePosition);
    Asserts::AssertEqual(obj.Value, L"Test", SourcePosition);

    return true;
}

bool Seed::Core::Testing::Test::Functionality::Optional::GetValueL::Execute()
{
    auto obj = Core::Functionality::Optional<String>(L"Test");

    auto value = obj.GetValue();

    Asserts::AssertTrue(obj.HasValue, SourcePosition);
    Asserts::AssertEqual(obj.Value, L"Test", SourcePosition);
    Asserts::AssertEqual(value, L"Test", SourcePosition);

    return true;
}

bool Seed::Core::Testing::Test::Functionality::Optional::GetValueR::Execute()
{
    auto obj = Core::Functionality::Optional<String>(L"Test");

    auto value = MoveRValue(obj).GetValue();

    Asserts::AssertFalse(obj.HasValue, SourcePosition);
    Asserts::AssertEqual(value, L"Test", SourcePosition);

    return true;
}

bool Seed::Core::Testing::Test::Functionality::Optional::GetValueL_WhenValueNotExist::Execute()
{
    auto obj = Core::Functionality::Optional<String>();

    Asserts::AssertThrowException<NullPointException>([&] () {
        auto value = obj.GetValue();
    }, SourcePosition);

    return true;
}

bool Seed::Core::Testing::Test::Functionality::Optional::GetValueR_WhenValueNotExist::Execute()
{
    Asserts::AssertThrowException<NullPointException>([&] () {
        auto obj = Core::Functionality::Optional<String>();

        auto value = MoveRValue(obj).GetValue();
    }, SourcePosition);

    return true;
}