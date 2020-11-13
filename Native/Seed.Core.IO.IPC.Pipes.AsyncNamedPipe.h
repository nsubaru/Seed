#pragma once
#include "Seed.Core.IO.IPC.Pipes.Base.h"
#include "Seed.Core.IO.IPC.Pipes.NamedPipe.h"

#include "Seed.Core.Threading.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			namespace IPC {
				namespace Pipes {
					/// <summary>
					/// Інтерфейс серверного з'єднання іменованого каналу
					/// </summary>
					class IAsyncNamedPipeServerInstance {
					public:
						/// <summary>
						/// Закриття з'єднання
						/// </summary>
						virtual void CloseConnection() = 0;
					public:
						/// <summary>
						/// Деструктор
						/// </summary>
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

					/// <summary>
					/// Інтерфейс асинхроного каналу
					/// </summary>
					/// <typeparam name="TBufferType"></typeparam>
					template<typename TBufferType>
					class IAsyncNamedPipe : public IO::IBaseAsyncIO<TBufferType> {
					public:

						virtual void CloseConnection() = 0;

						virtual void WaitClosing() const = 0;

						virtual void WaitClosing(Time::TimeSpan timeout) const = 0;

						virtual ~IAsyncNamedPipe() = 0 {};
					};

					/// <summary>
					/// Клієнтський асинхроний канал
					/// </summary>
					/// <typeparam name="TDataType">Тип блоку даних</typeparam>
					template<typename TDataType>
					class AsyncNamedPipeClient final : public IAsyncNamedPipe<TDataType> {
					private:
						Details::BaseAsyncNamedPipeClient* This;
					protected:
						virtual void Dispose() override {
							if (This->poolTask != nullptr) {
								UnRegisterNtIoObjectInThreadPool(*This->poolTask);
								delete This->poolTask;
								This->poolTask = nullptr;
							}
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
							if (!IsValid)
								Throw(InvalidHandleUsage(ExceptionPosition));

							// verify pipe data transfer mode
							if (This->mode != PipeMode::In && This->mode != PipeMode::Both)
								Throw(InvalidPipeException(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								// Get buffer and its size
								void* buffer = (void*)(const TDataType::DataType*)data;
								SizeT size = data.Count * sizeof(typename TDataType::DataType);
								// create irp packet
								irp = new ReadIRP<TDataType>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeClient(*this)),
									onFinishReadAction, 
									MoveRValue(data));
								// create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									// notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									// Perfome os specific read operations
									This->InitAsyncRead(MoveRValue(irp), buffer, size, GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									// If read failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									// and rethrow exception as we don't how handle it
									ReThrow;
								}

								// return task object
								return task;
							}
							catch (...) {
								// destroy irp object as it allocated manualy from heap
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
							if (!IsValid)
								Throw(InvalidHandleUsage(ExceptionPosition));

							// verify pipe data transfer mode
							if (This->mode != PipeMode::Out && This->mode != PipeMode::Both)
								Throw(InvalidPipeException(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								// Get buffer and its size
								void* buffer = (void*)(const TDataType::DataType*)data;
								SizeT size = data.Count * sizeof(typename TDataType::DataType);
								// create irp packet
								irp = new WriteIRP<TDataType>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeClient(*this)),
									onFinishWriteAction, 
									MoveRValue(data));
								// create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									// notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									//Perfome os specific read operations
									This->InitAsyncWrite(MoveRValue(irp), buffer, size, GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									// If read failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									// and rethrow exception as we don't how handle it
									ReThrow;
								}

								// return task object
								return task;
							}
							catch (...) {
								// destroy irp object as it allocated manualy from heap
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
						virtual bool GetIsValid() const {
							return This->hPipe != nullptr;
						}
					public:
						/// <summary>
						/// Закритя з'єднання
						/// </summary>
						virtual void CloseConnection() { This->CloseConnection(); }

						/// <summary>
						/// Очікування закриття з'єднання
						/// </summary>
						virtual void WaitClosing() const { This->WaitClosing(); }

						/// <summary>
						/// Очікування закриття з'єднання з максимальним допустимими часом очікування 
						/// </summary>
						/// <param name="timeout">Максимальний допустимий час очікування</param>
						virtual void WaitClosing(Time::TimeSpan timeout) const { This->WaitClosing(timeout); }
					public:
						/// <summary>
						/// Очікує зчитування даних з другої сторони в синхроному режимі
						/// </summary>
						void Flush() {
							This->Flush();
						}
					public:
						/// <summary>
						/// Вертає ім'я каналу
						/// </summary>
						/// <returns>Рядок з іменем каналу</returns>
						const String& GetName() const { return This->pipeName; }

						/// <summary>
						/// Режим каналу
						/// </summary>
						/// <returns>Поточний режим каналу</returns>
						PipeMode GetSupportedMode() const { return This->mode; }

						/// <summary>
						/// Вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						/// <returns>Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації</returns>
						Boolean GetIsNoCache() const { return This->isNoCache; }
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						Boolean Property(GetIsValid) IsValid;

						/// <summary>
						/// Ім'я каналу
						/// </summary>
						String Property(GetName) Name;

						/// <summary>
						/// Режим роботи каналу
						/// </summary>
						PipeMode Property(GetSupportedMode) SupportedMode;

						/// <summary>
						/// Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						Boolean Property(GetIsNoCache) IsNoCache;
					public:
						/// <summary>
						/// Вертає посилання на задачу в пулі яка опрацьовує операції даного каналу
						/// </summary>
						/// <returns>Об'єкт операцій I/O в пулі потоків</returns>
						virtual const Seed::Core::Threading::UnifiedThreadPoolIoTask& GetIoPoolTask() const {
							return *This->poolTask;
						}
					public:
						template<typename UDataType>
						friend AsyncNamedPipeClient<UDataType> AsyncConnectPipe(
							const Threading::UnifiedThreadPool& ownerPool, 
							const String& name, 
							PipeMode mode, 
							Boolean isNoCache);
					public:
						RefCountClass(AsyncNamedPipeClient, This)
					};

					/// <summary>
					/// Екземпляр серверного каналу
					/// </summary>
					/// <typeparam name="TDataType">Тип блоку даних</typeparam>
					template<typename TDataType>
					class AsyncNamedPipeServerInstance final :
						public IAsyncNamedPipe<TDataType>,
						public IAsyncNamedPipeServerInstance,
						public IBaseConnectableAsyncIO<AsyncNamedPipeServerInstance<TDataType>> {
					private:
						Details::BaseAsyncNamedPipeServerInstance* This;
					protected:
						virtual void Dispose() override {
							if (This->poolTask != nullptr) {
								UnRegisterNtIoObjectInThreadPool(*This->poolTask);
								delete This->poolTask;
								This->poolTask = nullptr;
							}
							This->Dispose();
						}
					protected:
						virtual Handle GetHandle() {
							return This->hPipe;
						}
					protected:
						AsyncNamedPipeServerInstance(Handle hPipe, const String& name, PipeMode mode, Boolean isNoCache, const Threading::UnifiedThreadPool& threadPool)
							: This(new Details::BaseAsyncNamedPipeServerInstance(String(name), hPipe, mode, isNoCache))
						{
							// Register I/O object in thread pool
							This->poolTask = new Threading::UnifiedThreadPoolIoTask(RegisterNtIoObjectInThreadPool(threadPool, This->hPipe));
						}
					protected:
						void SetInstaceRef(Containers::ConcurencyList<SmartPtr<IAsyncNamedPipeServerInstance>>::Iterator serverRef) {
							This->serverRef = serverRef;
							// Must as serverRef container ref to this object in server pipe
							--This->RefCount;
						}
					protected:
						virtual IOTask AsyncConnect(SmartPtr<IFinishConnectCallback<AsyncNamedPipeServerInstance<TDataType>>> onFinishConnectAction) {
							if (!IsValid)
								Throw(InvalidHandleUsage(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								// create irp packet
								irp = new ConnectIRP<AsyncNamedPipeServerInstance>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeServerInstance(*this)),
									onFinishConnectAction,
									AsyncNamedPipeServerInstance(*this));
								// create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									// notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									// Perfome os specific connect operations
									This->InitAsyncConnect(MoveRValue(irp), GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									// If connect failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									// and rethrow exception as we don't how handle it
									ReThrow;
								}

								// return task object
								return task;
							}
							catch (...) {
								// destroy irp object as it allocated manualy from heap
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
							if (!IsValid)
								Throw(InvalidHandleUsage(ExceptionPosition));

							// verify pipe data transfer mode
							if (This->mode != PipeMode::In && This->mode != PipeMode::Both)
								Throw(InvalidPipeException(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								// Get buffer and its size
								void* buffer = (void*)(const TDataType::DataType*)data;
								SizeT size = data.Count * sizeof(typename TDataType::DataType);
								// create irp packet
								irp = new ReadIRP<TDataType>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeServerInstance(*this)),
									onFinishReadAction,
									MoveRValue(data));
								// create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									// notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									// Perfome os specific read operations
									This->InitAsyncRead(MoveRValue(irp), buffer, size, GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									// If read failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									// and rethrow exception as we don't how handle it
									ReThrow;
								}

								// return task object
								return task;
							}
							catch (...) {
								// destroy irp object as it allocated manualy from heap
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
							if (!IsValid)
								Throw(InvalidHandleUsage(ExceptionPosition));

							// verify pipe data transfer mode
							if (This->mode != PipeMode::Out && This->mode != PipeMode::Both)
								Throw(InvalidPipeException(ExceptionPosition));

							IRPBase* irp = nullptr;
							try {
								// Get buffer and its size
								void* buffer = (void*)(const TDataType::DataType*)data;
								SizeT size = data.Count * sizeof(typename TDataType::DataType);
								// create irp packet
								irp = new WriteIRP<TDataType>(
									SmartPtr<IAsyncIOBase>((IAsyncIOBase*&&)new AsyncNamedPipeServerInstance(*this)),
									onFinishWriteAction,
									MoveRValue(data));
								// create task object
								auto task = GetIOTaskFromIRPBase(irp);

								try {
									// notify about arrived io operation
									NotifyIoOperation(*This->poolTask);
									// Perfome os specific read operations
									This->InitAsyncWrite(MoveRValue(irp), buffer, size, GetReservedPlaceInIrp(irp));
								}
								catch (...) {
									// If read failed cancel notification
									CancelNotifyIoOperation(*This->poolTask);
									// and rethrow exception as we don't how handle it
									ReThrow;
								}

								// return task object
								return task;
							}
							catch (...) {
								// destroy irp object as it allocated manualy from heap
								if (irp != nullptr)
									delete irp;
								ReThrow;
							}
						}
					public:
						/// <summary>
						/// Очікує зчитування даних з другої сторони в синхроному режимі
						/// </summary>
						void Flush() {
							This->Flush();
						}
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						/// <returns>Логічне значення, true, якщо канал робочий</returns>
						virtual bool GetIsValid() const {
							return This->IsValid();
						}

						/// <summary>
						/// Вертає ім'я каналу
						/// </summary>
						/// <returns>Рядок з іменем каналу</returns>
						const String& GetName() const { return This->pipeName; }

						/// <summary>
						/// Режим каналу
						/// </summary>
						/// <returns>Поточний режим каналу</returns>
						PipeMode GetSupportedMode() const { return This->mode; }

						/// <summary>
						/// Вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						/// <returns>Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації</returns>
						Boolean GetIsNoCache() const { return This->isNoCache; }
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						Boolean Property(GetIsValid) IsValid;

						/// <summary>
						/// Ім'я каналу
						/// </summary>
						String Property(GetName) Name;

						/// <summary>
						/// Режим роботи каналу
						/// </summary>
						PipeMode Property(GetSupportedMode) SupportedMode;

						/// <summary>
						/// Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						Boolean Property(GetIsNoCache) IsNoCache;
					public:
						/// <summary>
						/// Закритя з'єднання
						/// </summary>
						virtual void CloseConnection() { This->CloseConnection(); }

						/// <summary>
						/// Очікування закриття з'єднання
						/// </summary>
						virtual void WaitClosing() const { This->WaitClosing(); }

						/// <summary>
						/// Очікування закриття з'єднання з максимальним допустимими часом очікування 
						/// </summary>
						/// <param name="timeout">Максимальний допустимий час очікування</param>
						virtual void WaitClosing(Time::TimeSpan timeout) const { This->WaitClosing(timeout); }
					public:
						/// <summary>
						/// Вертає посилання на задачу в пулі яка опрацьовує операції даного каналу
						/// </summary>
						/// <returns>Об'єкт операцій I/O в пулі потоків</returns>
						virtual const Seed::Core::Threading::UnifiedThreadPoolIoTask& GetIoPoolTask() const {
							return *This->poolTask;
						}
					public:
						RefCountClass(AsyncNamedPipeServerInstance, This)
					public:
						template<typename TBufferType>
						friend class AsyncNamedPipeServer;
					};

					/// <summary>
					/// Сервер іменованих каналів
					/// </summary>
					/// <typeparam name="TBufferType">Тип буфера даних</typeparam>
					template<typename TBufferType>
					class AsyncNamedPipeServer final : IDisposable {
					private:
						Details::BaseAsyncNamedPipeServer* This;
					protected:
						virtual void Dispose() override {
							This->Dispose();
						}
					protected:
						AsyncNamedPipeServer(String&& pipeName, PipeMode mode, Boolean isNoCache, UInt8 maxConnectionCount)
						: This(new Details::BaseAsyncNamedPipeServer(MoveRValue(pipeName), mode, isNoCache, maxConnectionCount)) {}
					public:
						/// <summary>
						/// Асинхроне очікування нового клієнта
						/// </summary>
						/// <param name="ownerPool">Пул який буде обробляти I/O запити</param>
						/// <param name="onFinishConnectAction">Дія яка буде викликана при приєднанні нового клієнта</param>
						/// <returns>Об'єкт асинхроної операції</returns>
						IOTask AsyncConnect(const Threading::UnifiedThreadPool& ownerPool, SmartPtr<IFinishConnectCallback<AsyncNamedPipeServerInstance<TBufferType>>> onFinishConnectAction) {
							//create pipe object
							auto pipe = AsyncNamedPipeServerInstance<TBufferType>(This->CreateServerPipeHandle(), This->pipeName, This->mode, This->isNoCache, ownerPool);
							//store it in list and save ref in object
							pipe.SetInstaceRef(This->instances.AddLast(new AsyncNamedPipeServerInstance(pipe)));
							//call pipe async connect
							return pipe.AsyncConnect(onFinishConnectAction);
						}

						/// <summary>
						/// Закриває всі активні з'єднання з клієнтами
						/// </summary>
						void CloseAllInstances() {
							This->CloseAllInstances();
						}

						/// <summary>
						/// Знищує канал разом з усіма активними з'єднаннями
						/// </summary>
						void DestroyPipe() {
							Dispose();
						}
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						/// <returns>Логічне значення, true, якщо канал робочий</returns>
						bool GetIsValid() const { return This->IsValid(); }

						/// <summary>
						/// Вертає кількість активних з'єднаннь
						/// </summary>
						/// <returns>Число яке вказує кількість активних з'єднаннь</returns>
						SizeT GetInstanceCount() const { return This == nullptr ? 0 : This->instances.Count; }

						/// <summary>
						/// Вертає ім'я каналу
						/// </summary>
						/// <returns>Рядок з іменем каналу</returns>
						const String& GetName() const { return This->pipeName; }

						/// <summary>
						/// Режим каналу
						/// </summary>
						/// <returns>Поточний режим каналу</returns>
						PipeMode GetSupportedMode() const { return This->mode; }

						/// <summary>
						/// Вказує чи даному екземпляру виконувати передачу даних без буферизації
						/// </summary>
						/// <returns>Логічне значення яке вказує чи даному екземпляру виконувати передачу даних без буферизації</returns>
						Boolean GetIsNoCache() const { return This->isNoCache; }
					public:
						/// <summary>
						/// Вказує чи канал робочий
						/// </summary>
						Boolean Property(GetIsValid) IsValid;

						/// <summary>
						/// Кількість активних з'єднань
						/// </summary>
						SizeT Property(GetInstanceCount) InstanceCount;

						/// <summary>
						/// Ім'я каналу
						/// </summary>
						String Property(GetName) Name;

						/// <summary>
						/// Режим роботи каналу
						/// </summary>
						PipeMode Property(GetSupportedMode) SupportedMode;

						/// <summary>
						/// Логічне значення яке вказує чи підтримує автоскидання буферів після кожної операції
						/// </summary>
						Boolean Property(GetIsNoCache) IsNoCache;
					public:
						RefCountClass(AsyncNamedPipeServer, This)
					public:
						template<typename UBufferType>
						friend AsyncNamedPipeServer<UBufferType> CreateAsyncPipe(
							const String& name, 
							UInt8 maxInstanceCount,
							PipeMode mode, 
							Boolean isNoCache);
					};

					/// <summary>
					/// Асинхроне з'єдання з іменованим каналом
					/// </summary>
					/// <typeparam name="TBufferType">Тип буфера даних</typeparam>
					/// <param name="ownerPool">Пул який буде опрацьовувати I/O операції</param>
					/// <param name="name">Ім'я каналу</param>
					/// <param name="mode">Режим передачі даних</param>
					/// <param name="isNoCache">Якщо true то використовується безбуферний режим роботи</param>
					/// <returns>Об'єкт іменованого каналу</returns>
					template<typename TBufferType = Containers::Blob>
					AsyncNamedPipeClient<TBufferType> AsyncConnectPipe(
						const Threading::UnifiedThreadPool& ownerPool, 
						const String& name, 
						PipeMode mode = PipeMode::Both, 
						Boolean isNoCache = true)
					{
						return AsyncNamedPipeClient<TBufferType>(String(name), mode, isNoCache, ownerPool);
					}

					/// <summary>
					/// Створення сервера іменованих каналів
					/// </summary>
					/// <typeparam name="TBufferType">Буфер даних</typeparam>
					/// <param name="name">Ім'я каналу</param>
					/// <param name="maxInstanceCount">Максимальна кількість клієнтів</param>
					/// <param name="mode">Режим передачі даних</param>
					/// <param name="isNoCache">Якщо true то використовується безбуферний режим роботи</param>
					/// <returns>Об'єкт сервера іменованих каналів</returns>
					template<typename TBufferType = Containers::Blob>
					AsyncNamedPipeServer<TBufferType> CreateAsyncPipe(
						const String& name, 
						UInt8 maxInstanceCount = Math::Max<UInt8>(),
						PipeMode mode = PipeMode::Both,
						Boolean isNoCache = true)
					{
						return AsyncNamedPipeServer<TBufferType>(String(name), mode, isNoCache, maxInstanceCount);
					}
				}
			}
		}
	}
}