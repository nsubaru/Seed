// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Asserts.h"
#include "Seed.Core.Testing.Tests.Text.String.h"

bool Seed::Core::Testing::Test::Text::StringCtor::Excecute() {
	String str1;
	String str2 = "123";
	String str3 = L"123";
	String str4 = str2;
	String str5 = MoveRValue(String(L"123"));
	String str6 = SensitiveString(L"123");
	return str1.Lenght == 0 && str2.Lenght == 3 && str3.Lenght == 3 && str4.Lenght == 3 && str5.Lenght == 3 && str6.Lenght == 3;
}

bool Seed::Core::Testing::Test::Text::StringContactanation::Excecute() {
	String str1;
	String str2;
	String str3 = "123";
	String str4 = L"123";

	String str5 = str1 + str2; //-V525
	String str6 = str1 + str3;
	String str7 = str3 + str4;
	String str8 = str1 + L"123";
	String str9 = str1 + "123";
	String str10 = str3 + L"123";

	bool EmptySumMustBeEmpty = str5.Lenght == 0 && str5 == L"";
	bool EmptySumWithDataMustBeOnlyData = str6.Lenght == str3.Lenght && str6 == str3 && str6 == L"123";
	bool SumWithDataMustBeAllData = str7.Lenght == str3.Lenght + str4.Lenght && str7.Lenght == 6 && str7 == L"123123";
	bool EmptyWithLiteralMustConatineOnlyDataFromLiteral = str8.Lenght == 3 && str9.Lenght == 3 && str8 == str9 && str8 == L"123";

	str3 += str4;

	bool AddingToThisMustContaineAllData = str3.Lenght == 6 && str3 == L"123123";
	
	str4 += L"123";

	bool AddingLiteralToThisMustContaineAllData = str4.Lenght == 6 && str4 == L"123123";

	String str11 = L"123";

	str11 += str11;

	bool AddingSelfToTisMustContaineAllData = str11.Lenght == 6 && str11 == L"123123";

	return
		EmptySumMustBeEmpty &&
		EmptySumWithDataMustBeOnlyData &&
		SumWithDataMustBeAllData &&
		EmptyWithLiteralMustConatineOnlyDataFromLiteral &&
		AddingToThisMustContaineAllData &&
		AddingLiteralToThisMustContaineAllData &&
		AddingSelfToTisMustContaineAllData;
}

