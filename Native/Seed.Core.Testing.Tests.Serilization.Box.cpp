// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Serilization.Box.h"

#include "Seed.Core.Serilization.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Reflection.h"

bool Seed::Core::Testing::Test::Serilization::LazySerilization::Excecute() {
    auto registry = Core::Serilization::TypesRegistry{};

    auto data = Lazy<String>(ArrowFunct(String(L"" __FUNCSIG__)));
    auto serilizedData = Core::Serilization::Serilize(data, registry);
    decltype(data) deserilizedData = Lazy<String>();
    Core::Serilization::Deserilize(serilizedData, deserilizedData, registry);
    Asserts::AssertEqual(*data, *deserilizedData);

    return true;
}

bool Seed::Core::Testing::Test::Serilization::SmartPtrSerilization::Excecute() {
    auto registry = Core::Serilization::TypesRegistry{};

    auto data = SmartPtr<String>(new String(L"" __FUNCSIG__));
    auto serilizedData = Core::Serilization::Serilize(data, registry);
    decltype(data) deserilizedData = SmartPtr<String>();
    Core::Serilization::Deserilize(serilizedData, deserilizedData, registry);
    Asserts::AssertEqual(*data, *deserilizedData);

    return true;
}
