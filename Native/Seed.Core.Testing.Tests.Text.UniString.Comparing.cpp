#include "Seed.Core.Testing.Tests.Text.UniString.h"

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringConstEquality::Execute()
{
	auto constStr = ConstString(L"Test123");

	auto uniString = Core::UniString(constStr);

	Asserts::AssertEqual(uniString, constStr, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringConstAEquality::Execute()
{
	auto constStr = ConstAString("Test123");

	auto uniString = Core::UniString(constStr);

	Asserts::AssertEqual(uniString, constStr, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringStringEquality::Execute()
{
	auto str = String(L"Test123");

	auto uniString = Core::UniString(str);

	Asserts::AssertEqual(uniString, str, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringTextRefEquality::Execute()
{
	auto uniString = Core::UniString(L"Test123");

	Asserts::AssertEqual(uniString, L"Test123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringTextARefEquality::Execute()
{
	auto uniString = Core::UniString("Test123");

	Asserts::AssertEqual(uniString, "Test123", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringConstArrEquality::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr(L"Test123");

	auto uniString = Core::UniString(cArr);

	Asserts::AssertEqual(uniString, cArr, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Equality::UniStringConstArrACharEquality::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr("Test123");

	auto uniString = Core::UniString(cArr);

	Asserts::AssertEqual(uniString, cArr, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringConstSpaceship::Execute()
{
	auto constStr = ConstString(L"Test123");

	auto constStr1 = ConstString(L"");
	auto constStr2 = ConstString(L"Test1234");

	auto uniString = Core::UniString(constStr);

	Asserts::AssertTrue(uniString > constStr1, SourcePosition);
	Asserts::AssertTrue(uniString < constStr2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringConstASpaceship::Execute()
{
	auto constStr = ConstAString("Test123");

	auto constStr1 = ConstAString("");
	auto constStr2 = ConstAString("Test1234");

	auto uniString = Core::UniString(constStr);

	Asserts::AssertTrue(uniString > constStr1, SourcePosition);
	Asserts::AssertTrue(uniString < constStr2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringStringSpaceship::Execute()
{
	auto str = String(L"Test123");

	auto str1 = String(L"");
	auto str2 = String(L"Test1234");

	auto uniString = Core::UniString(str);

	Asserts::AssertTrue(uniString > str1, SourcePosition);
	Asserts::AssertTrue(uniString < str2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringTextRefSpaceship::Execute()
{
	auto uniString = Core::UniString(L"Test123");

	Asserts::AssertTrue(uniString > L"", SourcePosition);
	Asserts::AssertTrue(uniString < L"Test1234", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringTextARefSpaceship::Execute()
{
	auto uniString = Core::UniString("Test123");

	Asserts::AssertTrue(uniString > "", SourcePosition);
	Asserts::AssertTrue(uniString < "Test1234", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringConstArrSpaceship::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr(L"Test123");

	auto cArr1 = TypeTraits::CreateTypeStr(L"");
	auto cArr2 = TypeTraits::CreateTypeStr(L"Test1235");

	auto uniString = Core::UniString(cArr);

	Asserts::AssertTrue(uniString > cArr1, SourcePosition);
	Asserts::AssertTrue(uniString < cArr2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Comparing::Spaceship::UniStringConstArrACharSpaceship::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr("Test123");

	auto cArr1 = TypeTraits::CreateTypeStr("");
	auto cArr2 = TypeTraits::CreateTypeStr("Test1234");

	auto uniString = Core::UniString(cArr);

	Asserts::AssertTrue(uniString > cArr1, SourcePosition);
	Asserts::AssertTrue(uniString < cArr2, SourcePosition);

	return true;
}