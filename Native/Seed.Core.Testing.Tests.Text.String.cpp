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

bool Seed::Core::Testing::Test::Text::StringComparing::Excecute() {
	String str1;
	String str2;
	String str3 = "123";
	String str4 = L"123";
	SensitiveString str5 = "123";
	ConstAString str6 = "123";
	ConstString str7 = L"123";
	String str8 = L"1234";
	SensitiveString str9 = "1234";
	ConstString str10 = L"1234";
	ConstAString str11 = "1234";

	bool EmptyStringEquals = str1 == str2;
	bool EmptyStringNotEqualNonEmpty = str1 != str3;
	bool EmptyStringNotNotEqualNonEmpty = !(str1 == str3);

	bool StringSelfEqual = str1 == str1 && str3 == str3; //-V501
	bool StringSelfNotNotEqual = !(str1 != str1) && !(str3 != str3); //-V501
	
	bool StringEqualOtherWithSameData = str3 == str4;
	bool StringNotNotEqualOtherWithSameData = !(str3 != str4);
	bool StringNotEqualOtherWithOtherData = !(str4 == str8);
	bool StringNotNotEqualOtherWithOtherData = str4 != str8;

	bool StringEqualOtherHeapStringWithSameData = str4 == str5 && str5 == str4; //-V501
	bool StringNotNotEqualOtherHeapStringWithSameData = !(str4 != str5) && !(str5 != str4); //-V501
	bool StringNotEqualOtherHeapStringWithOtherData = !(str4 == str9);
	bool StringNotNotEauqlOtherHeapStringWithOtherData = str4 != str9;

	bool StringEqualConstStringWithSameData = str4 == str7 && str7 == str4; //-V501
	bool StringNotNotEqualConstStringWithSameData = !(str4 != str7) && !(str7 != str4); //-V501
	bool StringNotEqualConstStringWithOtherData = !(str4 == str10);
	bool StringNotNotEqualConstStringWithOtherData = str4 != str10;

	bool StringEqualConstAStringWithSameData = str4 == str6 && str6 == str4; //-V501
	bool StringNotNotEqualConstAStringWithSameData = !(str4 != str6) && !(str6 != str4); //-V501
	bool StringNotEqualConstAStringWithOtherData = !(str4 == str11);
	bool StringNotNotEqualConstAStringWithOtherData = str4 != str11;

	bool StringEqualLiteralWithSameData = L"123" == str4 && str4 == L"123"; //-V501
	bool StringNotNotEqualLiteralWithSameData = !(L"123" != str4) && !(str4 != L"123"); //-V501

	bool StringEqualAsciiLiteralWithSameData = "123" == str4 && str4 == "123"; //-V501
	bool StringNotNotEqualAsciiLiteralWithSameData = !("123" != str4) && !(str4 != "123"); //-V501

	bool EmptyStringTestResult = 
		EmptyStringEquals && 
		EmptyStringNotEqualNonEmpty && 
		EmptyStringNotNotEqualNonEmpty;
	bool SelfStringTestResult = 
		StringSelfEqual && 
		StringSelfNotNotEqual;
	bool StringWithSameHeapTestResult = 
		StringEqualOtherWithSameData && 
		StringNotNotEqualOtherWithSameData && 
		StringNotEqualOtherWithOtherData && 
		StringNotNotEqualOtherWithOtherData;
	bool StringWithOtherHeapTestResult =
		StringEqualOtherHeapStringWithSameData &&
		StringNotNotEqualOtherHeapStringWithSameData &&
		StringNotEqualOtherHeapStringWithOtherData &&
		StringNotNotEauqlOtherHeapStringWithOtherData;
	bool StringWithConstStrinTestResult =
		StringEqualConstStringWithSameData &&
		StringNotNotEqualConstStringWithSameData &&
		StringNotEqualConstStringWithOtherData &&
		StringNotNotEqualConstStringWithOtherData;
	bool StringWithConstAStrinTestResult =
		StringEqualConstAStringWithSameData &&
		StringNotNotEqualConstAStringWithSameData &&
		StringNotEqualConstAStringWithOtherData &&
		StringNotNotEqualConstAStringWithOtherData;
	bool StringWithLiteralTestResult =
		StringEqualLiteralWithSameData &&
		StringNotNotEqualLiteralWithSameData;
	bool StringWithAciiLiteralTestResult =
		StringEqualAsciiLiteralWithSameData &&
		StringNotNotEqualAsciiLiteralWithSameData;

	return
		EmptyStringTestResult &&
		SelfStringTestResult &&
		StringWithSameHeapTestResult &&
		StringWithOtherHeapTestResult &&
		StringWithConstStrinTestResult &&
		StringWithConstAStrinTestResult &&
		StringWithLiteralTestResult &&
		StringWithAciiLiteralTestResult;
}

