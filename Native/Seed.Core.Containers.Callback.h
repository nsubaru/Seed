#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.Internals.h"
#include "Seed.Core.Memory.h"

namespace Seed {
	inline namespace Core {
		///<summary>Класс контейнер для об'єкта функтора</summary>
		template<typename Heap, typename Callback>
		class BaseCallbackContainer : public IDisposable, virtual public Memory::Allocator<Heap> {
			struct Data : virtual public Memory::Allocator<Heap> {
				RefCountT RefCount;
				Callback* callback;

				Data(Callback*&& callback)
					:callback(callback), RefCount(1)
				{}
			};

			Data* This;
		private:
			void Dispose() { delete This->callback; }
		public:
			BaseCallbackContainer(Callback*&& callback)
				:This(new Data(MoveRValue(callback)))
			{}

			RefCountClass(BaseCallbackContainer, This);

			const Callback* const operator->() {
				return This->callback;
			}
		};

		template<typename Signature>
		using Callback = BaseCallbackContainer<Seed::Core::Memory::Heap, Signature>;

		template<typename Signature>
		using SensitiveCallback = BaseCallbackContainer<Seed::Core::Memory::SensitiveDataHeap, Signature>;
	}
}