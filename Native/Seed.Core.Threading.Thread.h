#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Threading.Event.h"
#include "Seed.Core.Containers.Delegate.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			class SeedKernelCall Thread final : IDisposable{
			private:
				friend class ThreadableThread;
			private:
				struct Data {
					RefCountT RefCount;
					volatile Delegate delegate;
					volatile String name;
					volatile ID TID;
					volatile EThreadabledStates State;
					Handle hThread;
					Event* AbortEvent;
					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator This;

					Data(const String& Name, Delegate&& delegate) :
						name(Name),
						delegate(MoveRValue(delegate)),
						State(EThreadabledStates::NotStarted),
						hThread(nullptr),
						TID(0),
						This(),
						AbortEvent(nullptr) ,
						RefCount(1)
					{}

					Data(String&& Name, Delegate&& delegate) :
						name(Name),
						delegate(MoveRValue(delegate)),
						State(EThreadabledStates::NotStarted),
						hThread(nullptr),
						TID(0),
						This(),
						AbortEvent(nullptr),
						RefCount(1)
					{}
				};
			private:
				Data* This = nullptr;
			private:
				friend extern void RegisterThread(Thread* thread);
				friend extern void UnRegisterThread(Thread* thread);
				friend extern volatile Delegate& ThreadEntryPoint(Thread* thread);
				friend extern bool MainThread(Delegate&& delegate);
			private:
				void RefreshName();
			protected:
				virtual void Dispose() override final;
			public:
				Thread(Delegate&& delegate) : This(new Data(L"Unnamed Thread", MoveRValue(delegate))) {}
				Thread(String&& Name, Delegate&& delegate) : This(new Data(MoveRValue(Name), MoveRValue(delegate))) {}
				Thread(const String& Name, Delegate&& delegate) : This(new Data(Name, MoveRValue(delegate))) {}
			public:
				Thread& Start();
				Thread& Join();
				Thread& Join(DateTime timeOut);
				Thread& Interupt();
				Thread& Suspend();
			public:
				template<typename T>
				T GetThreadResult() {
					return This->delegate.GetResult<T>();
				}
			public:
				operator volatile IThreadable& () { if (This->This != nullptr) return **This->This; else Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read)); }
				operator const volatile IThreadable& () const { if (This->This != nullptr) return **This->This; else Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read)); }
				operator SizeT () const { return This->TID; }
				operator Boolean () const { return This->TID != 0; }
			public:
				void SetName(const String& Name) { const_cast<String&>(This->name) = Name; RefreshName(); }
				void SetName(String&& Name) { const_cast<String&>(This->name) = MoveRValue(Name); RefreshName(); }
				const String& GetName() { return const_cast<String&>(This->name); }
			public:
				RefCountClass(Thread, This)
			};
		}
	}
}