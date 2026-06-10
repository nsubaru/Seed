#include "Seed.Core.Crypt.Hash.SHA1.h"

#define uchar unsigned char // 8-bit byte
#define uint unsigned long // 32-bit word

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

Seed::Core::Crypt::Hash::SHA1Hash::SHA1Hash() : This(new Data())
{
	This->datalen = 0;
	This->bitlen = 0;
	This->state[0] = 0x67452301;
	This->state[1] = 0xEFCDAB89;
	This->state[2] = 0x98BADCFE;
	This->state[3] = 0x10325476;
	This->state[4] = 0xC3D2E1F0;
}

void Seed::Core::Crypt::Hash::SHA1Hash::Transform()
{
	uint a, b, c, d, e, f, j, k, t1, m[80]{};

	SizeT i = 0;

	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (This->data[j] << 24) | (This->data[j + 1] << 16) | (This->data[j + 2] << 8) | (This->data[j + 3]);
	for (; i < 80; ++i)
		m[i] = ROTLEFT(m[i - 3] ^ m[i - 8] ^ m[i - 14] ^ m[i - 16], 1);

	a = This->state[0];
	b = This->state[1];
	c = This->state[2];
	d = This->state[3];
	e = This->state[4];

	for (size_t j = 0; j < 80; ++j)
	{
		if (j < 20)
			f = (b & c) | (~b & d), k = 0x5A827999;
		else if (j < 40)
			f = b ^ c ^ d, k = 0x6ED9EBA1;
		else if (j < 60)
			f = (b & c) | (b & d) | (c & d), k = 0x8F1BBCDC;
		else
			f = b ^ c ^ d, k = 0xCA62C1D6;

		t1 = ROTLEFT(a, 5) + f + e + k + m[j];
		e = d;
		d = c;
		c = ROTLEFT(b, 30);
		b = a;
		a = t1;
	}

	This->state[0] += a;
	This->state[1] += b;
	This->state[2] += c;
	This->state[3] += d;
	This->state[4] += e;
}

void Seed::Core::Crypt::Hash::SHA1Hash::UpdateHash(const Byte* data, SizeT len)
{
	for (SizeT i = 0; i < len; ++i)
	{
		This->data[This->datalen] = data[i];
		This->datalen++;
		if (This->datalen == 64)
		{
			Transform();
			This->bitlen += 512;
			This->datalen = 0;
		}
	}
}

Seed::Core::Crypt::Hash::SHA1Hash::THash Seed::Core::Crypt::Hash::SHA1Hash::GetFixedHash()&&
{
	SizeT i = This->datalen;

	This->data[i++] = 0x80;

	if (This->datalen < 56)
	{
		while (i < 56)
			This->data[i++] = 0x00;
	}
	else
	{
		while (i < 64)
			This->data[i++] = 0x00;
		Transform();
		Memory::MemBlkFill(This->data, 56, 0);
	}

	// Append to the padding the total message's length in bits and transform.
	This->bitlen += This->datalen * 8;

	This->data[63] = static_cast<Byte>((This->bitlen >> 0) & 0xFF);
	This->data[62] = static_cast<Byte>((This->bitlen >> 8) & 0xFF);
	This->data[61] = static_cast<Byte>((This->bitlen >> 16) & 0xFF);
	This->data[60] = static_cast<Byte>((This->bitlen >> 24) & 0xFF);
	This->data[59] = static_cast<Byte>((This->bitlen >> 32) & 0xFF);
	This->data[58] = static_cast<Byte>((This->bitlen >> 40) & 0xFF);
	This->data[57] = static_cast<Byte>((This->bitlen >> 48) & 0xFF);
	This->data[56] = static_cast<Byte>((This->bitlen >> 56) & 0xFF);
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
	}

	return MoveRValue(hash);
}

Seed::Core::Crypt::Hash::SHA1Hash::THash Seed::Core::Crypt::Hash::SHA1Hash::GetFixedHash()&
{
	// Save current state of hash object
	Byte oldData[64]{};
	UInt32 oldState[5]{};
	UInt64 oldBitlen = This->bitlen;
	UInt64 oldDatalen = This->datalen;

	SizeT i = This->datalen;

	This->data[i++] = 0x80;

	if (This->datalen < 56)
	{
		while (i < 56)
			This->data[i++] = 0x00;
	}
	else
	{
		while (i < 64)
			This->data[i++] = 0x00;
		Transform();
		Memory::MemBlkFill(This->data, 56, 0);
	}

	// Append to the padding the total message's length in bits and transform.
	This->bitlen += This->datalen * 8;

	This->data[63] = static_cast<Byte>((This->bitlen >> 0) & 0xFF);
	This->data[62] = static_cast<Byte>((This->bitlen >> 8) & 0xFF);
	This->data[61] = static_cast<Byte>((This->bitlen >> 16) & 0xFF);
	This->data[60] = static_cast<Byte>((This->bitlen >> 24) & 0xFF);
	This->data[59] = static_cast<Byte>((This->bitlen >> 32) & 0xFF);
	This->data[58] = static_cast<Byte>((This->bitlen >> 40) & 0xFF);
	This->data[57] = static_cast<Byte>((This->bitlen >> 48) & 0xFF);
	This->data[56] = static_cast<Byte>((This->bitlen >> 56) & 0xFF);
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
	}

	// Restore old state of hash object
	This->bitlen = oldBitlen;
	This->datalen = oldDatalen;

	Memory::Copy(oldData, This->data);
	Memory::Copy(oldState, This->state);

	return MoveRValue(hash);
}
