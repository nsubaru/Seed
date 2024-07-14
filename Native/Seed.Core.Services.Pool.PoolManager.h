#pragma once
#include "Seed.Core.Services.Pool.Base.h"
#include "Seed.Core.Services.Pool.Pool.h"

#include "Seed.Core.Services.BootupServicesManager.h"
#include "Seed.Core.Services.InternalSensors.h"

namespace Seed::Core::Services::Pool
{
	/// <summary>
	/// Pool builder service
	/// </summary>
	class PoolManager final : public IDisposable, IBootUpService<PoolManager, Services::InternalSensorsService> {
		struct Data {
			Events::LocalEventSubscription<bool> subscription;

			Containers::ConcurencyList<Pool> activePools;

			RefCountT RefCount;

			Data() 
				: subscription(), activePools(), RefCount(1)
			{}
		};

		Data* This;

	protected:
		virtual void Dispose() override;

		PoolManager(Data* This) : This(This) {
			++This->RefCount;
		}

	private:
		class LowCommitConditionEventTarget final : public Events::ILocalEventCallback<bool> {
		private:
			Data* This;

		protected:
			virtual void Dispose() {
			}

		public:
			LowCommitConditionEventTarget(Data* This) : This(This) {
			}

			virtual void onEvent(bool isUp) override;

		public:
			~LowCommitConditionEventTarget() {}
		};

	public:
		/// <summary>
		/// Service constructor
		/// </summary>
		PoolManager(Services::InternalSensorsService sensorsService);

	public:
		/// <summary>
		/// Creates new pool
		/// </summary>
		/// <param name="poolName">Pool name</param>
		/// <param name="maxPoolObjectSize">Maximum pool object buffer size</param>
		/// <param name="maxPoolObjectCount">Maximum pool object buffer count</param>
		/// <returns>New pool</returns>
		Pool CreatePool(String&& poolName, SizeT maxPoolObjectSize, SizeT maxPoolObjectCount, SizeT startSize = 0);

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
		Containers::Array<PoolStatistics> GetStats();

	public:
		RefCountClass(PoolManager, This)
	};
}