bool Seed::Core::Testing::Test::Text::StringOrdering::Excecute() {
	String str1;
	String str2;
	String str3 = "123";
	String str4 = L"123";
	SensitiveString str5 = "123";
	ConstAString str6 = "123";
	ConstString str7 = L"123";
	String str8 = L"1234";
	SensitiveString str9 = "1234";
	ConstString str10 = L"1234";
	ConstAString str11 = "1234";

	bool ComparingEmptyStrings = !(str1 > str2) && !(str1 < str2) && str1 <= str2 && str1 >= str2;
	bool ComparingWithEmptyString = str2 < str3 && !(str2 > str3) && str2 <= str3 && !(str3 <= str2);
	bool ComparingSameDataStrings = !(str3 < str4) && !(str3 > str4) && str3 <= str4 && str3 >= str4;
	bool ComparingWithSelf = !(str3 < str3) && !(str3 > str3) && str3 <= str3 && str3 >= str3; //-V501
	bool ComparingWithOtherHeapString = !(str4 < str5) && !(str4 > str5) && str4 <= str5 && str4 >= str5;
	bool ComparingWithConstString = !(str4 < str7) && !(str4 > str7) && str4 <= str7 && str4 >= str7;
	bool ComparingWithConstAString = !(str4 < str6) && !(str4 > str6) && str4 <= str6 && str4 >= str6;
	bool ComparingWithLiteral = !(str4 < L"123") && !(str4 > L"123") && str4 <= L"123" && str4 >= L"123";
	bool ComparingWithAsciiLiteral = !(str4 < "123") && !(str4 > "123") && str4 <= "123" && str4 >= "123";

	bool ComparingWithLowerString = str4 < str8 && str8 > str4 && str4 <= str8 && str8 >= str4; //-V501
	bool ComparingWithLowerOtherHeapString = str4 < str9 && str9 > str4 && str4 <= str9 && str9 >= str4; //-V501
	bool ComparingWithLowerConstString = str4 < str10 && String(str10) > str4 && str4 <= str10 && String(str10) >= str4;
	bool ComparingWithLowerConstAString = str4 < str11 && String(str11) > str4 && str4 <= str11 && String(str11) >= str4;
	bool ComparingWithLowerLiteral = str4 < L"1234" && String(L"1234") > str4 && str4 <= L"1234" && String(L"1234") >= str4;
	bool CompatingWithLowerAciiLiteral = str4 < "1234" && String("1234") > str4 && str4 <= "1234" && String("1234") >= str4;

	return
		ComparingEmptyStrings &&
		ComparingWithEmptyString &&
		ComparingSameDataStrings &&
		ComparingWithSelf &&
		ComparingWithOtherHeapString &&
		ComparingWithConstString &&
		ComparingWithConstAString &&
		ComparingWithLiteral &&
		ComparingWithAsciiLiteral &&
		ComparingWithLowerString &&
		ComparingWithLowerOtherHeapString &&
		ComparingWithLowerConstString &&
		ComparingWithLowerConstAString &&
		ComparingWithLowerLiteral &&
		CompatingWithLowerAciiLiteral;
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

	Asserts::AssertTrue(String::toString(Pattern1, 1) == L"1.0");
	Asserts::AssertTrue(String::toString(Pattern2, 2) == L"2.12");
	Asserts::AssertTrue(String::toString(Pattern3, 2) == L"0.14");
	Asserts::AssertTrue(String::toString(Pattern4, 3) == L"1.018");
	Asserts::AssertTrue(String::toString(Pattern5, 3) == L"0.016");

	Asserts::AssertTrue(Math::Equals(L"1.0"S.toDouble(),	Pattern1));
	Asserts::AssertTrue(Math::Equals(L"2.12"S.toDouble(),	Pattern2));
	Asserts::AssertTrue(Math::Equals(L"0.14"S.toDouble(),	Pattern3));
	Asserts::AssertTrue(Math::Equals(L"1.018"S.toDouble(), Pattern4));
	Asserts::AssertTrue(Math::Equals(L"0.016"S.toDouble(), Pattern5));

	return true;
}

