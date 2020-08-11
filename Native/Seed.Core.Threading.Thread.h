#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Threading.Event.h"
#include "Seed.Core.Containers.Delegate.h"

#pragma warning(push)
#pragma warning(disable: 4251)

namespace Seed {
	inline namespace Core {
		namespace Threading {
			class Thread final : IDisposable{
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
					SynchronizeEvent* AbortEvent;
					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator This;

					ForceInline Data(const String& Name, Delegate&& delegate) :
						name(Name),
						delegate(MoveRValue(delegate)),
						State(EThreadabledStates::NotStarted),
						hThread(nullptr),
						TID(0),
						This(),
						AbortEvent(nullptr) ,
						RefCount(1)
					{}

					ForceInline Data(String&& Name, Delegate&& delegate) :
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
				SeedKernelCall friend void RegisterThread(Thread* thread);
				SeedKernelCall friend void UnRegisterThread(Thread* thread);
				SeedKernelCall friend volatile Delegate& ThreadEntryPoint(Thread* thread);
				SeedKernelCall friend bool MainThread(Delegate&& delegate);

				friend void KillThread(Thread& target, UInt32 statusCode);
			private:
				SeedKernelCall void RefreshName();
			protected:
				virtual void Dispose() override final;
			public:
				ForceInline Thread(Delegate&& delegate) : This(new Data(L"Unnamed Thread", MoveRValue(delegate))) {}
				ForceInline Thread(String&& Name, Delegate&& delegate) : This(new Data(MoveRValue(Name), MoveRValue(delegate))) {}
				ForceInline Thread(const String& Name, Delegate&& delegate) : This(new Data(Name, MoveRValue(delegate))) {}
			public:
				SeedKernelCall Thread& Start();
				SeedKernelCall Thread& Join();
				SeedKernelCall Thread& Join(DateTime timeOut);
				SeedKernelCall Thread& Interupt();
				SeedKernelCall Thread& Suspend();
			public:
				template<typename T>
				ForceInline T GetThreadResult() {
					return This->delegate.GetResult<T>();
				}
			public:
				ForceInline operator volatile IThreadable& () { if (This->This != nullptr) return **This->This; else Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read)); }
				ForceInline operator const volatile IThreadable& () const { if (This->This != nullptr) return **This->This; else Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read)); }
				ForceInline operator ID () const { return This->TID; }
				ForceInline operator Boolean () const { return This->TID != 0; }
			public:
				ForceInline void SetName(const String& name) { const_cast<String&>(This->name) = name; RefreshName(); }
				ForceInline void SetName(String&& name) { const_cast<String&>(This->name) = MoveRValue(name); RefreshName(); }
				ForceInline const String& GetName() { return const_cast<String&>(This->name); }

				ForceInline EThreadabledStates GetThreadState() { return this->This->State; }

				ForceInline ID GetTID() { return this->This->TID; }

				String Property(GetName, SetName) Name;
				EThreadabledStates Property(GetThreadState) ThreadState;
				ID Property(GetTID) TID;
			public:
				RefCountClass(Thread, This)
			};
		}
	}
}

#pragma warning(pop)