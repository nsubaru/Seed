#include "Seed.Core.IO.IPC.Pipes.AsyncNamedPipe.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma region Named pipe client

Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeClient::BaseAsyncNamedPipeClient(String&& pipeName, PipeMode mode, Boolean isNoCache)
	: pipeName(MoveRValue(pipeName)), mode(mode), isNoCache(isNoCache), hPipe(nullptr), poolTask(nullptr), RefCount(1)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	UNICODE_STRING usPipeName = { 0 };

	String realPipeName = L"\\??\\pipe\\";
	realPipeName += this->pipeName;

	RtlInitUnicodeStringEx(&usPipeName, (PWSTR)(ConstCharStr)realPipeName);

	InitializeObjectAttributes(&ObjectAttributes,
		&usPipeName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	ULONG PipeMode = 0;
	switch (mode)
	{
	case PipeMode::In:
		PipeMode = GENERIC_READ;
		break;
	case PipeMode::Out:
		PipeMode = GENERIC_WRITE;
		break;
	case PipeMode::Both:
		PipeMode = GENERIC_READ | GENERIC_WRITE;
		break;
	default:
		Throw(InvalidPipeException(ExceptionPosition));
	}

	NTSTATUS status = NtCreateFile(
		&this->hPipe,
		PipeMode | SYNCHRONIZE | PIPE_WAIT,
		&ObjectAttributes,
		&IoStatusBlock,
		NULL,
		NULL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		(isNoCache ? FILE_WRITE_THROUGH : NULL),
		NULL,
		0);

	if (!NT_SUCCESS(status))
		Throw(NullPointException(ExceptionPosition));

	realPipeName.LeftResize(realPipeName.Lenght - 9);
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeClient::InitAsyncRead(IRPBase*&& irp, void* buffer, SizeT size, void* ioStatusBlk)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = NtReadFile(
		hPipe,
		NULL,
		NULL,
		irp,
		(IO_STATUS_BLOCK*)ioStatusBlk,
		buffer,
		size,
		0,
		NULL
	);

	if (status == STATUS_PIPE_BROKEN) {
		this->Dispose();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeClient::InitAsyncWrite(IRPBase*&& irp, void* buffer, SizeT size, void* ioStatusBlk)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	NTSTATUS status = STATUS_SUCCESS;

	NtFlushBuffersFile(hPipe, &IoStatusBlock);
	status = NtWriteFile(
		hPipe,
		NULL,
		NULL,
		NULL,
		(IO_STATUS_BLOCK*)ioStatusBlk,
		buffer,
		size,
		0,
		NULL
	);

	if (status == STATUS_PIPE_BROKEN) {
		this->Dispose();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeClient::CloseConnection()
{
	if (this->hPipe != nullptr) {
		NtClose(this->hPipe);
		this->hPipe = nullptr;
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeClient::Flush()
{
	if (this->hPipe == nullptr)
		Throw(NullPointException(ExceptionPosition));

	IO_STATUS_BLOCK ioStatusBlock = { 0 };
	NTSTATUS status = NtFlushBuffersFile(this->hPipe, &ioStatusBlock);

	if (status == STATUS_PIPE_BROKEN) {
		this->CloseConnection();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeClient::WaitClosing() const
{
	if (this->hPipe == nullptr)
		Throw(NullPointException(ExceptionPosition));

	NtWaitForSingleObject(hPipe, TRUE, NULL);
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeClient::WaitClosing(Time::TimeSpan timeout) const
{
	if (this->hPipe == nullptr)
		Throw(NullPointException(ExceptionPosition));

	LARGE_INTEGER ntTimeout = { .QuadPart = -(timeout.Ticks) };

	NtWaitForSingleObject(hPipe, TRUE, &ntTimeout);
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeClient::Dispose()
{
	CloseConnection();
}

#pragma endregion

#pragma region Named pipe server 

Seed::Core::Handle Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServer::CreateServerPipeHandle()
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	UNICODE_STRING usPipeName = { 0 };
	LARGE_INTEGER WaitTime = { 0 };

	String realPipeName = String(L"\\??\\pipe\\") + pipeName;

	RtlInitUnicodeStringEx(&usPipeName, (PWSTR)(ConstCharStr)realPipeName);

	InitializeObjectAttributes(&ObjectAttributes,
		&usPipeName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	WaitTime.QuadPart = -500000;

	HANDLE hPipe = NULL;

	ULONG pipeMode = 0, InBuffSize = 0, OutBuffSize = 0;
	switch (mode)
	{
	case PipeMode::In:
		pipeMode = GENERIC_READ;
		InBuffSize = 2048; OutBuffSize = 0;
		break;
	case PipeMode::Out:
		pipeMode = GENERIC_WRITE;
		InBuffSize = 0; OutBuffSize = 2048;
		break;
	case PipeMode::Both:
		pipeMode = GENERIC_READ | GENERIC_WRITE;
		InBuffSize = OutBuffSize = 2048;
		break;
	default:
		Throw(InvalidPipeException(ExceptionPosition));
	}

	NTSTATUS status = NtCreateNamedPipeFile(
		&hPipe,
		pipeMode | SYNCHRONIZE | PIPE_WAIT | ((instances.Count > 0) ? FILE_CREATE_PIPE_INSTANCE : NULL),
		&ObjectAttributes,
		&IoStatusBlock,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		(isNoCache ? FILE_WRITE_THROUGH : NULL),
		TRUE,
		TRUE,
		TRUE,
		maxConnectionCount,
		InBuffSize,
		OutBuffSize,
		&WaitTime);

	if (!NT_SUCCESS(status))
		Throw(NullPointException(ExceptionPosition));

	return hPipe;
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServer::CloseAllInstances()
{
	instances.CopyTo<Containers::List>().RemoveAll([](auto&& instance) { instance->CloseConnection(); });
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServer::Dispose()
{
	CloseAllInstances();
}

#pragma endregion

#pragma region Named pipe server instance

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServerInstance::InitAsyncConnect(IRPBase*&& irp, void* ioStatusBlk)
{
	NTSTATUS status = NtFsControlFile(hPipe,
		NULL,
		NULL,
		irp,
		(IO_STATUS_BLOCK*)ioStatusBlk,
		FSCTL_PIPE_LISTEN,
		NULL,
		NULL,
		NULL,
		NULL);

	if (!NT_SUCCESS(status)) {
		Dispose();
		Throw(NullPointException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServerInstance::InitAsyncRead(IRPBase*&& irp, void* buffer, SizeT size, void* ioStatusBlk)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = NtReadFile(
		hPipe,
		NULL,
		NULL,
		irp,
		(IO_STATUS_BLOCK*)ioStatusBlk,
		buffer,
		size,
		0,
		NULL
	);

	if (status == STATUS_PIPE_BROKEN) {
		this->Dispose();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServerInstance::InitAsyncWrite(IRPBase*&& irp, void* buffer, SizeT size, void* ioStatusBlk)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	NTSTATUS status = STATUS_SUCCESS;

	NtFlushBuffersFile(hPipe, &IoStatusBlock);
	status = NtWriteFile(
		hPipe,
		NULL,
		NULL,
		NULL,
		(IO_STATUS_BLOCK*)ioStatusBlk,
		buffer,
		size,
		0,
		NULL
	);

	if (status == STATUS_PIPE_BROKEN) {
		this->Dispose();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServerInstance::CloseConnection()
{
	if (this->hPipe != nullptr) {
		NtClose(this->hPipe);
		this->serverRef.Remove(true);
		this->hPipe = nullptr;
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServerInstance::Flush()
{
	if (this->hPipe == nullptr)
		Throw(NullPointException(ExceptionPosition));

	IO_STATUS_BLOCK ioStatusBlock = { 0 };
	NTSTATUS status = NtFlushBuffersFile(this->hPipe, &ioStatusBlock);

	if (status == STATUS_PIPE_BROKEN) {
		this->CloseConnection();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServerInstance::WaitClosing() const
{
	if (this->hPipe == nullptr)
		Throw(NullPointException(ExceptionPosition));

	NtWaitForSingleObject(hPipe, TRUE, NULL);
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServerInstance::WaitClosing(Time::TimeSpan timeout) const
{
	if (this->hPipe == nullptr)
		Throw(NullPointException(ExceptionPosition));

	LARGE_INTEGER ntTimeout = { .QuadPart = -(timeout.Ticks) };

	NtWaitForSingleObject(hPipe, TRUE, &ntTimeout);
}

void Seed::Core::IO::IPC::Pipes::Details::BaseAsyncNamedPipeServerInstance::Dispose()
{
	if (this->hPipe != nullptr) {
		//set fake ref count value as pipe dtor can call in close connection due removing server ref
		this->RefCount = 2;
		CloseConnection();
		//restore real ref count value
		this->RefCount = 0;
	}
}

#pragma endregion