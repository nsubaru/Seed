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