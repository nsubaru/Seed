#pragma once
#include "Seed.Core.Services.Pool.Base.h"
#include "Seed.Core.Services.Pool.ObjectsPool.h"

#include "Seed.Core.Services.EventService.h"
#include "Seed.Core.Services.BootupServicesManager.h"

namespace Seed::Core::Services::Pool
{
	/// <summary>
	/// Pool builder service
	/// </summary>
	class PoolBuilder final : public IDisposable, IBootUpService<PoolBuilder, Services::EventService> {
		struct Data {
			Services::EventService eventService;
			ID lowCommitConditionEvenHandlerId;

			Containers::ConcurencyList<ObjectsPool> activePools;

			RefCountT RefCount;

			Data(Services::EventService eventService) : eventService(eventService), lowCommitConditionEvenHandlerId(0), activePools(), RefCount(1) {}
		};

		Data* This;

	protected:
		virtual void Dispose() override {
			This->eventService
				.GetEventByName(CurrentSettings().StaticSettings.HostCommitLowEventName)
				.UnRegisterCallback(This->lowCommitConditionEvenHandlerId);
		}

		PoolBuilder(Data* This) : This(This) {
			++This->RefCount;
		}

	private:
		class LowCommitConditionEventTarget final : public Events::IGlobalEventCallback {
		private:
			Data* This;

		protected:
			virtual void Dispose() {
			}

		public:
			LowCommitConditionEventTarget(Data* This) : This(This) {
			}

			virtual void onEvent(Events::GlobalEventTarget target, Events::EventState eventOnCallState, RTTI::Dynamic dynamic) override {
				This->activePools.ForEach([] (ObjectsPool& pool) { pool.FlushPool(); });
			}

		public:
			~LowCommitConditionEventTarget() {
			}
		};

	public:
		/// <summary>
		/// Service constructor
		/// </summary>
		PoolBuilder(Services::EventService eventService) : This(new Data(eventService)) {
			This->lowCommitConditionEvenHandlerId = eventService
				.GetEventByName(CurrentSettings().StaticSettings.HostCommitLowEventName)
				.RegisterCallback(new LowCommitConditionEventTarget(This));
		};

	public:
		/// <summary>
		/// Creates new pool
		/// </summary>
		/// <param name="poolName">Pool name</param>
		/// <param name="maxPoolObjectSize">Maximum pool object buffer size</param>
		/// <param name="maxPoolObjectCount">Maximum pool object buffer count</param>
		/// <returns>New pool</returns>
		ObjectsPool CreatePool(String&& poolName, SizeT maxPoolObjectSize, SizeT maxPoolObjectCount, SizeT startSize = 0) {
			if (maxPoolObjectCount == 0)
			{
				Throw(InvalidArgumentException(SourcePosition, L"maxPoolObjectSize", L"Size can't be null"));
			}

			auto retObj = ObjectsPool(MoveRValue(poolName), maxPoolObjectSize, maxPoolObjectCount, startSize ? startSize : maxPoolObjectCount);

			retObj.This->it = This->activePools.AddLast(retObj);
			retObj.This->RefCount--;

			return retObj;
		}

	public:
		/// <summary>
		/// Returns number of active pool managers
		/// </summary>
		/// <returns>Number of active pool managers</returns>
		SizeT GetActivePoolsCount() {
			return This->activePools.Count;
		}

		/// <summary>
		/// Number of active pool managers
		/// </summary>
		SizeT Property(GetActivePoolsCount) ActivePoolsCount;

		/// <summary>
		/// Returns pool statistics
		/// </summary>
		/// <returns>Pool statistics</returns>
		Containers::Array<PoolStatistics> GetStats() {
			return This->activePools.CopyTo<Containers::Array>([] (const ObjectsPool& item) {
				return PoolStatistics{
					.Name = item.PoolManagerName,
					.MaxSize = item.MaxPoolObjectSize,
					.MaxCount = item.MaxPoolObjectCount,
					.CurrentCount = item.AvailablePoolObjectCount
				};
				});
		}

	public:
		RefCountClass(PoolBuilder, This)
	};
}