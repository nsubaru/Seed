#include "Seed.Core.Testing.Tests.IO.FS.NamedPipe.Async.h"

#include "Seed.Core.IO.h"

bool Seed::Core::Testing::Test::IO::NamedPipe::Async::AsyncNamedPipeServerCreation::Excecute()
{
    auto pool = Core::Threading::UnifiedThreadPoolBuilder()
        .SetName(L"Test thread pool")
        .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetMinThreadCount(1)
        .SetMaxThreadCount(8)
        .CreateThreadPool();

    auto pipeName = L"test__seed__pipe"S;

    auto pipe = Seed::Core::IO::IPC::Pipes::CreateAsyncPipe(pipeName);

    Asserts::AssertEqual(pipe.Name, pipeName, ExceptionPosition);
    Asserts::AssertEqual(pipe.IsNoCache, true, ExceptionPosition);
    Asserts::AssertEqual(pipe.SupportedMode, Core::IO::IPC::Pipes::PipeMode::Both, ExceptionPosition);
    Asserts::AssertEqual(pipe.InstanceCount, 0, ExceptionPosition);

    return true;
}

bool Seed::Core::Testing::Test::IO::NamedPipe::Async::AsyncNamedPipeComunication::Excecute()
{
    class ServerConnectCallback final : public Core::IO::IFinishConnectCallback<Core::IO::IPC::Pipes::AsyncNamedPipeServerInstance<Core::Containers::Blob>> {
    protected:
        virtual void Dispose() {}
    public:
        using PipeInstance = Core::SmartPtr<Core::IO::IPC::Pipes::AsyncNamedPipeServerInstance<Core::Containers::Blob>>;
    public:
        PipeInstance ConnectedPipe = nullptr;
        UInt32 ErrorCode = 0;
    public:
        virtual void OnConnectFinish(Core::IO::IPC::Pipes::AsyncNamedPipeServerInstance<Core::Containers::Blob>&& connectionObject) {
            ConnectedPipe = PipeInstance
                (new Core::IO::IPC::Pipes::AsyncNamedPipeServerInstance<Core::Containers::Blob>(MoveRValue(connectionObject)));
        }

        virtual void OnError(UInt32 errorCode) {
            ErrorCode = errorCode;
        }
    public:
        virtual ~ServerConnectCallback() {}
    };

    class ReadCallback final : public Core::IO::IFinishReadCallback<Core::Containers::Blob> {
    protected:
        virtual void Dispose() {}
    public:
        UInt32 ErrorCode = 0;
        Core::Containers::Blob Data = {};
    public:
        virtual void OnFinishRead(SizeT byteTransfered, Core::Containers::Blob&& data) {
            Data = MoveRValue(data);
        }

        virtual void OnError(UInt32 errorCode) {
            ErrorCode = errorCode;
        }
    public:
        virtual ~ReadCallback() {}
    };

    class WriteCallback final : public Core::IO::IFinishWriteCallback<Core::Containers::Blob> {
    protected:
        virtual void Dispose() {}
    public:
        UInt32 ErrorCode = 0;
        Core::Containers::Blob Data = {};
    public:
        virtual void OnWriteFinish(SizeT byteTransfered, const Core::Containers::Blob&& data) {
            Data = MoveRValue(const_cast<Core::Containers::Blob&&>(data));
        }

        virtual void OnError(UInt32 errorCode) {
            ErrorCode = errorCode;
        }
    public:
        virtual ~WriteCallback() {}
    };

    Core::Serilization::TypesRegistry registry;

    auto pool = Core::Threading::UnifiedThreadPoolBuilder()
        .SetName(L"Test thread pool")
        .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetMinThreadCount(1)
        .SetMaxThreadCount(8)
        .CreateThreadPool();

    auto pipeName = L"test__seed__pipe"S;

    auto serverPipe = Seed::Core::IO::IPC::Pipes::CreateAsyncPipe(pipeName);

    auto serverConnectCallback = SmartPtr<Core::IO::IFinishConnectCallback<Core::IO::IPC::Pipes::AsyncNamedPipeServerInstance<Core::Containers::Blob>>>
        (new ServerConnectCallback());

    auto serverConnectionPipeIOTask = serverPipe.AsyncConnect(pool, serverConnectCallback);

    auto clientPipe = Seed::Core::IO::IPC::Pipes::AsyncConnectPipe(pool, pipeName);

    serverConnectionPipeIOTask.WaitComplete(10s);
    Asserts::AssertTrue(serverConnectionPipeIOTask.IsCompleted, ExceptionPosition);
    Asserts::AssertEqual(serverConnectCallback.As<ServerConnectCallback>().ErrorCode, 0, ExceptionPosition);
    Asserts::AssertNotEqual(serverConnectCallback.As<ServerConnectCallback>().ConnectedPipe, nullptr, ExceptionPosition);

    auto fileList = Core::IO::FS::QueryFilesInDirectory(L".\\");

    auto serverWriteCallback = SmartPtr<Core::IO::IFinishWriteCallback<Core::Containers::Blob>>(new WriteCallback());

    auto serverWriteIOTask = serverConnectCallback.
        As<ServerConnectCallback>().
        ConnectedPipe->
        AsyncWrite(
            Core::Serilization::Serilize(fileList, registry), 
            serverWriteCallback);

    auto clientReadCallback = SmartPtr<Core::IO::IFinishReadCallback<Core::Containers::Blob>>(new ReadCallback());
    auto clientReadTaskIO = clientPipe.AsyncRead(Core::Containers::Blob(Core::Serilization::Serilize(fileList, registry).Count), clientReadCallback);

    serverWriteIOTask.WaitComplete(10s);
    clientReadTaskIO.WaitComplete(10s);

    Asserts::AssertTrue(serverWriteIOTask.IsCompleted, ExceptionPosition);
    Asserts::AssertTrue(clientReadTaskIO.IsCompleted, ExceptionPosition);

    Asserts::AssertEqual(serverWriteCallback.As<WriteCallback>().ErrorCode, 0, ExceptionPosition);
    Asserts::AssertEqual(clientReadCallback.As<ReadCallback>().ErrorCode, 0, ExceptionPosition);

    Asserts::AssertEqual(serverWriteCallback.As<WriteCallback>().Data.Count, clientReadCallback.As<ReadCallback>().Data.Count, ExceptionPosition);

    for (SizeT i = 0; i < serverWriteCallback.As<WriteCallback>().Data.Count; i++)
    {
        Asserts::AssertEqual(serverWriteCallback.As<WriteCallback>().Data[i], clientReadCallback.As<ReadCallback>().Data[i], ExceptionPosition);
    }

    clientPipe.CloseConnection();

    serverConnectCallback.
        As<ServerConnectCallback>().ConnectedPipe->WaitClosing(10s);

    serverPipe.CloseAllInstances();

    return true;
}
