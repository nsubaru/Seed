#pragma once
#include "Seed.Core.Stage1.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Services
		{
			class SystemTaskPoolService final : IDisposable {
			private:
				struct Data {
					Threading::ThreadPool pool;

					RefCountT RefCount;

					Data() : pool(L"System task pool service"), RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {}
			public:
				SystemTaskPoolService() : This(new Data()) {};
			public:
				Threading::JobPool PoolJob(const String& name, Delegate&& action) {
					return This->pool.Dispatch(name, MoveRValue(action));
				}

				Threading::JobPool PoolJob(String&& name, Delegate&& action) {
					return This->pool.Dispatch(MoveRValue(name), MoveRValue(action));
				}
			public:
				UInt64 GetJobsCount() const {
					return This->pool.GetTasksCount();
				}
			public:
				RefCountClass(SystemTaskPoolService, This);
			};
		}
	}
}