#include "Seed.Core.IO.IPC.Pipes.NamedPipe.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma region Named pipe client

void Seed::Core::IO::IPC::Pipes::NamedPipeClient::Dispose()
{
	if (This->pipe != nullptr) {
		this->Close();
	}
}

void Seed::Core::IO::IPC::Pipes::NamedPipeClient::ReadRaw(Containers::Blob& data)
{
	if (this->This == nullptr || this->This->pipe == nullptr)
		Throw(NullPointException(ExceptionPosition));
	if (This->mode != PipeMode::In && This->mode != PipeMode::Both)
		Throw(InvalidPipeException(ExceptionPosition));

	NTSTATUS status = STATUS_SUCCESS;

	IO_STATUS_BLOCK IoStatusBlock = { 0 };

	This->inputLock.ExclusiveLock([&]() InlineLambda
	{
		status = NtReadFile(
			This->pipe,
			NULL,
			NULL,
			NULL,
			&IoStatusBlock,
			(void*)(const Byte*)data,
			ULONG(data.Count),
			0,
			NULL
		);
	});

	if (status == STATUS_PIPE_BROKEN) {
		this->Close();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::NamedPipeClient::WriteRawData(const Containers::Blob& data)
{
	if (this->This == nullptr || this->This->pipe == nullptr)
		Throw(NullPointException(ExceptionPosition));
	if (This->mode != PipeMode::Out && This->mode != PipeMode::Both)
		Throw(InvalidPipeException(ExceptionPosition));

	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	NTSTATUS status = STATUS_SUCCESS;

	This->ouputLock.ExclusiveLock([&]() InlineLambda
	{
		NtFlushBuffersFile(This->pipe, &IoStatusBlock);
		status = NtWriteFile( //-V107
			This->pipe,
			NULL,
			NULL,
			NULL,
			&IoStatusBlock,
			(void*)(const Byte*)data,
			ULONG(data.Count), 
			0,
			NULL
		);
	});

	if (status == STATUS_PIPE_BROKEN) {
		this->Close();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::NamedPipeClient::Close()
{
	if (this->This != nullptr)
		if (this->This->pipe != nullptr)
		{
			NtClose(This->pipe);
			this->This->pipe = nullptr;
		}
}

void Seed::Core::IO::IPC::Pipes::NamedPipeClient::WaitClosing() const
{
	if (this->This == nullptr || this->This->pipe == nullptr)
		Throw(NullPointException(ExceptionPosition));

	NtWaitForSingleObject(This->pipe, TRUE, NULL);
}

#pragma endregion


#pragma region Named pipe server

void Seed::Core::IO::IPC::Pipes::NamedPipeServer::Dispose()
{
	this->DestroyPipe();
}

Seed::Core::IO::IPC::Pipes::NamedPipeServerInstance Seed::Core::IO::IPC::Pipes::NamedPipeServer::Connect()
{
	if (This == nullptr || This->pipeHandle == nullptr)
		Throw(NullPointException(ExceptionPosition));

	IO_STATUS_BLOCK IoStatusBlock = { 0 };

	NTSTATUS Status = NtFsControlFile(This->pipeHandle,
		NULL,
		NULL,
		NULL,
		&IoStatusBlock,
		FSCTL_PIPE_LISTEN,
		NULL,
		NULL,
		NULL,
		NULL);

	NtFlushBuffersFile(This->pipeHandle, &IoStatusBlock);


	auto it =
		This->Instances.AddLast(
			NamedPipeServerInstance(
				new NamedPipeServerInstance::Data(
					This->pipeHandle,
					This->mode)
			)
		);

	it->This->ThisInstanceLink = it;
	return *it;
}

void Seed::Core::IO::IPC::Pipes::NamedPipeServer::CloseAllInstance()
{
	if (this->This != nullptr)
	{
		this->This->Instances
			.CopyTo<Containers::List>()
			.ForEach([&](NamedPipeServerInstance& item) InlineLambda{
				item.Close();
			})
			.RemoveAll();
	}
}

void Seed::Core::IO::IPC::Pipes::NamedPipeServer::DestroyPipe()
{
	if (this->This != nullptr)
	{
		this->CloseAllInstance();

		NtClose(this->This->pipeHandle);
		This->pipeHandle = nullptr;
	}
}

#pragma endregion

#pragma region NamedPipeServerInstance

void Seed::Core::IO::IPC::Pipes::NamedPipeServerInstance::Dispose()
{
}

void Seed::Core::IO::IPC::Pipes::NamedPipeServerInstance::ReadRaw(Containers::Blob& data)
{
	if (this->This == nullptr || this->This->pipe == nullptr)
		Throw(NullPointException(ExceptionPosition));
	if (This->mode != PipeMode::In && This->mode != PipeMode::Both)
		Throw(InvalidPipeException(ExceptionPosition));

	NTSTATUS status = STATUS_SUCCESS;

	IO_STATUS_BLOCK IoStatusBlock = { 0 };

	This->inputLock.ExclusiveLock([&]() InlineLambda
		{
			status = NtReadFile(
				This->pipe,
				NULL,
				NULL,
				NULL,
				&IoStatusBlock,
				(void*)(const Byte*)data,
				ULONG(data.Count),
				0,
				NULL
			);
		});

	if (status == STATUS_PIPE_BROKEN) {
		this->Close();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::NamedPipeServerInstance::WriteRawData(const Containers::Blob& data)
{
	if (this->This == nullptr || this->This->pipe == nullptr)
		Throw(NullPointException(ExceptionPosition));
	if (This->mode != PipeMode::Out && This->mode != PipeMode::Both)
		Throw(InvalidPipeException(ExceptionPosition));

	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	NTSTATUS status = STATUS_SUCCESS;

	This->ouputLock.ExclusiveLock([&]() InlineLambda
		{
			NtFlushBuffersFile(This->pipe, &IoStatusBlock);
			status = NtWriteFile( //-V107
				This->pipe,
				NULL,
				NULL,
				NULL,
				&IoStatusBlock,
				(void*)(const Byte*)data,
				ULONG(data.Count),
				0,
				NULL
			);
		});

	if (status == STATUS_PIPE_BROKEN) {
		this->Close();
		Throw(PipeBrokenException(ExceptionPosition));
	}
}

void Seed::Core::IO::IPC::Pipes::NamedPipeServerInstance::Close()
{
	if (this->This != nullptr)
		if (this->This->pipe != nullptr)
		{
			This->ThisInstanceLink.Remove(false);
			NtClose(This->pipe);
			this->This->pipe = nullptr;
		}
}

void Seed::Core::IO::IPC::Pipes::NamedPipeServerInstance::WaitClosing() const
{
	if (this->This == nullptr || this->This->pipe == nullptr)
		Throw(NullPointException(ExceptionPosition));

	NtWaitForSingleObject(This->pipe, TRUE, NULL);
}

#pragma endregion

using namespace Seed;
using namespace Seed::Core::IO::IPC::Pipes;

NamedPipeServer Seed::IO::IPC::Pipes::CreatePipe(
	const String& name, 
	UInt8 MaxInstanceCount, 
	PipeMode mode)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	UNICODE_STRING usPipeName = { 0 };
	LARGE_INTEGER WaitTime = { 0 };

	String realPipeName = String(L"\\??\\pipe\\") + name;

	RtlInitUnicodeStringEx(&usPipeName, (PWSTR)(ConstCharStr)realPipeName);

	InitializeObjectAttributes(&ObjectAttributes,
		&usPipeName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	WaitTime.QuadPart = -500000;

	HANDLE Pipe = NULL;

	ULONG PipeMode = 0, InBuffSize = 0, OutBuffSize = 0;
	switch (mode)
	{
	case PipeMode::In:
		PipeMode = GENERIC_READ;
		InBuffSize = 2048; OutBuffSize = 0;
		break;
	case PipeMode::Out:
		PipeMode = GENERIC_WRITE;
		InBuffSize = 0; OutBuffSize = 2048;
		break;
	case PipeMode::Both:
		PipeMode = GENERIC_READ | GENERIC_WRITE;
		InBuffSize = OutBuffSize = 2048;
		break;
	default:
		Throw(InvalidPipeException(ExceptionPosition));
	}

	NTSTATUS status = NtCreateNamedPipeFile(
		&Pipe,
		PipeMode | SYNCHRONIZE | PIPE_WAIT,
		&ObjectAttributes,
		&IoStatusBlock,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_ALERT,
		TRUE,
		TRUE,
		FALSE,
		MaxInstanceCount,
		InBuffSize,
		OutBuffSize,
		&WaitTime);

	if (!NT_SUCCESS(status))
		Throw(NullPointException(ExceptionPosition));

	return NamedPipeServer(new NamedPipeServer::Data(Pipe, mode, MaxInstanceCount, realPipeName));
}

Seed::Core::IO::IPC::Pipes::NamedPipeClient Seed::Core::IO::IPC::Pipes::ConnectPipe(const String& name, PipeMode mode)
{
	IO_STATUS_BLOCK IoStatusBlock = { 0 };
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	UNICODE_STRING usPipeName = { 0 };

	String realPipeName = L"\\??\\pipe\\";
	realPipeName += name;

	RtlInitUnicodeStringEx(&usPipeName, (PWSTR)(ConstCharStr)realPipeName);

	InitializeObjectAttributes(&ObjectAttributes,
		&usPipeName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);

	HANDLE pipe = NULL;

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
		&pipe,
		PipeMode | SYNCHRONIZE | PIPE_WAIT,
		&ObjectAttributes,
		&IoStatusBlock,
		NULL,
		NULL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_ALERT,
		NULL,
		0);

	if (!NT_SUCCESS(status))
		Throw(NullPointException(ExceptionPosition));

	realPipeName.LeftResize(realPipeName.Lenght - 9);
	return NamedPipeClient(new NamedPipeClient::Data(pipe, mode, MoveRValue(realPipeName)));
}