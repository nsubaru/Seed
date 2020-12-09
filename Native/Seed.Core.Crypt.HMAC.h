#pragma once
#include "Seed.Core.Crypt.Base.h"
#include "Seed.Core.Crypt.Hash.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Crypt
		{
			template<typename THashProvider> requires Concepts::IsBasedOn<Hash::IHashProvider, THashProvider>
			class HMAC {
			public:
				static constexpr SizeT HashSize = THashProvider::HashSize;
				using THash = typename THashProvider::THash;
			private:
				THashProvider hashProvider;
				THash key;
			public:
				HMAC(THash&& key) : 
					hashProvider(), 
					key(MoveRValue(key)) 
				{ 
					// Inner padded key
					auto ikeypad = key.CopyToFixed<Containers::SensitiveFixedArray>([] (const Byte& x) { return (Byte) (x ^ 0x36); });

					hashProvider.AddBlock(ikeypad);
				}

				HMAC(const THash& key) : hashProvider(), key(key) {
					// Inner padded key
					auto ikeypad = key.CopyToFixed<Containers::SensitiveFixedArray>([] (const Byte& x) { return (Byte) (x ^ 0x36); });

					hashProvider.AddBlock(ikeypad);
				}
			public:
				template<typename THeap>
				HMAC& AddBlock(const Containers::BaseArray<THeap, Byte>& blob) {
					hashProvider.AddBlock(blob);
					return *this;
				}

				template<typename THeap, SizeT TSize>
				HMAC& AddBlock(const Containers::BaseFixedArray<THeap, TSize, Byte>& block) {
					hashProvider.AddBlock(block);
					return *this;
				}
			public:
				THash Generate(const THash& key) {
					// Generate hash as pseudocoded function:
					//  "hash(outer_key || hash(inner_key || data))"
					//	where '||' operator combine data
					//
					// inner key we combine in ctor by adding it before any other data
					// oiter key we combine in this method as it's require data hash
					return THashProvider().AddBlock(Containers::Combine(
						//Outer padded key
						key.CopyToFixed<Containers::SensitiveFixedArray>([] (const Byte& x) { return (Byte) (x ^ 0x5c); }),
						//Data hash
						hashProvider.GetFixedHash()
					)).GetFixedHash();
				}
			};

			using HmacSha256 = HMAC<Hash::SHA256Hash>;
		}
	}
}