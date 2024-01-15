#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed::Core::Testing::Test::Crypt::Algorithms
{
    // RFC 6238 data (HMAC-SHA-(1, 256, 512))
    SeedTestDef(TOTPSha1HashTest1);
    SeedTestDef(TOTPSha1HashTest2);
    SeedTestDef(TOTPSha1HashTest3);
    SeedTestDef(TOTPSha1HashTest4);
    SeedTestDef(TOTPSha1HashTest5);
    SeedTestDef(TOTPSha1HashTest6);

    SeedTestDef(TOTPSha256HashTest1);
    SeedTestDef(TOTPSha256HashTest2);
    SeedTestDef(TOTPSha256HashTest3);
    SeedTestDef(TOTPSha256HashTest4);
    SeedTestDef(TOTPSha256HashTest5);
    SeedTestDef(TOTPSha256HashTest6);

    //RFC 2202 data (HMAC-SHA-1, HMAC-MD5)
    SeedTestDef(HMACSHA1Test1);
    SeedTestDef(HMACSHA1Test2);
    SeedTestDef(HMACSHA1Test3);
    SeedTestDef(HMACSHA1Test4);
    SeedTestDef(HMACSHA1Test5);
    SeedTestDef(HMACSHA1Test6);
    SeedTestDef(HMACSHA1Test7);

    //RFC 4231 data (HMAC-SHA-(224,256,384,512))
    SeedTestDef(HMACSHA256Test1);
    SeedTestDef(HMACSHA256Test2);
    SeedTestDef(HMACSHA256Test3);
    SeedTestDef(HMACSHA256Test4);
    SeedTestDef(HMACSHA256Test5);
    SeedTestDef(HMACSHA256Test6);
    SeedTestDef(HMACSHA256Test7);
}