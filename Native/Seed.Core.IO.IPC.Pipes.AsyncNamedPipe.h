//#pragma once
//#include "Seed.Core.IO.IPC.Pipes.Base.h"
//#include "Seed.Core.IO.IPC.Pipes.NamedPipe.h"
//
//#include "Seed.Core.Threading.h"
//
//namespace Seed {
//	inline namespace Core {
//		namespace IO {
//			namespace IPC {
//				namespace Pipes {
//					namespace Details {
//						class BaseAsyncNamedPipeClient final : protected IDisposable {
//						protected:
//							Handle hPipe;
//
//						protected:
//							virtual void Dispose();
//						};
//					}
//					template<typename TBufferType>
//					class IAsyncNamedPipe : public IO::IBaseAsyncIO<TBufferType> {
//					public:
//						virtual void Close() = 0;
//
//						virtual void WaitClosing() const = 0;
//
//						virtual ~IAsyncNamedPipe() = 0 {};
//					};
//
//					template<typename TBufferType>
//					class AsyncNamedPipeClient final : public IAsyncNamedPipe<TBufferType> {
//					protected:
//						virtual void Dispose() override;
//					public:
//						/// <summary>
//						/// Ініціалізація асинхроної операції читання
//						/// </summary>
//						/// <param name="data">Буфер даних</param>
//						/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
//						/// <returns>Об'єкт задачі I/O</returns>
//						virtual IOTask AsyncRead(TDataType&& data, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) {
//
//						}
//
//						/// <summary>
//						/// Ініціалізація асинхроної операції запису
//						/// </summary>
//						/// <param name="data">Буфер даних</param>
//						/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
//						/// <returns>Об'єкт задачі I/O</returns>
//						virtual IOTask AsyncWrite(const TDataType&& data, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) {
//
//						}
//					public:
//						/// <summary>
//						/// Перевірка чи об'єкт коректний
//						/// </summary>
//						/// <returns>true якщо об'єкт допускає I/O операції</returns>
//						virtual bool IsValid() {
//
//						}
//					public:
//						friend AsyncNamedPipeClient AsyncConnectPipe(const String& name, PipeMode mode);
//					public:
//						RefCountClass(AsyncNamedPipeClient, This)
//					};
//
//					class IAsyncNamedPipeServerInstance {
//					public:
//						virtual ~IAsyncNamedPipeServerInstance() = 0 {}
//					};
//
//					template<typename TBufferType>
//					class AsyncNamedPipeServerInstance;
//
//					class AsyncNamedPipeServer final : protected IDisposable {
//					private:
//						struct Data {
//							PipeMode mode;
//							Handle pipeHandle;
//							UInt32 MaxInstance;
//							Containers::ConcurencyList<SmartPtr<IAsyncNamedPipeServerInstance>> Instances;
//							String name;
//
//							RefCountT RefCount;
//
//							Data(
//								Handle pipeHandle,
//								PipeMode mode,
//								UInt32 MaxInstance,
//								String name
//							) :
//								pipeHandle(pipeHandle),
//								mode(mode),
//								MaxInstance(MaxInstance),
//								name(name),
//								Instances(),
//								RefCount(1)
//							{};
//						};
//
//						Data* This;
//					private:
//						AsyncNamedPipeServer(Data*&& This) :This(This) {};
//					protected:
//						virtual void Dispose() override;
//					public:
//						AsyncNamedPipeServer() :This(nullptr) {};
//
//						template<typename TBufferType>
//						NamedPipeServerInstance AsyncConnect() {
//
//						}
//
//						void CloseAllInstance();
//
//						void DestroyPipe();
//
//						SizeT InstanceCount() { return This == nullptr ? 0 : This->Instances.Count; }
//
//						bool IsValid() const { return This != nullptr && This->pipeHandle != nullptr; }
//					public:
//						RefCountClass(AsyncNamedPipeServer, This)
//					public:
//						friend AsyncNamedPipeServer CreateAsyncPipe(const String& name, UInt32 MaxInstanceCount, PipeMode mode);
//					};
//
//					template<typename TBufferType>
//					class AsyncNamedPipeServerInstance : public IAsyncNamedPipe<TBufferType>, public IAsyncNamedPipeServerInstance {
//					protected:
//						virtual void Dispose() override;
//					public:
//						/// <summary>
//						/// Ініціалізація асинхроної операції читання
//						/// </summary>
//						/// <param name="data">Буфер даних</param>
//						/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
//						/// <returns>Об'єкт задачі I/O</returns>
//						virtual IOTask AsyncRead(TDataType&& data, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) {
//
//						}
//
//						/// <summary>
//						/// Ініціалізація асинхроної операції запису
//						/// </summary>
//						/// <param name="data">Буфер даних</param>
//						/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
//						/// <returns>Об'єкт задачі I/O</returns>
//						virtual IOTask AsyncWrite(const TDataType&& data, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) {
//
//						}
//					public:
//						/// <summary>
//						/// Перевірка чи об'єкт коректний
//						/// </summary>
//						/// <returns>true якщо об'єкт допускає I/O операції</returns>
//						virtual bool IsValid() {
//
//						}
//					public:
//						RefCountClass(AsyncNamedPipeServerInstance, This)
//					public:
//						friend class AsyncNamedPipeServer;
//					};
//
//
//					template<typename TBufferType>
//					AsyncNamedPipeClient<TBufferType> AsyncConnectPipe(const String& name, PipeMode mode)
//					{
//						return AsyncNamedPipeClient();
//					}
//
//					AsyncNamedPipeServer Seed::Core::IO::IPC::Pipes::CreateAsyncPipe(const String& name, UInt32 MaxInstanceCount, PipeMode mode)
//					{
//						return AsyncNamedPipeServer();
//					}
//				}
//			}
//		}
//	}
//}