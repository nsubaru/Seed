#pragma once
#include "Seed.Core.Services.Base.h"

namespace Seed::Core::Services::Pool
{
	/// <summary>
	/// Pool statistics structure
	/// </summary>
	struct PoolStatistics {
		String Name;
		SizeT MaxSize;
		SizeT MaxCount;
		SizeT CurrentCount;

		StructCttiDef(PoolStatistics,
			DefStructFieldData(PoolStatistics, Name),
			DefStructFieldData(PoolStatistics, MaxSize),
			DefStructFieldData(PoolStatistics, MaxCount),
			DefStructFieldData(PoolStatistics, CurrentCount)
		)
	};
}