#pragma once
#include "Seed.Core.Crypt.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Crypt
		{
			namespace Hash
			{
				class IHashProvider {
				public:
					virtual Containers::SensitiveBlob GetHash() = 0;
				public:
					virtual ~IHashProvider() = 0 {}
				};
			}
		}
	}
}