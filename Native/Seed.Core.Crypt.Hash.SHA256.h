#pragma once
#include "Seed.Core.Crypt.Hash.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Crypt
		{
			namespace Hash
			{
				class SHA256Hash : public IHashProvider, protected IDisposable {
				public:
					static constexpr SizeT HashBitsSize = 256;
					static constexpr SizeT HashSize = HashBitsSize / 8;
					using THash = Containers::SensitiveFixedArray<HashSize, Byte>;
				private:
					struct Data {
						Byte data[64];
						UInt32 datalen;
						UInt32 bitlen[2];
						UInt32 state[8];

						RefCountT RefCount;

						Data() : data(), datalen(), bitlen(), state(), RefCount(1) {}
					};
				private:
					Data* This;
				private:
					ForceInline void DBL_INT_ADD(UInt32& a, UInt32& b, UInt32 c) {
						if (a > 0xffffffff - (c)) ++b; a += c;
					}
				protected:
					virtual void Dispose() {}
				public:
					SHA256Hash();
					SHA256Hash(const Core::Containers::Blob& block) : SHA256Hash() {
						this->AddBlock(block);
					}

					template<typename THeap, SizeT TSize>
					SHA256Hash(const Containers::BaseFixedArray<THeap, TSize, Byte>& block) : SHA256Hash() {
						this->AddBlock(block);
					}
				public:
					template<typename THeap>
					SHA256Hash& AddBlock(const Containers::BaseArray<THeap, Byte>& block) {
						const Byte* const buffer = (const Byte* const) block;
						SizeT bytesCount = block.Count;

						UpdateHash(buffer, bytesCount);

						return *this;
					}

					template<typename THeap, SizeT TSize>
					SHA256Hash& AddBlock(const Containers::BaseFixedArray<THeap, TSize, Byte>& block) {
						UpdateHash(block);

						return *this;
					}
				private:
					template<typename THeap, SizeT TSize>
					void UpdateHash(const Containers::BaseFixedArray<THeap, TSize, Byte>& block) {
						decltype(auto) data = (const Byte(&)[TSize]) block;

						for (SizeT i = 0; i < block.Count; ++i)
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
				private:
					void Transform();
					void UpdateHash(const Byte* const data, SizeT len);
				public:
					THash GetFixedHash();
				public:
					Containers::SensitiveBlob GetHash() {
						return GetFixedHash().CopyTo<Containers::SensitiveArray>();
					}
				public:
					RefCountClass(SHA256Hash, This)
				};
			}
		}
	}
}