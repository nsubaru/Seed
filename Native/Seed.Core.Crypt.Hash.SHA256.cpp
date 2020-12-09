#include "Seed.Core.Crypt.Hash.SHA256.h"

using namespace Seed;

// Signed variables are for wimps
#define uchar unsigned char // 8-bit byte
#define uint unsigned long // 32-bit word

// DBL_INT_ADD treats two unsigned ints a and b as one 64-bit integer and adds c to it
//#define DBL_INT_ADD(a,b,c) if (a > 0xffffffff - (c)) ++b; a += c;
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

uint k[64] = {
   0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
   0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
   0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
   0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
   0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
   0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
   0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
   0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

Seed::Core::Crypt::Hash::SHA256Hash::SHA256Hash() : This(new Data()) {
    This->datalen = 0;
    This->bitlen[0] = 0;
    This->bitlen[1] = 0;
    This->state[0] = 0x6a09e667;
    This->state[1] = 0xbb67ae85;
    This->state[2] = 0x3c6ef372;
    This->state[3] = 0xa54ff53a;
    This->state[4] = 0x510e527f;
    This->state[5] = 0x9b05688c;
    This->state[6] = 0x1f83d9ab;
    This->state[7] = 0x5be0cd19;
}

void Seed::Core::Crypt::Hash::SHA256Hash::Transform() {
    uint a, b, c, d, e, f, g, h, j, t1, t2, m[64]{};

    SizeT i = 0;

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (This->data[j] << 24) | (This->data[j + 1] << 16) | (This->data[j + 2] << 8) | (This->data[j + 3]);
    for (; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    a = This->state[0];
    b = This->state[1];
    c = This->state[2];
    d = This->state[3];
    e = This->state[4];
    f = This->state[5];
    g = This->state[6];
    h = This->state[7];

    for (i = 0; i < 64; ++i)
    {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    This->state[0] += a;
    This->state[1] += b;
    This->state[2] += c;
    This->state[3] += d;
    This->state[4] += e;
    This->state[5] += f;
    This->state[6] += g;
    This->state[7] += h;
}

void Seed::Core::Crypt::Hash::SHA256Hash::UpdateHash(const Byte* const data, SizeT len) {
    for (SizeT i = 0; i < len; ++i)
    {
        This->data[This->datalen] = data[i];
        This->datalen++;
        if (This->datalen == 64)
        {
            Transform();
            DBL_INT_ADD(This->bitlen[0], This->bitlen[1], 512);
            This->datalen = 0;
        }
    }
}

Seed::Core::Crypt::Hash::SHA256Hash::THash Seed::Core::Crypt::Hash::SHA256Hash::GetFixedHash() {
    SizeT i;

    i = This->datalen;

    // Pad whatever data is left in the buffer.
    if (This->datalen < 56)
    {
        This->data[i++] = 0x80;
        while (i < 56)
            This->data[i++] = 0x00;
    }
    else
    {
        This->data[i++] = 0x80;
        while (i < 64)
            This->data[i++] = 0x00;
        Transform();
        Memory::MemBlkFill(This->data, 56, 0);
    }

    // Append to the padding the total message's length in bits and transform.
    DBL_INT_ADD(This->bitlen[0], This->bitlen[1], This->datalen * 8);
    This->data[63] = This->bitlen[0];
    This->data[62] = This->bitlen[0] >> 8;
    This->data[61] = This->bitlen[0] >> 16;
    This->data[60] = This->bitlen[0] >> 24;
    This->data[59] = This->bitlen[1];
    This->data[58] = This->bitlen[1] >> 8;
    This->data[57] = This->bitlen[1] >> 16;
    This->data[56] = This->bitlen[1] >> 24;
    Transform();

    THash hash = {};

    // Since this implementation uses little endian byte ordering and SHA uses big endian,
    // reverse all the bytes when copying the final state to the output hash.
    for (i = 0; i < 4; ++i)
    {
        hash[i] = (This->state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4] = (This->state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8] = (This->state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (This->state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (This->state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (This->state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (This->state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (This->state[7] >> (24 - i * 8)) & 0x000000ff;
    }

    return hash;
}