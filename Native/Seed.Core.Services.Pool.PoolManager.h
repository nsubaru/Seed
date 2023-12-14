#pragma once
#include "Seed.Core.Services.Pool.Base.h"
#include "Seed.Core.Services.Pool.Pool.h"

#include "Seed.Core.Services.EventService.h"
#include "Seed.Core.Services.BootupServicesManager.h"

namespace Seed::Core::Services::Pool
{
	/// <summary>
	/// Pool builder service
	/// </summary>
	class PoolManager final : public IDisposable, IBootUpService<PoolManager, Services::EventService> {
		struct Data {
			Services::EventService eventService;
			ID lowCommitConditionEvenHandlerId;

			Containers::ConcurencyList<Pool> activePools;

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

		PoolManager(Data* This) : This(This) {
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
				This->activePools.ForEach([] (Pool& pool) { pool.FlushPool(); });
			}

		public:
			~LowCommitConditionEventTarget() {
			}
		};

	public:
		/// <summary>
		/// Service constructor
		/// </summary>
		PoolManager(Services::EventService eventService) : This(new Data(eventService)) {
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
		Pool CreatePool(String&& poolName, SizeT maxPoolObjectSize, SizeT maxPoolObjectCount, SizeT startSize = 0) {
			if (maxPoolObjectCount == 0)
			{
				Throw(InvalidArgumentException(SourcePosition, L"maxPoolObjectSize", L"Size can't be null"));
			}

			auto retObj = Pool(MoveRValue(poolName), maxPoolObjectSize, maxPoolObjectCount, startSize ? startSize : maxPoolObjectCount);

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
			return This->activePools.CopyTo<Containers::Array>([] (const Pool& item) {
				return PoolStatistics{
					.Name = item.PoolManagerName,
					.MaxSize = item.MaxPoolObjectSize,
					.MaxCount = item.MaxPoolObjectCount,
					.CurrentCount = item.AvailablePoolObjectCount
				};
				});
		}

	public:
		RefCountClass(PoolManager, This)
	};
}