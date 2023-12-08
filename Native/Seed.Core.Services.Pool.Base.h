#pragma once
#include "Seed.Core.Services.Base.h"

namespace Seed::Core::Services::Pool
{
	struct PoolStatistics {
		String Name;
		SizeT MaxSize;
		SizeT MaxCount;
		SizeT CurrentCount;
	};
}