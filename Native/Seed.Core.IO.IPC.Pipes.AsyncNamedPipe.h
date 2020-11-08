#pragma once
#include "Seed.Core.IO.IPC.Pipes.Base.h"
#include "Seed.Core.IO.IPC.Pipes.NamedPipe.h"

#include "Seed.Core.Threading.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			namespace IPC {
				namespace Pipes {
					class IAsyncNamedPipeServerInstance {
					public:
						virtual void CloseConnection() = 0;
					public:
						virtual ~IAsyncNamedPipeServerInstance() = 0 {}
					};

					template<typename TBufferType>
					class AsyncNamedPipeClient;

					template<typename TBufferType>
					class AsyncNamedPipeServer;

					template<typename TBufferType>
					class AsyncNamedPipeServerInstance;

					namespace Details {
						class BaseAsyncNamedPipeClient final : protected IDisposable {
						protected:
							Handle hPipe;
							String pipeName;
							Boolean isNoCache;
							PipeMode mode;
							Threading::UnifiedThreadPoolIoTask* poolTask;

							RefCountT RefCount;
						protected:
							virtual void Dispose();
						protected:
							BaseAsyncNamedPipeClient(String&& pipeName, PipeMode mode, Boolean isNoCache);
						protected:
							void InitAsyncRead(IRPBase*&& irp, void* buffer, SizeT size, void* ioStatusBlk);
							void InitAsyncWrite(IRPBase*&& irp, void* buffer, SizeT size, void* ioStatusBlk);
						protected:
							void CloseConnection();
							void Flush();
							void WaitClosing() const;
							void WaitClosing(Time::TimeSpan timeout) const;
						protected:
							bool IsValid() { return hPipe != nullptr; }
						public:
							template<typename TBufferType>
							friend class AsyncNamedPipeClient;
						};

						class BaseAsyncNamedPipeServer final : protected IDisposable {
						protected:
							String pipeName;
							PipeMode mode;
							Boolean isNoCache;
							UInt8 maxConnectionCount;

							Containers::ConcurencyList<SmartPtr<IAsyncNamedPipeServerInstance>> instances;

							RefCountT RefCount;
						protected:
							virtual void Dispose();
						protected:
							BaseAsyncNamedPipeServer(String&& pipeName, PipeMode mode, Boolean isNoCache, UInt8 maxConnectionCount)
								: pipeName(MoveRValue(pipeName)), mode(mode), isNoCache(isNoCache), maxConnectionCount(maxConnectionCount), instances(), RefCount(1) {}
						protected:
							Handle CreateServerPipeHandle();
						protected:
							bool IsValid() const { return instances.Count < maxConnectionCount; }
						protected:
							void CloseAllInstances();
						public:
							template<typename TBufferType>
							friend class AsyncNamedPipeServer;
						};

						class BaseAsyncNamedPipeServerInstance final : protected IDisposable {
						protected:
							Handle hPipe;
							String pipeName;
							PipeMode mode;
							Boolean isNoCache;
							Threading::UnifiedThreadPoolIoTask* poolTask;

							Containers::ConcurencyList<SmartPtr<IAsyncNamedPipeServerInstance>>::Iterator serverRef;

							RefCountT RefCount;
						protected:
							virtual void Dispose();
						protected:
							BaseAsyncNamedPipeServerInstance(
								String&& pipeName,
								Handle hPipe,
								PipeMode mode,
								Boolean isNoCache
							) : 
								pipeName(MoveRValue(pipeName)), 
								hPipe(hPipe), 
								mode(mode), 
								isNoCache(isNoCache),
								poolTask(nullptr), 
								serverRef(), 
								RefCount(1) 
							{}
						protected:
							void InitAsyncConnect(IRPBase*&& irp, void* ioStatusBlk);
						protected:
							void InitAsyncRead(IRPBase*&& irp, void* buffer, SizeT size, void* ioStatusBlk);
							void InitAsyncWrite(IRPBase*&& irp, void* buffer, SizeT size, void* ioStatusBlk);
						protected:
							void CloseConnection();
							void Flush();
							void WaitClosing() const;
							void WaitClosing(Time::TimeSpan timeout) const;
						protected:
							bool IsValid() { return hPipe != nullptr; }
						public:
							template<typename TBufferType>
							friend class AsyncNamedPipeServerInstance;
						};
					}

					template<typename TBufferType>
					class IAsyncNamedPipe : public IO::IBaseAsyncIO<TBufferType> {
					public:

						virtual void CloseConnection() = 0;

						virtual void WaitClosing() const = 0;

						virtual void WaitClosing(Time::TimeSpan timeout) const = 0;

						virtual ~IAsyncNamedPipe() = 0 {};
					};

					template<typename TDataType>
					class AsyncNamedPipeClient final : public IAsyncNamedPipe<TDataType> {
					private:
						Details::BaseAsyncNamedPipeClient* This;
					protected:
						virtual void Dispose() override {
							This->Dispose();
						}
					protected:
						virtual Handle GetHandle() {
							return This->hPipe;
						}
					protected:
						AsyncNamedPipeClient(String&& pipeName, PipeMode mode, Boolean isNoCache, const Threading::UnifiedThreadPool& ownerPool)
							: This(new Details::BaseAsyncNamedPipeClient(MoveRValue(pipeName), mode, isNoCache)
						) {
							This->poolTask = new Threading::UnifiedThreadPoolIoTask(RegisterNtIoObjectInThreadPool(ownerPool, This->hPipe));
						}
					public:
						/// <summary>
						/// Ініціалізація асинхроної операції читання
						/// </summary>
						/// <param name="data">Буфер даних</param>
						/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
						/// <returns>Об'єкт задачі I/O</returns>
						virtual IOTask AsyncRead(TDataType&& data, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) {
							if (!IsValid())
								Throw(InvalidHandleUsage(ExceptionPosition));

							if (This->mode != PipeMode::In && This->mode != PipeMode::Both)
								Throw(InvalidPipeException(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								//Get buffer and its size
								void* buffer = (void*)(const Byte*)data;
								SizeT size = data.Count;
								//create irp packet
								irp = new ReadIRP<TDataType>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeClient(*this)),
									onFinishReadAction, 
									MoveRValue(data));
								//create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									//notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									//Perfome os specific read operations
									This->InitAsyncRead(MoveRValue(irp), buffer, size, GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									//If read failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									//and rethrow exception as we don't how handle it
									ReThrow;
								}

								//return task object
								return task;
							}
							catch (...) {
								if (irp != nullptr)
									delete irp;
								ReThrow;
							}
						}

						/// <summary>
						/// Ініціалізація асинхроної операції запису
						/// </summary>
						/// <param name="data">Буфер даних</param>
						/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
						/// <returns>Об'єкт задачі I/O</returns>
						virtual IOTask AsyncWrite(const TDataType&& data, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) {
							if (!IsValid())
								Throw(InvalidHandleUsage(ExceptionPosition));

							if (This->mode != PipeMode::Out && This->mode != PipeMode::Both)
								Throw(InvalidPipeException(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								//Get buffer and its size
								void* buffer = (void*)(const Byte*)data;
								SizeT size = data.Count;
								//create irp packet
								irp = new WriteIRP<TDataType>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeClient(*this)),
									onFinishWriteAction, 
									MoveRValue(data));
								//create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									//notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									//Perfome os specific read operations
									This->InitAsyncWrite(MoveRValue(irp), buffer, size, GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									//If read failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									//and rethrow exception as we don't how handle it
									ReThrow;
								}

								//return task object
								return task;
							}
							catch (...) {
								if (irp != nullptr)
									delete irp;
								ReThrow;
							}
						}
					public:
						/// <summary>
						/// Перевірка чи об'єкт коректний
						/// </summary>
						/// <returns>true якщо об'єкт допускає I/O операції</returns>
						virtual bool IsValid() {
							return This->hPipe != nullptr;
						}
					public:
						virtual void CloseConnection() { This->CloseConnection(); }
						virtual void WaitClosing() const { This->WaitClosing(); }
						virtual void WaitClosing(Time::TimeSpan timeout) const { This->WaitClosing(timeout); }
					public:
						virtual const Seed::Core::Threading::UnifiedThreadPoolIoTask& GetIoPoolTask() const {
							return *This->poolTask;
						}
					public:
						template<typename UDataType>
						friend AsyncNamedPipeClient<UDataType> AsyncConnectPipe(
							const Threading::UnifiedThreadPool& ownerPool, 
							const String& name, 
							PipeMode mode = PipeMode::Both, 
							Boolean isNoCache = true);
					public:
						RefCountClass(AsyncNamedPipeClient, This)
					};

					template<typename TBufferType>
					class AsyncNamedPipeServerInstance;

					template<typename TBufferType>
					class AsyncNamedPipeServer final : IDisposable {
					private:
						Details::BaseAsyncNamedPipeServer* This;
					protected:
						virtual void Dispose() override {
							This->Dispose();
						}
					protected:
						AsyncNamedPipeServer(String pipeName, PipeMode mode, Boolean isNoCache, UInt8 maxConnectionCount)
						: This(new Details::BaseAsyncNamedPipeServer(MoveRValue(pipeName), mode, isNoCache, maxConnectionCount)) {}
					public:
						IOTask AsyncConnect(SmartPtr<IFinishConnectCallback<AsyncNamedPipeServerInstance<TBufferType>>> onFinishConnectAction) {
							//create pipe object
							auto pipe = AsyncNamedPipeServerInstance<TBufferType>(This->CreateServerPipeHandle(), This->pipeName, This->mode, This->isNoCache);
							//store it in list and save ref in object
							pipe.SetInstaceRef(This->instances.AddLast(new AsyncNamedPipeServerInstance(pipe)));
							//call pipe async connect
							return pipe.AsyncConnect(onFinishConnectAction);
						}

						void CloseAllInstances() {
							This->CloseAllInstances();
						}

						void DestroyPipe() {
							Dispose();
						}

						SizeT InstanceCount() { return This == nullptr ? 0 : This->instances.Count; }

						bool IsValid() const { return This->IsValid(); }
					public:
						RefCountClass(AsyncNamedPipeServer, This)
					public:
						template<typename UBufferType>
						friend AsyncNamedPipeServer<UBufferType> CreateAsyncPipe(
							const String& name, 
							UInt32 maxInstanceCount, 
							PipeMode mode = PipeMode::Both, 
							Boolean isNoCache = true);
					};

					template<typename TDataType>
					class AsyncNamedPipeServerInstance final : 
						public IAsyncNamedPipe<TDataType>,
						public IAsyncNamedPipeServerInstance, 
						public IBaseConnectableAsyncIO<AsyncNamedPipeServerInstance<TDataType>> {
					private:
						Details::BaseAsyncNamedPipeServerInstance* This;
					protected:
						virtual void Dispose() override {
							This->Dispose();
						}
					protected:
						virtual Handle GetHandle() {
							return This->hPipe;
						}
					protected:
						AsyncNamedPipeServerInstance(Handle hPipe, const String& name, PipeMode mode, Boolean isNoCache)
							: This(new Details::BaseAsyncNamedPipeServerInstance(String(name), hPipe, mode, isNoCache))
						{}
					protected:
						void SetInstaceRef(Containers::ConcurencyList<SmartPtr<IAsyncNamedPipeServerInstance>>::Iterator serverRef) {
							This->serverRef = serverRef;
							//Must as serverRef container ref to this object in server pipe
							--This->RefCount;
						}
					protected:
						virtual IOTask AsyncConnect(SmartPtr<IFinishConnectCallback<AsyncNamedPipeServerInstance<TDataType>>> onFinishConnectAction) {
							if (IsValid())
								Throw(InvalidHandleUsage(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								//create irp packet
								irp = new ConnectIRP<AsyncNamedPipeServerInstance>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeServerInstance(*this)),
									onFinishConnectAction,
									AsyncNamedPipeServerInstance(*this));
								//create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									//notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									//Perfome os specific connect operations
									This->InitAsyncConnect(MoveRValue(irp), GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									//If connect failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									//and rethrow exception as we don't how handle it
									ReThrow;
								}

								//return task object
								return task;
							}
							catch (...) {
								if (irp != nullptr)
									delete irp;
								ReThrow;
							}
						}
					public:
						/// <summary>
						/// Ініціалізація асинхроної операції читання
						/// </summary>
						/// <param name="data">Буфер даних</param>
						/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
						/// <returns>Об'єкт задачі I/O</returns>
						virtual IOTask AsyncRead(TDataType&& data, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) {
							if (!IsValid())
								Throw(InvalidHandleUsage(ExceptionPosition));

							if (This->mode != PipeMode::In && This->mode != PipeMode::Both)
								Throw(InvalidPipeException(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								//Get buffer and its size
								void* buffer = (void*)(const Byte*)data;
								SizeT size = data.Count;
								//create irp packet
								irp = new ReadIRP<TDataType>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeServerInstance(*this)), 
									onFinishReadAction, 
									MoveRValue(data));
								//create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									//notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									//Perfome os specific read operations
									This->InitAsyncRead(MoveRValue(irp), buffer, size, GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									//If read failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									//and rethrow exception as we don't how handle it
									ReThrow;
								}

								//return task object
								return task;
							}
							catch (...) {
								if (irp != nullptr)
									delete irp;
								ReThrow;
							}
						}

						/// <summary>
						/// Ініціалізація асинхроної операції запису
						/// </summary>
						/// <param name="data">Буфер даних</param>
						/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
						/// <returns>Об'єкт задачі I/O</returns>
						virtual IOTask AsyncWrite(const TDataType&& data, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) {
							if (!IsValid())
								Throw(InvalidHandleUsage(ExceptionPosition));

							if (This->mode != PipeMode::Out && This->mode != PipeMode::Both)
								Throw(InvalidPipeException(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								//Get buffer and its size
								void* buffer = (void*)(const Byte*)data;
								SizeT size = data.Count;
								//create irp packet
								irp = new WriteIRP<TDataType>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeServerInstance(*this)),
									onFinishWriteAction, 
									MoveRValue(data));
								//create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									//notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									//Perfome os specific read operations
									This->InitAsyncWrite(MoveRValue(irp), buffer, size, GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									//If read failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									//and rethrow exception as we don't how handle it
									ReThrow;
								}

								//return task object
								return task;
							}
							catch (...) {
								if (irp != nullptr)
									delete irp;
								ReThrow;
							}
						}
					public:
						/// <summary>
						/// Перевірка чи об'єкт коректний
						/// </summary>
						/// <returns>true якщо об'єкт допускає I/O операції</returns>
						virtual bool IsValid() {

						}
					public:
						virtual void CloseConnection() { This->CloseConnection(); }
						virtual void WaitClosing() const { This->WaitClosing(); }
						virtual void WaitClosing(Time::TimeSpan timeout) const { This->WaitClosing(timeout); }
					public:
						virtual const Seed::Core::Threading::UnifiedThreadPoolIoTask& GetIoPoolTask() const {
							return *This->poolTask;
						}
					public:
						virtual void CloseConnection();
					public:
						RefCountClass(AsyncNamedPipeServerInstance, This)
					public:
						template<typename TBufferType>
						friend class AsyncNamedPipeServer;
					};


					template<typename TBufferType>
					AsyncNamedPipeClient<TBufferType> AsyncConnectPipe(
						const Threading::UnifiedThreadPool& ownerPool, 
						const String& name, 
						PipeMode mode = PipeMode::Both, 
						Boolean isNoCache = true)
					{
						return AsyncNamedPipeClient<TBufferType>(String(name), mode, isNoCache, ownerPool);
					}

					template<typename TBufferType>
					AsyncNamedPipeServer<TBufferType> CreateAsyncPipe(
						const String& name, 
						UInt32 maxInstanceCount, 
						PipeMode mode = PipeMode::Both,
						Boolean isNoCache = true)
					{
						return AsyncNamedPipeServer(String(name), mode, maxInstanceCount);
					}
				}
			}
		}
	}
}