bool Seed::Core::Testing::Test::Text::StringIntConvert::Excecute() {
	int Pattern1 = 1;
	int Pattern2 = 25;
	int Pattern3 = 0;
	int Pattern4 = 105;
	int Pattern5 = 1050;

	Asserts::AssertTrue(String::toString(Pattern1) == L"1");
	Asserts::AssertTrue(String::toString(Pattern2) == L"25");
	Asserts::AssertTrue(String::toString(Pattern3) == L"0");
	Asserts::AssertTrue(String::toString(Pattern4) == L"105");
	Asserts::AssertTrue(String::toString(Pattern5) == L"1050");

	Asserts::AssertTrue(L"1"S.toInt()	== Pattern1);
	Asserts::AssertTrue(L"25"S.toInt()	== Pattern2);
	Asserts::AssertTrue(L"0"S.toInt()	== Pattern3);
	Asserts::AssertTrue(L"105"S.toInt()	== Pattern4);
	Asserts::AssertTrue(L"1050"S.toInt()	== Pattern5);

	return true;
}

bool Seed::Core::Testing::Test::Text::StringSplitChar::Excecute() {
	auto text1 = L"123.123.123"S;

	auto items1 = text1.Split(L'.');

	Asserts::AssertTrue(items1.All(ArrowFunct(x, x == L"123")));

	auto text2 = L"123.123.123."S;

	auto items2 = text2.Split(L'.');

	Asserts::AssertTrue(items2.All(ArrowFunct(x, x == L"123")));

	auto text3 = L"1.1.1.1"S;

	auto items3 = text3.Split(L'.');

	Asserts::AssertTrue(items3.All(ArrowFunct(x, x == L"1")));

	return true;
}

bool Seed::Core::Testing::Test::Text::StringSplitCharLiteral::Excecute() {
	auto text1 = L"123::123::123"S;

	auto items1 = text1.Split(L"::");

	Asserts::AssertTrue(items1.All(ArrowFunct(x, x == L"123")));

	auto text2 = L"123::123::123::"S;

	auto items2 = text2.Split(L"::");

	Asserts::AssertTrue(items2.All(ArrowFunct(x, x == L"123")));

	auto text3 = L"1::1::1::1"S;

	auto items3 = text3.Split(L"::");

	Asserts::AssertTrue(items3.All(ArrowFunct(x, x == L"1")));

	return true;
}

bool Seed::Core::Testing::Test::Text::StringSplitString::Excecute() {
	auto text1 = L"123::123::123"S;

	auto items1 = text1.Split(L"::"S);

	Asserts::AssertTrue(items1.All(ArrowFunct(x, x == L"123")));

	auto text2 = L"123::123::123::"S;

	auto items2 = text2.Split(L"::"S);

	Asserts::AssertTrue(items2.All(ArrowFunct(x, x == L"123")));

	auto text3 = L"1::1::1::1"S;

	auto items3 = text3.Split(L"::"S);

	Asserts::AssertTrue(items3.All(ArrowFunct(x, x == L"1")));

	return true;
}

bool Seed::Core::Testing::Test::Text::StringCtorFromConstArray::Excecute() {
	Asserts::AssertEqual(String(Core::CTTI::Details::CreateTypeStr("123")), String(L"123"));
	Asserts::AssertEqual(String(Core::CTTI::Details::CreateTypeStr("123")) + L"4", String(L"1234"));
	Asserts::AssertEqual(String(Core::CTTI::Details::CreateTypeStr("123")) + L"4"S, String(L"1234"));
	Asserts::AssertEqual(L"1" + String(Core::CTTI::Details::CreateTypeStr("234")), String(L"1234"));
	Asserts::AssertEqual(L"1"S + String(Core::CTTI::Details::CreateTypeStr("234")), String(L"1234"));

	return true;
}
