#include "Seed.Core.Testing.Tests.Text.UniString.h"

constexpr auto testText = L"Test123";

bool Seed::Core::Testing::Test::UniString::Creation::EmptyUniStringCreation::Execute()
{
	auto uniString = Core::UniString();

	Asserts::AssertEqual(uniString.Length, 0, SourcePosition);
	Asserts::AssertTrue(uniString.IsConstMode, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::UniString::Creation::ConstStringUniStringCreation::Execute()
{
	auto cStr = ConstString(L"Test123");

	auto uniString = Core::UniString(cStr);

	Asserts::AssertEqual(uniString.Length, cStr.Length, SourcePosition);
	Asserts::AssertTrue(uniString.IsConstMode, SourcePosition);

	for (SizeT i = 0; i < uniString.Length; i++)
	{
		Asserts::AssertEqual(uniString[i], testText[i], SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::UniString::Creation::StringUniStringCreation::Execute()
{
	auto str = String(L"Test123");

	auto uniString = Core::UniString(str);

	Asserts::AssertEqual(uniString.Length, str.Length, SourcePosition);
	Asserts::AssertTrue(!uniString.IsConstMode, SourcePosition);

	for (SizeT i = 0; i < uniString.Length; i++)
	{
		Asserts::AssertEqual(uniString[i], testText[i], SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::UniString::Creation::TextRefUniStringCreation::Execute()
{
	auto uniString = Core::UniString(L"Test123");

	Asserts::AssertEqual(uniString.Length, sizeof(testText) - 1, SourcePosition);
	Asserts::AssertTrue(uniString.IsConstMode, SourcePosition);

	for (SizeT i = 0; i < uniString.Length; i++)
	{
		Asserts::AssertEqual(uniString[i], testText[i], SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::UniString::Creation::ConstArrUniStringCreation::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr(L"Test123");

	auto uniString = Core::UniString(cArr);

	Asserts::AssertEqual(uniString.Length, cArr.Count, SourcePosition);
	Asserts::AssertTrue(uniString.IsConstMode, SourcePosition);

	for (SizeT i = 0; i < uniString.Length; i++)
	{
		Asserts::AssertEqual(uniString[i], testText[i], SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::UniString::Creation::ConstArrACharUniStringCreation::Execute()
{
	auto cArr = TypeTraits::CreateTypeStr("Test123");

	auto uniString = Core::UniString(cArr);

	Asserts::AssertEqual(uniString.Length, cArr.Count, SourcePosition);
	Asserts::AssertTrue(uniString.IsConstMode, SourcePosition);

	for (SizeT i = 0; i < uniString.Length; i++)
	{
		Asserts::AssertEqual(uniString[i], testText[i], SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::UniString::Creation::ConstAStringUniStringCreation::Execute()
{
	auto cStr = ConstAString("Test123");

	auto uniString = Core::UniString(cStr);

	Asserts::AssertEqual(uniString.Length, cStr.Length, SourcePosition);
	Asserts::AssertTrue(uniString.IsConstMode, SourcePosition);

	for (SizeT i = 0; i < uniString.Length; i++)
	{
		Asserts::AssertEqual(uniString[i], testText[i], SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::UniString::Creation::TextARefUniStringCreation::Execute()
{
	auto uniString = Core::UniString("Test123");

	Asserts::AssertEqual(uniString.Length, sizeof(testText) - 1, SourcePosition);
	Asserts::AssertTrue(uniString.IsConstMode, SourcePosition);

	for (SizeT i = 0; i < uniString.Length; i++)
	{
		Asserts::AssertEqual(uniString[i], testText[i], SourcePosition);
	}

	return true;
}