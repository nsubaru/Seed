#pragma once
#include "Seed.Core.Containers.Internals.h"
#include "Seed.Core.Memory.h"

namespace Seed {
	inline namespace Core {
		///<summary>Класс контейнер для об'єкта функтора</summary>
		template<typename Heap, typename Callback>
		class BaseCallbackContainer : virtual public Memory::Allocator<Heap> {
			struct Data : virtual public Memory::Allocator<Heap> {
				RefCountT RefCount;
				Callback * callback;

				Data(Callback *&& callback)
					:callback(callback), RefCount(1)
				{}
			};

			Data * This;
		public:
			BaseCallbackContainer(Callback *&& callback)
				:This(new Data(MoveRValue(callback)))
			{}

			CopyAndMoveSemantic(BaseCallbackContainer, This, delete This->callback);

			const Callback * const operator->() {
				return This->callback;
			}
		};
	}
}