// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Seed.Core.Services.h"

#include "Seed.Core.Testing.Tests.Services.Pool.h"

namespace Seed::Core::Testing::Test::Services::Pool
{
	bool PoolStartTest::Execute() {
		auto service = Core::Services::GetGlobalIOC()
			.Resolve<Core::Services::Pool::PoolManager>();

		auto pool = service.CreatePool(L"Test pool", 5, 1);

		auto poolObj = pool.GetOrCreate();

		return true;
	}

	bool PoolCreateDestTest::Execute() {
		auto service = Core::Services::GetGlobalIOC()
			.Resolve<Core::Services::Pool::PoolManager>();

		auto pool = service.CreatePool(L"Test pool", 5, 1);

		do
		{
			auto poolObj = pool.GetOrCreate();

			for (SizeT i = 0; i < poolObj.MaxSize; i++)
			{
				poolObj[i] = 1;
			}
		}
		while (false);

		auto poolObj = pool.GetOrCreate();

		for (SizeT i = 0; i < poolObj.MaxSize; i++)
		{
			Asserts::AssertEqual(1, poolObj[i], SourcePosition);
		}

		return true;
	}

	bool PoolCustomTest::Execute() {
		auto service = Core::Services::GetGlobalIOC()
			.Resolve<Core::Services::Pool::PoolManager>();

		auto pool = service.CreatePool(L"Test pool", 5, 1);
		auto poolObj = pool.GetOrCreate();

		auto poolObjCustom = pool.GetOrCreate(10);

		Asserts::AssertNotEqual(poolObj.MaxSize, poolObjCustom.MaxSize, SourcePosition);

		return true;
	}

	bool PoolOverflowTest::Execute() {
		auto service = Core::Services::GetGlobalIOC()
			.Resolve<Core::Services::Pool::PoolManager>();

		auto pool = service.CreatePool(L"Test pool", 5, 1);

		auto poolObj = pool.GetOrCreate();

		Asserts::AssertEqual(pool.AvailablePoolObjectCount, 0, SourcePosition);

		auto poolObjOvr = pool.GetOrCreate();

		return true;
	}

	bool PoolFlushTest::Execute() {
		auto service = Core::Services::GetGlobalIOC()
			.Resolve<Core::Services::Pool::PoolManager>();

		auto pool = service.CreatePool(L"Test pool", 5, 2);

		do
		{
			auto poolObj = pool.GetOrCreate();

			Asserts::AssertEqual(pool.AvailablePoolObjectCount, 1, SourcePosition);

			pool.FlushPool();

			Asserts::AssertEqual(pool.AvailablePoolObjectCount, 0, SourcePosition);
		}
		while (false);

		Asserts::AssertEqual(pool.AvailablePoolObjectCount, 1, SourcePosition);

		return true;
	}

	bool PoolExtendTest::Execute() {
		auto service = Core::Services::GetGlobalIOC()
			.Resolve<Core::Services::Pool::PoolManager>();

		auto pool = service.CreatePool(L"Test pool", 5, 1);

		Asserts::AssertEqual(pool.MaxPoolObjectCount, 1, SourcePosition);

		pool.ExtendPool(5);

		Asserts::AssertEqual(pool.MaxPoolObjectCount, 5, SourcePosition);

		return true;
	}

	bool PoolShrinkTest::Execute() {
		auto service = Core::Services::GetGlobalIOC()
			.Resolve<Core::Services::Pool::PoolManager>();

		auto pool = service.CreatePool(L"Test pool", 5, 5, 1);

		pool.Shrink();

		Asserts::AssertEqual(pool.MaxPoolObjectCount, 1, SourcePosition);

		return true;
	}
}