bool Seed::Core::Testing::Test::Text::StringDoubleFloatConvert::Excecute() {
	double Pattern1 = 1.0;
	double Pattern2 = 2.12;
	double Pattern3 = 0.14;
	double Pattern4 = 1.018;
	double Pattern5 = 0.016;

	Asserts::AssertTrue(String::toString(Pattern1, 1) == L"1.0", ExceptionPosition);
	Asserts::AssertTrue(String::toString(Pattern2, 2) == L"2.12", ExceptionPosition);
	Asserts::AssertTrue(String::toString(Pattern3, 2) == L"0.14", ExceptionPosition);
	Asserts::AssertTrue(String::toString(Pattern4, 3) == L"1.018", ExceptionPosition);
	Asserts::AssertTrue(String::toString(Pattern5, 3) == L"0.016", ExceptionPosition);

	Asserts::AssertTrue(Math::Equals(L"1.0"S.toDouble(),	Pattern1), ExceptionPosition);
	Asserts::AssertTrue(Math::Equals(L"2.12"S.toDouble(),	Pattern2), ExceptionPosition);
	Asserts::AssertTrue(Math::Equals(L"0.14"S.toDouble(),	Pattern3), ExceptionPosition);
	Asserts::AssertTrue(Math::Equals(L"1.018"S.toDouble(), Pattern4), ExceptionPosition);
	Asserts::AssertTrue(Math::Equals(L"0.016"S.toDouble(), Pattern5), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringIntConvert::Excecute() {
	int Pattern1 = 1;
	int Pattern2 = 25;
	int Pattern3 = 0;
	int Pattern4 = 105;
	int Pattern5 = 1050;

	Asserts::AssertTrue(String::toString(Pattern1) == L"1", ExceptionPosition);
	Asserts::AssertTrue(String::toString(Pattern2) == L"25", ExceptionPosition);
	Asserts::AssertTrue(String::toString(Pattern3) == L"0", ExceptionPosition);
	Asserts::AssertTrue(String::toString(Pattern4) == L"105", ExceptionPosition);
	Asserts::AssertTrue(String::toString(Pattern5) == L"1050", ExceptionPosition);

	Asserts::AssertTrue(L"1"S.toInt()	== Pattern1, ExceptionPosition);
	Asserts::AssertTrue(L"25"S.toInt()	== Pattern2, ExceptionPosition);
	Asserts::AssertTrue(L"0"S.toInt()	== Pattern3, ExceptionPosition);
	Asserts::AssertTrue(L"105"S.toInt()	== Pattern4, ExceptionPosition);
	Asserts::AssertTrue(L"1050"S.toInt()	== Pattern5, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringSplitChar::Excecute() {
	auto text1 = L"123.123.123"S;

	auto items1 = text1.Split(L'.');

	Asserts::AssertTrue(items1.All(ArrowFunct(x, x == L"123")), ExceptionPosition);

	auto text2 = L"123.123.123."S;

	auto items2 = text2.Split(L'.');

	Asserts::AssertTrue(items2.All(ArrowFunct(x, x == L"123")), ExceptionPosition);

	auto text3 = L"1.1.1.1"S;

	auto items3 = text3.Split(L'.');

	Asserts::AssertTrue(items3.All(ArrowFunct(x, x == L"1")), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringSplitCharLiteral::Excecute() {
	auto text1 = L"123::123::123"S;

	auto items1 = text1.Split(L"::");

	Asserts::AssertTrue(items1.All(ArrowFunct(x, x == L"123")), ExceptionPosition);

	auto text2 = L"123::123::123::"S;

	auto items2 = text2.Split(L"::");

	Asserts::AssertTrue(items2.All(ArrowFunct(x, x == L"123")), ExceptionPosition);

	auto text3 = L"1::1::1::1"S;

	auto items3 = text3.Split(L"::");

	Asserts::AssertTrue(items3.All(ArrowFunct(x, x == L"1")), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringSplitString::Excecute() {
	auto text1 = L"123::123::123"S;

	auto items1 = text1.Split(L"::"S);

	Asserts::AssertTrue(items1.All(ArrowFunct(x, x == L"123")), ExceptionPosition);

	auto text2 = L"123::123::123::"S;

	auto items2 = text2.Split(L"::"S);

	Asserts::AssertTrue(items2.All(ArrowFunct(x, x == L"123")), ExceptionPosition);

	auto text3 = L"1::1::1::1"S;

	auto items3 = text3.Split(L"::"S);

	Asserts::AssertTrue(items3.All(ArrowFunct(x, x == L"1")), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringCtorFromConstArray::Excecute() {
	Asserts::AssertEqual(String(Seed::Core::Details::CreateTypeStr(L"123")), String(L"123"), ExceptionPosition);
	Asserts::AssertEqual(String(Seed::Core::Details::CreateTypeStr(L"123")) + L"4", String(L"1234"), ExceptionPosition);
	Asserts::AssertEqual(String(Seed::Core::Details::CreateTypeStr(L"123")) + L"4"S, String(L"1234"), ExceptionPosition);
	Asserts::AssertEqual(L"1" + String(Seed::Core::Details::CreateTypeStr(L"234")), String(L"1234"), ExceptionPosition);
	Asserts::AssertEqual(L"1"S + String(Seed::Core::Details::CreateTypeStr(L"234")), String(L"1234"), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringLeftResize::Excecute()
{
	auto testStr = L"12345678"S;

	Asserts::AssertEqual(testStr.LeftResize(testStr.Lenght - 2), L"345678"S, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringInt32Hex::Excecute()
{
	Int32 value = 0xabcdef12;

	Asserts::AssertEqual(String::Int32asHex(value), L"0xabcdef12", ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringUInt32Hex::Excecute()
{
	UInt64 value = 0xabcdef1234567891;

	Asserts::AssertEqual(String::UInt64asHex(value), L"0xabcdef1234567891", ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringBlobHex::Excecute()
{
	auto value = Core::Containers::Blob{ 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 };

	Asserts::AssertEqual(String::binaryToHex(value), L"123456789abcdef0", ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Comparing::EmptyStringEquals::Excecute() {
	String str1;
	String str2;
	String str3 = "123";

	Asserts::AssertTrue(str1 == str2, ExceptionPosition);
	Asserts::AssertTrue(str1 != str3, ExceptionPosition);
	Asserts::AssertTrue(!(str1 == str3), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Comparing::SelfStringTestResult::Excecute() {
	String str1;
	String str2 = "123";

	Asserts::AssertTrue(str1 == str1 && str2 == str2, ExceptionPosition);
	Asserts::AssertTrue(!(str1 != str1) && !(str2 != str2), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Comparing::StringWithSameHeapTestResult::Excecute() {
	String str1 = "123";
	String str2 = L"123";
	String str3 = L"1234";

	Asserts::AssertTrue(str1 == str2, ExceptionPosition);
	Asserts::AssertTrue(!(str1 != str2), ExceptionPosition);
	Asserts::AssertTrue(!(str2 == str3), ExceptionPosition);
	Asserts::AssertTrue(str2 != str3, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Comparing::StringWithOtherHeapTestResult::Excecute() {
	String str1 = L"123";
	SensitiveString str2 = "123";
	SensitiveString str3 = "1234";

	Asserts::AssertTrue(str1 == str2 && str2 == str1, ExceptionPosition);
	Asserts::AssertTrue(!(str1 != str2) && !(str2 != str1), ExceptionPosition);
	Asserts::AssertTrue(!(str1 == str3), ExceptionPosition);
	Asserts::AssertTrue(str1 != str3, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Comparing::StringWithConstStringTestResult::Excecute() {
	String str1 = L"123";
	ConstString str2 = L"123";
	ConstString str3 = L"1234";

	Asserts::AssertTrue(str1 == str2 && str2 == str1, ExceptionPosition);
	Asserts::AssertTrue(!(str1 != str2) && !(str2 != str1), ExceptionPosition);
	Asserts::AssertTrue(!(str1 == str3), ExceptionPosition);
	Asserts::AssertTrue(str1 != str3, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Comparing::StringWithConstAStringTestResult::Excecute() {
	String str1 = L"123";
	ConstAString str2 = "123";
	ConstAString str3 = "1234";

	Asserts::AssertTrue(str1 == str2 && str2 == str1, ExceptionPosition);
	Asserts::AssertTrue(!(str1 != str2) && !(str2 != str1), ExceptionPosition);
	Asserts::AssertTrue(!(str1 == str3), ExceptionPosition);
	Asserts::AssertTrue(str1 != str3, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Comparing::StringWithLiteralTestResult::Excecute() {
	String str1 = L"123";

	Asserts::AssertTrue(L"123" == str1 && str1 == L"123", ExceptionPosition);
	Asserts::AssertTrue(!(L"123" != str1) && !(str1 != L"123"), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Comparing::StringWithAciiLiteralTestResult::Excecute() {
	String str1 = L"123";

	Asserts::AssertTrue("123" == str1 && str1 == "123", ExceptionPosition);
	Asserts::AssertTrue(!("123" != str1) && !(str1 != "123"), ExceptionPosition);

	return true;
}


bool Seed::Core::Testing::Test::Ordering::ComparingEmptyStrings::Excecute() {
	String str1;
	String str2;

	Asserts::AssertTrue(!(str1 > str2) && !(str1 < str2) && str1 <= str2 && str1 >= str2, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithEmptyString::Excecute() {
	String str1;
	String str2 = "123";

	Asserts::AssertTrue(str1 < str2 && !(str1 > str2) && str1 <= str2 && !(str2 <= str1), ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingSameDataStrings::Excecute() {
	String str1 = "123";
	String str2 = L"123";

	Asserts::AssertTrue(!(str1 < str2) && !(str1 > str2) && str1 <= str2 && str1 >= str2, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithSelf::Excecute() {
	String str1 = "123";

	Asserts::AssertTrue(!(str1 < str1) && !(str1 > str1) && str1 <= str1 && str1 >= str1, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithOtherHeapString::Excecute() {
	String str1 = L"123";
	SensitiveString str2 = "123";

	Asserts::AssertTrue(!(str1 < str2) && !(str1 > str2) && str1 <= str2 && str1 >= str2, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithConstString::Excecute() {
	String str1 = L"123";
	ConstString str2 = L"123";

	Asserts::AssertTrue(!(str1 < str2) && !(str1 > str2) && str1 <= str2 && str1 >= str2, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithConstAString::Excecute() {
	String str1 = L"123";
	ConstAString str2 = "123";

	Asserts::AssertTrue(!(str1 < str2), ExceptionPosition);
	Asserts::AssertTrue(!(str1 > str2), ExceptionPosition);
	Asserts::AssertTrue(str1 <= str2, ExceptionPosition);
	Asserts::AssertTrue(str1 >= str2, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithLiteral::Excecute() {
	String str1 = L"123";

	Asserts::AssertTrue(!(str1 < L"123") && !(str1 > L"123") && str1 <= L"123" && str1 >= L"123", ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithAsciiLiteral::Excecute() {
	String str1 = L"123";

	Asserts::AssertTrue(!(str1 < "123") && !(str1 > "123") && str1 <= "123" && str1 >= "123", ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithLowerString::Excecute() {
	String str1 = L"123";
	String str2 = L"1234";

	Asserts::AssertTrue(str1 < str2&& str2 > str1 && str1 <= str2 && str2 >= str1, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithLowerOtherHeapString::Excecute() {
	String str1 = L"123";
	SensitiveString str2 = "1234";

	Asserts::AssertTrue(str1 < str2&& str2 > str1 && str1 <= str2 && str2 >= str1, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithLowerConstString::Excecute() {
	String str1 = L"123";
	ConstString str2 = L"1234";

	Asserts::AssertTrue(str1 < str2, ExceptionPosition);
	Asserts::AssertTrue(String(str2) > str1, ExceptionPosition);
	Asserts::AssertTrue(str1 <= str2, ExceptionPosition);
	Asserts::AssertTrue(String(str2) >= str1, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithLowerConstAString::Excecute() {
	String str1 = L"123";
	ConstAString str2 = "1234";

	Asserts::AssertTrue(str1 < str2, ExceptionPosition);
	Asserts::AssertTrue(String(str2) > str1, ExceptionPosition);
	Asserts::AssertTrue(str1 <= str2, ExceptionPosition);
	Asserts::AssertTrue(String(str2) >= str1, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::ComparingWithLowerLiteral::Excecute() {
	String str1 = L"123";

	Asserts::AssertTrue(str1 < L"1234", ExceptionPosition);
	Asserts::AssertTrue(String(L"1234") > str1, ExceptionPosition);
	Asserts::AssertTrue(str1 <= L"1234", ExceptionPosition);
	Asserts::AssertTrue(String(L"1234") >= str1, ExceptionPosition);

	return true;
}

bool Seed::Core::Testing::Test::Ordering::CompatingWithLowerAciiLiteral::Excecute() {
	String str1 = L"123";

	Asserts::AssertTrue(str1 < "1234", ExceptionPosition);
	Asserts::AssertTrue(String("1234") > str1, ExceptionPosition);
	Asserts::AssertTrue(str1 <= "1234", ExceptionPosition);
	Asserts::AssertTrue(String("1234") >= str1, ExceptionPosition);

	return true;
}