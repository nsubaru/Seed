// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Services.Cache.Service.h"

using namespace Seed;
using namespace Seed::Core;
using namespace Seed::Core::Services;
using namespace Seed::Core::Services::Cache;

template InMemoryCacheProviderService DI::DependencyInjection::Resolve<InMemoryCacheProviderService>();
template RTTI::RttiTypeRecord TypeRecord<InMemoryCacheProviderService>();

void InMemoryCacheProviderService::Dispose()
{
	This->eventService
		.GetEventByName(RuntimeConfigurations().StaticConfigurations.HostCommitLowEventName)
		.UnRegisterCallback(This->lowMemoryCallbackId);

	This->caches.CopyTo<Containers::Array>().RemoveAll([] (IMemoryCache* x) { ++x->GetRefCount(); delete x; });
}

SizeT Seed::Core::Services::Cache::InMemoryCacheProviderService::GetCacheItemCount() const
{
	return This->caches.Aggregate<SizeT>(ArrowFunct(x, y, x + y->GetCount()), 0);
}

SizeT Seed::Core::Services::Cache::InMemoryCacheProviderService::GetCacheMaxItemCount() const
{
	return This->caches.Aggregate<SizeT>(ArrowFunct(x, y, x + y->GetMaxCount()), 0);
}

SizeT Seed::Core::Services::Cache::InMemoryCacheProviderService::GetCacheSize() const
{
	return This->caches.Aggregate<SizeT>(ArrowFunct(x, y, x + y->GetSize()), 0);
}

SizeT Seed::Core::Services::Cache::InMemoryCacheProviderService::GetCacheMaxSize() const
{
	return This->caches.Aggregate<SizeT>(ArrowFunct(x, y, x + y->GetMaxSize()), 0);
}
