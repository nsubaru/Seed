#include "Seed.Core.Crypt.h"

#include "Seed.Core.Testing.Tests.Crypt.Hashing.h"

bool Seed::Core::Testing::Test::Crypt::Hashing::Sha256HashTest1::Excecute() {
    auto message = Core::Containers::Blob{ 'a', 'b', 'c' };
    auto hashGenerator = Core::Crypt::Hash::SHA256Hash(message);

    Asserts::AssertEqual(String::binaryToHex(hashGenerator.GetHash()).ToDowncase(),
        L"BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD"S.ToDowncase());

    return true;
}

bool Seed::Core::Testing::Test::Crypt::Hashing::Sha256HashTest2::Excecute() {
    constexpr Byte messageText[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    auto message = Core::Containers::FixedArray<56, Byte>(messageText, 0);

    auto hashGenerator = Core::Crypt::Hash::SHA256Hash(message);

    Asserts::AssertEqual(String::binaryToHex(hashGenerator.GetHash()).ToDowncase(),
        L"248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1"S.ToDowncase());

    return true;
}
