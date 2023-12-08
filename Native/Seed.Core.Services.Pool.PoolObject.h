#pragma once
#include "Seed.Core.Services.Pool.Base.h"

namespace Seed::Core::Services::Pool
{
	class ObjectsPool;

	/// <summary>
	/// Pool object flags
	/// </summary>
	enum class EPoolObjectFlags {
		Custom,
		InUse
	};

	/// <summary>
	/// Pool object
	/// </summary>
	class PoolObject final : public IExtendedDisposable {
	private:
		using FatherRefType = WeakPtr<Containers::ConcurencyDynamicStack<PoolObject>>;

		struct Data {
			SmartPtr<Containers::Blob> data;
			EnumFlags<EPoolObjectFlags> flags;

			FatherRefType fatherRef;

			RefCountT RefCount;

			Data(SmartPtr<Containers::Blob>&& dataContainer, FatherRefType&& fatherRef, EnumFlags<EPoolObjectFlags> flags) : data(MoveRValue(dataContainer)), flags(flags), fatherRef(MoveRValue(fatherRef)), RefCount(1) {
			}
		};

		Data* This;

	protected:
		virtual bool Dispose() override {
			if (This->flags & EPoolObjectFlags::Custom)
			{
				return true;
			}

			if (This->flags & EPoolObjectFlags::InUse)
			{
				if (This->fatherRef.HasValue)
				{
					This->flags = {};

					if (This->fatherRef->Count != This->fatherRef->BufferSize)
					{
						This->RefCount++;
						This->fatherRef->Push(*this);

						This->RefCount--;
					}
				}
				else
				{
					return true;
				}

				return false;
			}
			else
			{
				return true;
			}
		}

		PoolObject(SmartPtr<Containers::Blob>&& dataContainer, FatherRefType&& fatherRef, EnumFlags<EPoolObjectFlags> flags) : This(new Data(MoveRValue(dataContainer), MoveRValue(fatherRef), flags)) {
		}

	public:
		/// <summary>
		/// Do not use this shit. For internal use only.
		/// </summary>
		explicit PoolObject() : This(nullptr) {
		}

		/// <summary>
		/// The access operator to fields/methods
		/// </summary>
		/// <returns>A pointer to an object</returns>
		ForceInline Containers::Blob* operator->() {
			return This->data.operator->();
		}

		ForceInline Containers::Blob& operator*() {
			return This->data.operator*();
		}

		ForceInline const Containers::Blob* operator->() const {
			return This->data.operator->();
		}

		ForceInline const Containers::Blob& operator*() const {
			return This->data.operator*();
		}

		Byte& operator[](SizeT idx) {
			return (*This->data)[idx];
		}

		const Byte& operator[](SizeT idx) const {
			return (*This->data)[idx];
		}

	public:
		/// <summary>
		/// Returns pool object maximum size
		/// </summary>
		/// <returns>Pool object maximum size</returns>
		SizeT GetMaxSize() {
			return This->data->Count;
		}

		/// <summary>
		/// Pool object maximum size
		/// </summary>
		SizeT Property(GetMaxSize) MaxSize;

	public:
		friend class ObjectsPool;

	public:
		RefCountClass(PoolObject, This)
	};
}