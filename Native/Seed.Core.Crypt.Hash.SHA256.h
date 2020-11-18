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
						Boolean isFirstBlock;

						Int8 data[64];
						Int32 datalen;
						Int64 bitlen;
						Int32 state[8];

						RefCountT RefCount;

						Data() : isFirstBlock(true), data(), datalen(), bitlen(), state(), RefCount(1) {}
					};
				private:
					Data* This;
				protected:
					virtual void Dispose() {}
				public:
					SHA256Hash();
				public:
					template<typename THeap>
					SHA256Hash& AddBlock(const Containers::BaseArray<THeap, Byte>& block) {
						const Byte* const buffer = (const Byte* const) block;
						SizeT bytesCount = block.Count;

						if (This->isFirstBlock)
						{
							This->isFirstBlock = false;
							FirstBlockHash(buffer, bytesCount);
						}
						else
						{
							UpdateHash(buffer, bytesCount);
						}
					}

					template<typename THeap, SizeT TSize>
					SHA256Hash& AddBlock(const Containers::BaseFixedArray<THeap, TSize, Byte>& block) {
						if (This->isFirstBlock)
						{
							This->isFirstBlock = false;
							FirstBlockHash(block);
						}
						else
						{
							UpdateHash(block);
						}
					}
				private:
					template<typename THeap, SizeT TSize>
					void FirstBlockHash(const Containers::BaseFixedArray<THeap, TSize, Byte>& block) {
						auto data = (const Byte(&)[TSize]) block;

						for (int i = 0; i < TSize; ++i)
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

					template<typename THeap, SizeT TSize>
					void UpdateHash(const Containers::BaseFixedArray<THeap, TSize, Byte>& block) {
						auto data = (const Byte(&)[TSize]) block;

						for (int i = 0; i < TSize; ++i)
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
				private:
					void Transform();
					void FirstBlockHash(const Byte* const data, SizeT len);
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