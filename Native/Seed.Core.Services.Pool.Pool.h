#pragma once
#include "Seed.Core.Services.Pool.Base.h"
#include "Seed.Core.Services.Pool.PoolEntity.h"

namespace Seed::Core::Services::Pool
{
	/// <summary>
	/// Pool objects container
	/// </summary>
	class Pool final : public IDisposable {
	private:
		using ContainerType = Containers::ConcurencyDynamicStack<PoolEntity>;

		struct Data {
			String poolName;

			SmartPtr<ContainerType> pool;
			SizeT maxPoolObjectSize;

			Containers::ConcurencyList<Pool>::Iterator it;
			RefCountT RefCount;

			Data(String&& name, SizeT maxPoolObjectSize, SizeT maxPoolObjectCount) :
				poolName(MoveRValue(name)),
				pool(new ContainerType(maxPoolObjectCount)),
				maxPoolObjectSize(maxPoolObjectSize),
				it(),
				RefCount(1) {
			}
		};

		Data* This;

	protected:
		virtual void Dispose() override {
			This->RefCount = Math::Max<SizeT>();
			This->it.Remove(false);
			This->pool->RemoveAll();
		};

		Pool(String&& name, SizeT maxPoolObjectSize, SizeT maxPoolObjectCount, SizeT startSize) :
			This(new Data(MoveRValue(name), maxPoolObjectSize, maxPoolObjectCount)) {
			for (SizeT i = 0; i < startSize; i++)
			{
				This->pool->Push(PoolEntity(new Containers::Blob(maxPoolObjectSize), This->pool.AsWeak(), {}));
			}
		}

	public:
		/// <summary>
		/// Returns ready to use available or created pool object.
		/// </summary>
		/// <returns>Pool object</returns>
		PoolEntity GetOrCreate() {
			if (This->pool->Count != 0)
			{
				auto retObj = This->pool->Pop();
				retObj.This->flags = EPoolObjectFlags::InUse;
				return retObj;
			}
			else
			{
				return PoolEntity(new Containers::Blob(This->maxPoolObjectSize), This->pool.AsWeak(), EPoolObjectFlags::InUse);
			}
		}

		/// <summary>
		/// Returns ready to use available or created pool object.
		/// </summary>
		/// <param name="minRequiredSize">Object minimal buffer size</param>
		/// <returns>Pool object</returns>
		PoolEntity GetOrCreate(SizeT minRequiredSize) {
			if (minRequiredSize > This->maxPoolObjectSize)
			{
				return PoolEntity(new Containers::Blob(minRequiredSize), This->pool.AsWeak(), EPoolObjectFlags::Custom | EPoolObjectFlags::InUse);
			}

			return GetOrCreate();
		};

	public:
		/// <summary>
		/// Remove all objects from pool
		/// </summary>
		void FlushPool() {
			auto count = This->pool->BufferSize;
			This->pool->RemoveAll();
			ExtendPool(count);
		}

		/// <summary>
		/// Extends pool to specified size if it's bigger then current
		/// </summary>
		/// <param name="newObjectCount">Desired pool size</param>
		void ExtendPool(SizeT newObjectCount) {
			if (This->pool->BufferSize < newObjectCount)
			{
				This->pool->ExtendBuffer(newObjectCount - This->pool->BufferSize);
			}
		}

		/// <summary>
		/// Changes pool max size to it's current number of free elements
		/// </summary>
		void Shrink() {
			This->pool->Shrink();
		}

	public:
		/// <summary>
		/// Returns pool manager name
		/// </summary>
		/// <returns>Pool manager name</returns>
		String GetPoolManagerName() const {
			return This->poolName;
		}

		/// <summary>
		/// Returns pool manager objects maximum non-custom size
		/// </summary>
		/// <returns>Pool manager objects maximum non-custom size</returns>
		SizeT GetMaxPoolObjectSize() const {
			return This->maxPoolObjectSize;
		}

		/// <summary>
		/// Returns pool manager objects maximum non-custom count
		/// </summary>
		/// <returns>Pool manager objects maximum non-custom count</returns>
		SizeT GetMaxPoolObjectCount() const {
			return This->pool->BufferSize;
		}

		/// <summary>
		/// Returns pool manager objects available non-custom count
		/// </summary>
		/// <returns>Pool manager objects available non-custom count</returns>
		SizeT GetAvailablePoolObjectCount() const {
			return This->pool->Count;
		}

		/// <summary>
		/// Pool manager objects maximum non-custom size
		/// </summary>
		SizeT Property(GetMaxPoolObjectSize) MaxPoolObjectSize;

		/// <summary>
		/// Pool manager objects maximum non-custom count
		/// </summary>
		SizeT Property(GetMaxPoolObjectCount) MaxPoolObjectCount;

		/// <summary>
		/// Pool manager objects available non-custom count
		/// </summary>
		SizeT Property(GetAvailablePoolObjectCount) AvailablePoolObjectCount;

		/// <summary>
		/// Pool manager name
		/// </summary>
		const String Property(GetPoolManagerName) PoolManagerName;

	public:
		friend class PoolManager;

	public:
		RefCountClass(Pool, This)
	};
}