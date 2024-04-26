#include "Seed.Core.Testing.Tests.Text.UniString.h"

using namespace Seed::Core;
using namespace Seed;

static bool DifTestEqStart(const Seed::Core::UniString& testObj)
{
	static auto constStr = ConstString(L"Test123");
	static auto aconstStr = ConstAString("Test123");

	static auto str = String(L"Test123");
	static auto astr = String("Test123");

	static auto cArr = TypeTraits::CreateTypeStr(L"Test123");
	static auto acArr = TypeTraits::CreateTypeStr("Test123");

	Asserts::AssertEqual(testObj, constStr, SourcePosition);
	Asserts::AssertEqual(testObj, aconstStr, SourcePosition);
	Asserts::AssertEqual(testObj, str, SourcePosition);
	Asserts::AssertEqual(testObj, astr, SourcePosition);
	Asserts::AssertEqual(testObj, cArr, SourcePosition);
	Asserts::AssertEqual(testObj, acArr, SourcePosition);
	Asserts::AssertEqual(testObj, L"Test123", SourcePosition);
	Asserts::AssertEqual(testObj, "Test123", SourcePosition);

	return true;
}

static bool DifTestSSStart(const Seed::Core::UniString& testObj)
{
	static auto constStr1 = ConstString(L"");
	static auto constStr2 = ConstString(L"Test");
	static auto constStr3 = ConstString(L"Test1234");

	static auto aconstStr1 = ConstAString("");
	static auto aconstStr2 = ConstAString("Test");
	static auto aconstStr3 = ConstAString("Test1234");

	static auto str1 = String(L"");
	static auto str2 = String(L"Test");
	static auto str3 = String(L"Test1234");

	static auto astr1 = String("");
	static auto astr2 = String("Test");
	static auto astr3 = String("Test1234");

	static auto cArr1 = TypeTraits::CreateTypeStr(L"");
	static auto cArr2 = TypeTraits::CreateTypeStr(L"Test");
	static auto cArr3 = TypeTraits::CreateTypeStr(L"Test1234");

	static auto acArr1 = TypeTraits::CreateTypeStr("");
	static auto acArr2 = TypeTraits::CreateTypeStr("Test");
	static auto acArr3 = TypeTraits::CreateTypeStr("Test1234");

	Asserts::AssertTrue(testObj > constStr1, SourcePosition);
	Asserts::AssertTrue(testObj > constStr2, SourcePosition);
	Asserts::AssertTrue(testObj < constStr3, SourcePosition);

	Asserts::AssertTrue(testObj > aconstStr1, SourcePosition);
	Asserts::AssertTrue(testObj > aconstStr2, SourcePosition);
	Asserts::AssertTrue(testObj < aconstStr3, SourcePosition);

	Asserts::AssertTrue(testObj > str1, SourcePosition);
	Asserts::AssertTrue(testObj > str2, SourcePosition);
	Asserts::AssertTrue(testObj < str3, SourcePosition);

	Asserts::AssertTrue(testObj > astr1, SourcePosition);
	Asserts::AssertTrue(testObj > astr2, SourcePosition);
	Asserts::AssertTrue(testObj < astr3, SourcePosition);

	Asserts::AssertTrue(testObj > cArr1, SourcePosition);
	Asserts::AssertTrue(testObj > cArr2, SourcePosition);
	Asserts::AssertTrue(testObj < cArr3, SourcePosition);

	Asserts::AssertTrue(testObj > acArr1, SourcePosition);
	Asserts::AssertTrue(testObj > acArr2, SourcePosition);
	Asserts::AssertTrue(testObj < acArr3, SourcePosition);

	Asserts::AssertTrue(testObj > L"", SourcePosition);
	Asserts::AssertTrue(testObj > L"Test", SourcePosition);
	Asserts::AssertTrue(testObj < L"Test1234", SourcePosition);

	Asserts::AssertTrue(testObj > "", SourcePosition);
	Asserts::AssertTrue(testObj > "Test", SourcePosition);
	Asserts::AssertTrue(testObj < "Test1234", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringConstDifEquality::Execute()
{
	auto constStr = ConstString(L"Test123");

	auto uniString = Core::UniString(constStr);

	return DifTestEqStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringConstADifEquality::Execute()
{
	auto constStr = ConstAString("Test123");

	auto uniString = Core::UniString(constStr);

	return DifTestEqStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringStringDifEquality::Execute()
{
	auto str = String(L"Test123");

	auto uniString = Core::UniString(str);

	return DifTestEqStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringTextRefDifEquality::Execute()
{
	auto uniString = Core::UniString(L"Test123");

	return DifTestEqStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringTextARefDifEquality::Execute()
{
	auto uniString = Core::UniString("Test123");

	return DifTestEqStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringConstArrDifEquality::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr(L"Test123");

	auto uniString = Core::UniString(cArr);

	return DifTestEqStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringConstArrACharDifEquality::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr("Test123");

	auto uniString = Core::UniString(cArr);

	return DifTestEqStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringConstDifSpaceship::Execute()
{
	auto constStr = ConstString(L"Test123");

	auto uniString = Core::UniString(constStr);

	return DifTestSSStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringConstADifSpaceship::Execute()
{
	auto constStr = ConstAString("Test123");

	auto uniString = Core::UniString(constStr);

	return DifTestSSStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringStringDifSpaceship::Execute()
{
	auto str = String(L"Test123");

	auto uniString = Core::UniString(str);

	return DifTestSSStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringTextRefDifSpaceship::Execute()
{
	auto uniString = Core::UniString(L"Test123");

	return DifTestSSStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringTextARefDifSpaceship::Execute()
{
	auto uniString = Core::UniString("Test123");

	return DifTestSSStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringConstArrDifSpaceship::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr(L"Test123");

	auto uniString = Core::UniString(cArr);

	return DifTestSSStart(uniString);
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringConstArrACharDifSpaceship::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr("Test123");

	auto uniString = Core::UniString(cArr);

	return DifTestSSStart(uniString);
}