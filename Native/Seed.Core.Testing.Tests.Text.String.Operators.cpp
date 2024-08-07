// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Asserts.h"
#include "Seed.Core.Testing.Tests.Text.String.h"

#pragma region AdditionToEmptyString
bool Seed::Core::Testing::Test::Text::String::Operators::AdditionStringLiteralToEmptyStringTest::Execute() {
	Core::String str;
	str += L"";

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += L"\n";

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += L"123";

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	str = Core::String(L"123");
	str += L"";

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionStringALiteralToEmptyStringTest::Execute() {
	Core::String str;
	str += "";

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += "\n";

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += "123";

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	str = Core::String(L"123");
	str += "";

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionConstStringToEmptyStringTest::Execute() {
	Core::String str;
	str += Core::ConstString(L"");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += Core::ConstString(L"\n");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += Core::ConstString(L"123");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	str = Core::String(L"123");
	str += Core::ConstString(L"");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionConstAStringToEmptyStringTest::Execute() {
	Core::String str;
	str += Core::ConstAString("");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += Core::ConstAString("\n");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += Core::ConstAString("123");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	str = Core::String(L"123");
	str += Core::ConstAString("");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionStringWithOtherHeapToEmptyStringTest::Execute() {
	Core::String str;
	str += Core::SensitiveString(L"");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += Core::SensitiveString(L"\n");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += Core::SensitiveString(L"123");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	str = Core::String(L"123");
	str += Core::SensitiveString(L"");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionConstExprStringToEmptyStringTest::Execute() {
	Core::String str;
	str += TypeTraits::CreateTypeStr(L"");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += TypeTraits::CreateTypeStr(L"\n");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += TypeTraits::CreateTypeStr(L"123");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	str = Core::String(L"123");
	str += TypeTraits::CreateTypeStr(L"");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionConstExprAStringToEmptyStringTest::Execute() {
	Core::String str;
	str += TypeTraits::CreateTypeStr("");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += TypeTraits::CreateTypeStr("\n");

	Asserts::AssertEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(str.Length, 0, SourcePosition);

	str = Core::String();
	str += TypeTraits::CreateTypeStr("123");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	str = Core::String(L"123");
	str += TypeTraits::CreateTypeStr("");

	Asserts::AssertNotEqual((ConstCharStr) str, nullptr, SourcePosition);
	Asserts::AssertEqual(str.Size, 8b, SourcePosition);
	Asserts::AssertEqual(str.Length, 3, SourcePosition);

	Asserts::AssertEqual(str, L"123", SourcePosition);

	return true;
}
#pragma endregion

#pragma region SummationToEmptyString
bool Seed::Core::Testing::Test::Text::String::Operators::SummationStringLiteralToEmptyStringTest::Execute() {
	Core::String str;
	Core::String res = str + L"";

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + L"\n";

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + L"123";

	Asserts::AssertNotEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 8b, SourcePosition);
	Asserts::AssertEqual(res.Length, 3, SourcePosition);

	Asserts::AssertEqual(res, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationStringALiteralToEmptyStringTest::Execute() {
	Core::String str;
	Core::String res = str + "";

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + "\n";

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + "123";

	Asserts::AssertNotEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 8b, SourcePosition);
	Asserts::AssertEqual(res.Length, 3, SourcePosition);

	Asserts::AssertEqual(res, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationConstStringToEmptyStringTest::Execute() {
	Core::String str;
	Core::String res = str + Core::ConstString(L"");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + Core::ConstString(L"\n");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + Core::ConstString(L"123");

	Asserts::AssertNotEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 8b, SourcePosition);
	Asserts::AssertEqual(res.Length, 3, SourcePosition);

	Asserts::AssertEqual(res, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationConstAStringToEmptyStringTest::Execute() {
	Core::String str;
	Core::String res = str + Core::ConstAString("");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + Core::ConstAString("\n");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + Core::ConstAString("123");

	Asserts::AssertNotEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 8b, SourcePosition);
	Asserts::AssertEqual(res.Length, 3, SourcePosition);

	Asserts::AssertEqual(res, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationStringWithOtherHeapToEmptyStringTest::Execute() {
	Core::String str;
	Core::String res = str + Core::SensitiveString(L"");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + Core::SensitiveString(L"\n");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + Core::SensitiveString(L"123");

	Asserts::AssertNotEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 8b, SourcePosition);
	Asserts::AssertEqual(res.Length, 3, SourcePosition);

	Asserts::AssertEqual(res, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationConstExprStringToEmptyStringTest::Execute() {
	Core::String str;
	Core::String res = str + TypeTraits::CreateTypeStr(L"");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + TypeTraits::CreateTypeStr(L"\n");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + TypeTraits::CreateTypeStr(L"123");

	Asserts::AssertNotEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 8b, SourcePosition);
	Asserts::AssertEqual(res.Length, 3, SourcePosition);

	Asserts::AssertEqual(res, L"123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationConstExprAStringToEmptyStringTest::Execute() {
	Core::String str;
	Core::String res = str + TypeTraits::CreateTypeStr("");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + TypeTraits::CreateTypeStr("\n");

	Asserts::AssertEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 0Kb, SourcePosition);
	Asserts::AssertEqual(res.Length, 0, SourcePosition);

	res = str + TypeTraits::CreateTypeStr("123");

	Asserts::AssertNotEqual((ConstCharStr) res, nullptr, SourcePosition);
	Asserts::AssertEqual(res.Size, 8b, SourcePosition);
	Asserts::AssertEqual(res.Length, 3, SourcePosition);

	Asserts::AssertEqual(res, L"123", SourcePosition);

	return true;
}
#pragma endregion

#pragma region AdditionToString
bool Seed::Core::Testing::Test::Text::String::Operators::AdditionStringLiteralToStringTest::Execute() {
	Core::String str = L"123";
	str += L"123";

	Asserts::AssertEqual(str, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionStringALiteralToStringTest::Execute() {
	Core::String str = L"123";
	str += "123";

	Asserts::AssertEqual(str, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionConstStringToStringTest::Execute() {
	Core::String str = L"123";
	str += Core::ConstString(L"123");

	Asserts::AssertEqual(str, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionConstAStringToStringTest::Execute() {
	Core::String str = L"123";
	str += Core::ConstAString("123");

	Asserts::AssertEqual(str, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionStringWithOtherHeapToStringTest::Execute() {
	Core::String str = L"123";
	str += Core::SensitiveString(L"123");

	Asserts::AssertEqual(str, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionConstExprStringToStringTest::Execute() {
	Core::String str = L"123";
	str += TypeTraits::CreateTypeStr(L"123");

	Asserts::AssertEqual(str, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::AdditionConstExprAStringToStringTest::Execute() {
	Core::String str = L"123";
	str += TypeTraits::CreateTypeStr("123");

	Asserts::AssertEqual(str, L"123123", SourcePosition);

	return true;
}
#pragma endregion

#pragma region SummationToString
bool Seed::Core::Testing::Test::Text::String::Operators::SummationStringLiteralToStringTest::Execute() {
	Core::String str = L"123";
	Core::String res = str + L"123";

	Asserts::AssertEqual(res, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationStringALiteralToStringTest::Execute() {
	Core::String str = Core::String(L"123");
	Core::String res = str + "123";

	Asserts::AssertEqual(res, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationConstStringToStringTest::Execute() {
	Core::String str = Core::String(L"123");
	Core::String res = str + Core::ConstString(L"123");

	Asserts::AssertEqual(res, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationConstAStringToStringTest::Execute() {
	Core::String str = Core::String(L"123");
	Core::String res = str + Core::ConstAString("123");

	Asserts::AssertEqual(res, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationStringWithOtherHeapToStringTest::Execute() {
	Core::String str = Core::String(L"123");
	Core::String res = str + Core::SensitiveString(L"123");

	Asserts::AssertEqual(res, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationConstExprStringToStringTest::Execute() {
	Core::String str = Core::String(L"123");
	Core::String res = str + TypeTraits::CreateTypeStr(L"123");

	Asserts::AssertEqual(res, L"123123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::String::Operators::SummationConstExprAStringToStringTest::Execute() {
	Core::String str = Core::String(L"123");
	Core::String res = str + TypeTraits::CreateTypeStr("123");

	Asserts::AssertEqual(res, L"123123", SourcePosition);

	return true;
}
#pragma endregion