#include "Seed.Core.Testing.Tests.IO.FS.NamedPipe.Sync.h"

#include "Seed.Core.IO.h"

bool Seed::Core::Testing::Test::IO::NamedPipe::Sync::SyncNamedPipeServerCreation::Excecute()
{
    auto pipeName = L"test__seed__pipe"S;

    auto pipe = Seed::Core::IO::IPC::Pipes::CreatePipe(pipeName);

    Asserts::AssertEqual(pipe.Name, pipeName, ExceptionPosition);
    Asserts::AssertEqual(pipe.IsNoCache, true, ExceptionPosition);
    Asserts::AssertEqual(pipe.SupportedMode, Core::IO::IPC::Pipes::PipeMode::Both, ExceptionPosition);
    Asserts::AssertEqual(pipe.InstanceCount, 0, ExceptionPosition);

    return true;
}

bool Seed::Core::Testing::Test::IO::NamedPipe::Sync::SyncNamedPipeComunication::Excecute()
{
    Core::Threading::SynchronizeEvent event = Core::Threading::SynchronizeEvent(true);
    event.Reset();

    auto serverThread = Core::Threading::Thread(L"Pipe test server thread", Core::Delegate([](Core::Threading::SynchronizeEvent event) {
        auto pipeName = L"test__seed__pipe"S;

        auto pipeServer = Seed::Core::IO::IPC::Pipes::CreatePipe(pipeName);
        event.Set();
        auto pipeInstance = pipeServer.Connect();

        Core::Containers::Blob data = pipeName;
        Core::Containers::Blob buffer = pipeName;

        pipeInstance.ReadRaw(buffer);

        Asserts::AssertEqual(data.Count, buffer.Count, ExceptionPosition);
        for (SizeT i = 0; i < data.Count; i++)
        {
            Asserts::AssertEqual(data[i], buffer[i], ExceptionPosition);
        }

        pipeInstance.WriteRawData(buffer);

        pipeInstance.WaitClosing();
        pipeInstance.Close();

        Asserts::AssertFalse(pipeInstance.IsValid, ExceptionPosition);
        Asserts::AssertEqual(0, pipeServer.InstanceCount, ExceptionPosition);

        return true;
        }, event));

    auto clientThread = Core::Threading::Thread(L"Pipe test client thread", Core::Delegate([](Core::Threading::SynchronizeEvent event) {
        event.Wait();
        Core::Threading::Sleep(100ms);
        auto pipeName = L"test__seed__pipe"S;
        
        auto pipe = Seed::Core::IO::IPC::Pipes::ConnectPipe(pipeName);

        Core::Containers::Blob data = pipeName;
        Core::Containers::Blob buffer = pipeName;

        pipe.WriteRawData(buffer);
        pipe.ReadRaw(buffer);

        Asserts::AssertEqual(data.Count, buffer.Count, ExceptionPosition);
        for (SizeT i = 0; i < data.Count; i++)
        {
            Asserts::AssertEqual(data[i], buffer[i], ExceptionPosition);
        }

        pipe.Close();

        Asserts::AssertFalse(pipe.IsValid, ExceptionPosition);

        return true;
        }, event));

    serverThread.Start();
    clientThread.Start();

    serverThread.Join(10s);
    clientThread.Join(10s);

    serverThread.Interupt();
    clientThread.Interupt();

    Asserts::AssertNoThrowException<InteruptException>([&]() { Asserts::AssertTrue(serverThread.GetThreadResult<bool>(), ExceptionPosition); }, ExceptionPosition);
    Asserts::AssertNoThrowException<InteruptException>([&]() { Asserts::AssertTrue(clientThread.GetThreadResult<bool>(), ExceptionPosition); }, ExceptionPosition);

    return true;
}
