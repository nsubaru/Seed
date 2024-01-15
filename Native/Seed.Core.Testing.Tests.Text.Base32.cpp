// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Text.Base32.h"

#include "Seed.Core.Text.Base32.h"

bool Seed::Core::Testing::Test::Text::Base32::RFCBase32Test1::Execute()
{
	auto result = Core::Text::Base32::Encode(Containers::Blob());

	Asserts::AssertEqual(result, L"", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCBase32Test2::Execute()
{
	auto testData = Containers::Blob({ 'f' });

	auto result = Core::Text::Base32::Encode(testData);

	Asserts::AssertEqual(result, L"MY======", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCBase32Test3::Execute()
{
	auto testData = Containers::Blob({ 'f', 'o' });

	auto result = Core::Text::Base32::Encode(testData);

	Asserts::AssertEqual(result, L"MZXQ====", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCBase32Test4::Execute()
{
	auto testData = Containers::Blob({ 'f', 'o', 'o' });

	auto result = Core::Text::Base32::Encode(testData);

	Asserts::AssertEqual(result, L"MZXW6===", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCBase32Test5::Execute()
{
	auto testData = Containers::Blob({ 'f', 'o', 'o', 'b' });

	auto result = Core::Text::Base32::Encode(testData);

	Asserts::AssertEqual(result, L"MZXW6YQ=", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCBase32Test6::Execute()
{
	auto testData = Containers::Blob({ 'f', 'o', 'o', 'b', 'a' });

	auto result = Core::Text::Base32::Encode(testData);

	Asserts::AssertEqual(result, L"MZXW6YTB", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCBase32Test7::Execute()
{
	auto testData = Containers::Blob({ 'f', 'o', 'o', 'b', 'a', 'r' });

	auto result = Core::Text::Base32::Encode(testData);

	Asserts::AssertEqual(result, L"MZXW6YTBOI======", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCTest1Base32::Execute()
{
	auto testData = L"MY======"S;

	auto result = Core::Text::Base32::Decode(testData);

	Asserts::AssertEqual(result, Containers::Blob({ 'f' }), SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCTest2Base32::Execute()
{
	auto testData = L"MZXQ===="S;

	auto result = Core::Text::Base32::Decode(testData);

	Asserts::AssertEqual(result, Containers::Blob({ 'f', 'o' }), SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCTest3Base32::Execute()

{
	auto testData = L"MZXW6==="S;

	auto result = Core::Text::Base32::Decode(testData);

	Asserts::AssertEqual(result, Containers::Blob({ 'f', 'o', 'o' }), SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCTest4Base32::Execute()
{
	auto testData = L"MZXW6YQ="S;

	auto result = Core::Text::Base32::Decode(testData);

	Asserts::AssertEqual(result, Containers::Blob({ 'f', 'o', 'o', 'b' }), SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCTest5Base32::Execute()
{
	auto testData = L"MZXW6YTB"S;

	auto result = Core::Text::Base32::Decode(testData);

	Asserts::AssertEqual(result, Containers::Blob({ 'f', 'o', 'o', 'b', 'a' }), SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::Base32::RFCTest6Base32::Execute()
{
	auto testData = L"MZXW6YTBOI======"S;

	auto result = Core::Text::Base32::Decode(testData);

	Asserts::AssertEqual(result, Containers::Blob({ 'f', 'o', 'o', 'b', 'a', 'r' }), SourcePosition);

	return true;
}