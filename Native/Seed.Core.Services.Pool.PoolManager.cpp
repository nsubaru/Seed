#include "Seed.Core.Services.Pool.PoolManager.h"

using namespace Seed;
using namespace Seed::Core;
using namespace Seed::Core::Services;
using namespace Seed::Core::Services::Pool;

template PoolManager DI::DependencyInjection::Resolve<PoolManager>();
template RTTI::RttiTypeRecord TypeRecord<PoolManager>();

void PoolManager::Dispose()
{
	if (This->subscription.IsValid())
	{
		MoveRValue(This->subscription).UnSubscibe();
	}
}

PoolManager::PoolManager(Services::InternalSensorsService sensorsService) : This(new Data())
{
	This->subscription = sensorsService.LowCommitMemoryEventSubscribe(new LowCommitConditionEventTarget(This));
}

Services::Pool::Pool PoolManager::CreatePool(String && poolName, SizeT maxPoolObjectSize, SizeT maxPoolObjectCount, SizeT startSize)
{
	if (maxPoolObjectCount == 0)
	{
		Throw(InvalidArgumentException(SourcePosition, L"maxPoolObjectSize", L"Size can't be null"));
	}

	auto retObj = Pool(MoveRValue(poolName), maxPoolObjectSize, maxPoolObjectCount, startSize ? startSize : maxPoolObjectCount);

	retObj.This->it = This->activePools.AddLast(retObj);
	retObj.This->RefCount--;

	return retObj;
}

Containers::Array<PoolStatistics> Seed::Core::Services::Pool::PoolManager::GetStats()
{
	return This->activePools.CopyTo<Containers::Array>([] (const Pool& item) {
		return PoolStatistics{
			.Name = item.PoolManagerName,
			.MaxSize = item.MaxPoolObjectSize,
			.MaxCount = item.MaxPoolObjectCount,
			.CurrentCount = item.AvailablePoolObjectCount
		};
	});
}

void PoolManager::LowCommitConditionEventTarget::onEvent(bool isUp)
{
	if (isUp)
	{
		This->activePools.ForEach([] (Pool& pool) { pool.FlushPool(); });
	}
}
