#pragma once
#include "Seed.Core.Base.Definitions.h"
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.UUID.h"

#pragma warning(push)
#pragma warning(disable: 4996)

namespace Seed
{
	inline namespace Core
	{
		namespace Internals
		{
			InternalApiCall SeedKernelCall void UppercaseCharStrImpl(Seed::Core::CharStr from, Seed::Core::CharStr to);
			InternalApiCall SeedKernelCall bool ConvertCharStrToUUID(Seed::Core::CharStr str, Seed::Core::UUID& uuid);
			InternalApiCall SeedKernelCall void ConvertUUIDtoCharStr(const Seed::Core::UUID& id, Seed::Core::CharStr buffer);
			InternalApiCall SeedKernelCall Seed::Core::Char UpcaseCharImp(Seed::Core::Char symb);
			InternalApiCall SeedKernelCall Seed::Core::Char DowncaseCharImp(Seed::Core::Char symb);
			InternalApiCall SeedKernelCall void DowncaseCharStrImpl(Seed::Core::CharStr from, Seed::Core::CharStr to);
			InternalApiCall SeedKernelCall Seed::Int32 CompareString(Seed::CharStr left, Seed::CharStr right, Seed::SizeT leftSize, Seed::SizeT rightSize);
		}
	}
}

#pragma warning(pop)