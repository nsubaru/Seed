// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Crypt.Algorithms.h"

#include "Seed.Core.Crypt.Algorithms.h"
#include "Seed.Core.Text.h"

using namespace Seed::Core;

#pragma region Utils
template<SizeT TN>
struct TextWithouZero {
	unsigned char Text[TN];

	static constexpr SizeT Lenght = TN;
};

template<SizeT TN>
constexpr static TextWithouZero<TN - 1> CreateStr(char const (&text)[TN])
{
	TextWithouZero<TN - 1> value;

	for (SizeT i = 0; i < TN - 1; i++)
	{
		value.Text[i] = text[i];
	}

	return value;
}
#pragma endregion

#pragma region HMAC
#pragma region HMAC-SHA1
bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA1Test1::Execute()
{
	auto key = Containers::SensitiveFixedArray<20, Byte>::Generate(ArrowFunct(0x0b));

	constexpr auto str = CreateStr("Hi There");

	auto msg = Containers::Blob(str.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA1Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"b617318655057264e28bc0b6fb378c8ef146be00", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA1Test2::Execute()
{
	auto keyStr = CreateStr("Jefe");
	auto key = Containers::SensitiveFixedArray<keyStr.Lenght, Byte>(keyStr.Text);

	constexpr auto str = CreateStr("what do ya want for nothing?");

	auto msg = Containers::Blob(str.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA1Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"effcdf6ae5eb2fa2d27416d5f184df9c259a7c79"S, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA1Test3::Execute()
{
	auto key = Containers::SensitiveFixedArray<20, Byte>::Generate(ArrowFunct(0xaa));

	auto msg = Containers::Blob::Generate(ArrowFunct(0xdd), 50);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA1Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"125d7342b9ac11cd91a39af48aa17b4f63f175d3", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA1Test4::Execute()
{
	auto key = Containers::SensitiveFixedArray<25, Byte>({ 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19 });

	auto msg = Containers::Blob::Generate(ArrowFunct(0xcd), 50);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA1Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"4c9007f4026250c6bc8414f9bf50c86c2d7235da", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA1Test5::Execute()
{
	auto key = Containers::SensitiveFixedArray<20, Byte>::Generate(ArrowFunct(0x0c));

	constexpr auto str = CreateStr("Test With Truncation");

	auto msg = Containers::Blob(str.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA1Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"4c1a03424b55e07fe7f27be1d58bb9324a9a5a04", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA1Test6::Execute()
{
	auto key = Containers::SensitiveFixedArray<80, Byte>::Generate(ArrowFunct(0xaa));

	constexpr auto str = CreateStr("Test Using Larger Than Block-Size Key - Hash Key First");

	auto msg = Containers::Blob(str.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA1Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"aa4ae5e15272d00e95705637ce8a3b55ed402112", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA1Test7::Execute()
{
	auto key = Containers::SensitiveFixedArray<80, Byte>::Generate(ArrowFunct(0xaa));

	constexpr auto strp1 = CreateStr("Test Using Larger Than Block-Size Key and Larger ");
	constexpr auto strp2 = CreateStr("Than One Block-Size Data");

	static_assert(strp1.Lenght + strp2.Lenght == 73);

	auto msgp1 = Containers::Blob(strp1.Text);
	auto msgp2 = Containers::Blob(strp2.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA1Hash>(key).AddBlock(msgp1).AddBlock(msgp2).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"e8e99d0f45237d786d6bbaa7965c7808bbff1a91", SourcePosition);

	return true;
}
#pragma endregion

#pragma region HMAC-SHA256
bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA256Test1::Execute()
{
	auto key = Containers::SensitiveFixedArray<20, Byte>::Generate(ArrowFunct(0x0b));

	constexpr auto str = CreateStr("Hi There");

	auto msg = Containers::Blob(str.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA256Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA256Test2::Execute()
{
	auto keyStr = CreateStr("Jefe");
	auto key = Containers::SensitiveFixedArray<keyStr.Lenght, Byte>(keyStr.Text);

	constexpr auto str = CreateStr("what do ya want for nothing?");

	auto msg = Containers::Blob(str.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA256Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843"S, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA256Test3::Execute()
{
	auto key = Containers::SensitiveFixedArray<20, Byte>::Generate(ArrowFunct(0xaa));

	auto msg = Containers::Blob::Generate(ArrowFunct(0xdd), 50);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA256Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA256Test4::Execute()
{
	auto key = Containers::SensitiveFixedArray<25, Byte>({ 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19 });

	auto msg = Containers::Blob::Generate(ArrowFunct(0xcd), 50);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA256Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA256Test5::Execute()
{
	auto key = Containers::SensitiveFixedArray<20, Byte>::Generate(ArrowFunct(0x0c));

	constexpr auto str = CreateStr("Test With Truncation");

	auto msg = Containers::Blob(str.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA256Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase().Resize(33), L"a3b6167473100ee06e0c796c2955552b", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA256Test6::Execute()
{
	auto key = Containers::SensitiveFixedArray<131, Byte>::Generate(ArrowFunct(0xaa));

	constexpr auto str = CreateStr("Test Using Larger Than Block-Size Key - Hash Key First");

	auto msg = Containers::Blob(str.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA256Hash>(key).AddBlock(msg).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::HMACSHA256Test7::Execute()
{
	auto key = Containers::SensitiveFixedArray<131, Byte>::Generate(ArrowFunct(0xaa));

	constexpr auto strp1 = CreateStr("This is a test using a larger than block-size key and a larger t");
	constexpr auto strp2 = CreateStr("han block-size data. The key needs to be hashed before being used by the HMAC algorithm.");

	auto msgp1 = Containers::Blob(strp1.Text);
	auto msgp2 = Containers::Blob(strp2.Text);

	auto digest = Core::Crypt::Algorithms::HMAC<Core::Crypt::Hash::SHA256Hash>(key).AddBlock(msgp1).AddBlock(msgp2).Generate();

	Asserts::AssertEqual(String::binaryToHex(digest.CopyTo<Containers::SensitiveArray>()).ToDowncase(), L"9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2", SourcePosition);

	return true;
}
#pragma endregion
#pragma endregion

#pragma region TOTP
#pragma region SHA1
bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha1HashTest1::Execute()
{
	auto testSecretBase32 = L"GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ"S;
	auto testSteps = 1;

	auto testSecretBlob = Core::Text::Base32::Decode(testSecretBase32).CopyToFixed<Core::Crypt::Hash::SHA1Hash::THash>();

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA1Hash>(testSecretBlob);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"94287082", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha1HashTest2::Execute()
{
	auto testSecretBase32 = L"GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ"S;
	auto testSteps = 0x00000000023523EC;

	auto testSecretBlob = Core::Text::Base32::Decode(testSecretBase32).CopyToFixed<Core::Crypt::Hash::SHA1Hash::THash>();

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA1Hash>(testSecretBlob);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"07081804", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha1HashTest3::Execute()
{
	auto keyText = CreateStr("12345678901234567890");
	auto testSecretAscii = Containers::Blob(keyText.Text);
	auto testSteps = 0x00000000023523ED;

	auto testSecretBase32 = Seed::Core::Text::Base32::Encode(testSecretAscii);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA1Hash>(testSecretBase32);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"14050471", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha1HashTest4::Execute()
{
	auto keyText = CreateStr("12345678901234567890");
	auto testSecretAscii = Containers::Blob(keyText.Text);
	auto testSteps = 0x000000000273EF07;

	auto testSecretBase32 = Seed::Core::Text::Base32::Encode(testSecretAscii);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA1Hash>(testSecretBase32);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"89005924", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha1HashTest5::Execute()
{
	auto keyText = CreateStr("12345678901234567890");
	auto testSecretAscii = Containers::Blob(keyText.Text);
	auto testSteps = 0x0000000003F940AA;

	auto testSecretBase32 = Seed::Core::Text::Base32::Encode(testSecretAscii);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA1Hash>(testSecretBase32);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"69279037", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha1HashTest6::Execute()
{
	auto keyText = CreateStr("12345678901234567890");
	auto testSecretAscii = Containers::Blob(keyText.Text);
	auto testSteps = 0x0000000027BC86AA;

	auto testSecretBase32 = Seed::Core::Text::Base32::Encode(testSecretAscii);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA1Hash>(testSecretBase32);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"65353130", SourcePosition);

	return true;
}
#pragma endregion

#pragma region SHA256
bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha256HashTest1::Execute()
{
	auto testSecretBase32 = L"GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZA===="S;
	auto testSteps = 1;

	auto testSecretBlob = Core::Text::Base32::Decode(testSecretBase32).CopyToFixed<Core::Crypt::Hash::SHA256Hash::THash>(true);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA256Hash>(testSecretBlob);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"46119246", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha256HashTest2::Execute()
{
	auto testSecretBase32 = L"GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZA===="S;
	auto testSteps = 0x00000000023523EC;

	auto testSecretBlob = Core::Text::Base32::Decode(testSecretBase32).CopyToFixed<Core::Crypt::Hash::SHA256Hash::THash>(true);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA256Hash>(testSecretBlob);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"68084774", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha256HashTest3::Execute()
{
	auto keyText = CreateStr("12345678901234567890123456789012");
	auto testSecretAscii = Containers::Blob(keyText.Text);
	auto testSteps = 0x00000000023523ED;

	auto testSecretBase32 = Seed::Core::Text::Base32::Encode(testSecretAscii);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA256Hash>(testSecretBase32);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"67062674", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha256HashTest4::Execute()
{
	auto keyText = CreateStr("12345678901234567890123456789012");
	auto testSecretAscii = Containers::Blob(keyText.Text);
	auto testSteps = 0x000000000273EF07;

	auto testSecretBase32 = Seed::Core::Text::Base32::Encode(testSecretAscii);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA256Hash>(testSecretBase32);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"91819424", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha256HashTest5::Execute()
{
	auto keyText = CreateStr("12345678901234567890123456789012");
	auto testSecretAscii = Containers::Blob(keyText.Text);
	auto testSteps = 0x0000000003F940AA;

	auto testSecretBase32 = Seed::Core::Text::Base32::Encode(testSecretAscii);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA256Hash>(testSecretBase32);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"90698825", SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Crypt::Algorithms::TOTPSha256HashTest6::Execute()
{
	auto keyText = CreateStr("12345678901234567890123456789012");
	auto testSecretAscii = Containers::Blob(keyText.Text);
	auto testSteps = 0x0000000027BC86AA;

	auto testSecretBase32 = Seed::Core::Text::Base32::Encode(testSecretAscii);

	auto totp = Core::Crypt::Algorithms::TOTP<Core::Crypt::Hash::SHA256Hash>(testSecretBase32);

	auto key = totp.CreateCustomTOTPKey(testSteps, 8);

	Asserts::AssertEqual(key, L"77737706", SourcePosition);

	return true;
}
#pragma endregion
#pragma endregion