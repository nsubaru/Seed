#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Base.Interlocked.h"
#include "Seed.Core.Containers.Array.h"
#include "Seed.Core.Threading.LockPrimitives.h"

namespace Seed {
	inline namespace Core {
		//template<typename THeap, typename T>
		//class BaseSList {
		//private:
		//	struct Node {
		//		T Value;
		//		Node* Next;
		//	};
		//	struct Root {
		//		Node* Head;
		//		Threading::SRWLock Lock;
		//		RefCountT Count;
		//		RefCountT RefCount;
		//		Root() :
		//			Head(nullptr), 
		//			Lock(), 
		//			Count(0), 
		//			RefCount(1) 
		//		{}
		//	};
		//private:
		//	Root This;
		//public:
		//	BaseSList(): This(new Root) {}
		//	BaseSList& operator+=(const T& Value);
		//	BaseSList& operator+=(T&& Value);
		//	void AddFirst(const T& Value);
		//	void AddFirst(T&& Value);
		//	void AddLast(const T& Value);
		//	void AddLast(T&& Value);
		//};
	}
}