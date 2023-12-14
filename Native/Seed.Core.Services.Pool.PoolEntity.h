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
	/// Pool entity
	/// </summary>
	class PoolEntity final : public IExtendedDisposable {
	private:
		using FatherRefType = WeakPtr<Containers::ConcurencyDynamicStack<PoolEntity>>;

		struct Data {
			SmartPtr<Containers::Blob> data;
			EnumFlags<EPoolObjectFlags> flags;

			FatherRefType fatherRef;

			RefCountT RefCount;

			Data(SmartPtr<Containers::Blob>&& dataContainer, FatherRefType&& fatherRef, EnumFlags<EPoolObjectFlags> flags) : 
				data(MoveRValue(dataContainer)), 
				flags(flags), 
				fatherRef(MoveRValue(fatherRef)), 
				RefCount(1) 
			{}
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

						This = nullptr;
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

		PoolEntity(
			SmartPtr<Containers::Blob>&& dataContainer, 
			FatherRefType&& fatherRef, 
			EnumFlags<EPoolObjectFlags> flags
		) : This(new Data(
				MoveRValue(dataContainer), 
				MoveRValue(fatherRef), 
				flags)
			)
		{}

	public:
		/// <summary>
		/// Do not use this shit. For internal use only.
		/// </summary>
		explicit PoolEntity() : This(nullptr) {
		}

		/// <summary>
		/// The access operator to fields/methods
		/// </summary>
		/// <returns>A pointer to an object</returns>
		ForceInline Containers::Blob* operator->() {
			return This->data.operator->();
		}

		/// <summary>
		/// Denomination operator
		/// </summary>
		/// <returns>A reference to an object</returns>
		ForceInline Containers::Blob& operator*() {
			return This->data.operator*();
		}

		/// <summary>
		/// The access operator to fields/methods
		/// </summary>
		/// <returns>A pointer to an object</returns>
		ForceInline const Containers::Blob* operator->() const {
			return This->data.operator->();
		}

		/// <summary>
		/// Denomination operator
		/// </summary>
		/// <returns>A reference to an object</returns>
		ForceInline const Containers::Blob& operator*() const {
			return This->data.operator*();
		}

		/// <summary>
		/// An access operator to array elements
		/// </summary>
		/// <param name="Index">Element number</param>
		/// <returns>A reference to element</returns>
		Byte& operator[](SizeT idx) {
			return (*This->data)[idx];
		}

		/// <summary>
		/// An access operator to array elements
		/// </summary>
		/// <param name="Index">Element number</param>
		/// <returns>A reference to element</returns>
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
		friend class Pool;

	public:
		RefCountClass(PoolEntity, This)
	};
}