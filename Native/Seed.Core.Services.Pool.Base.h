#pragma once
#include "Seed.Core.Services.Base.h"

namespace Seed::Core::Services::Pool
{
	/// <summary>
	/// Pool statistics structure
	/// </summary>
	struct PoolStatistics {
		/// <summary>
		/// Pool name
		/// </summary>
		String Name;
		
		/// <summary>
		/// Max size of pool containable entity
		/// </summary>
		SizeT MaxSize;

		/// <summary>
		/// Max pool enitites number
		/// </summary>
		SizeT MaxCount;

		/// <summary>
		/// Current number of entities in pool
		/// </summary>
		SizeT CurrentCount;

		StructCttiDef(PoolStatistics,
			DefStructFieldData(PoolStatistics, Name),
			DefStructFieldData(PoolStatistics, MaxSize),
			DefStructFieldData(PoolStatistics, MaxCount),
			DefStructFieldData(PoolStatistics, CurrentCount)
		)
	};
}