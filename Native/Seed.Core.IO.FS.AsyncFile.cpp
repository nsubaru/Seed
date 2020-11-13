#include "Seed.Core.IO.FS.AsyncFile.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

void Seed::Core::IO::FS::Details::BaseAsyncFile::Dispose()
{	
	if (hFile != nullptr) {
		NtClose(hFile);
		hFile = nullptr;
	}
}

Seed::Core::IO::FS::Details::BaseAsyncFile::BaseAsyncFile(String&& path, File::OpeningMode mode, File::OpenDisposition policy, bool ShareWithOs)
	: policy(policy), mode(mode), ShareWithOs(ShareWithOs), hFile(nullptr), ntFileName(), RefCount(1)
{
	// check is NT file path format, if not than try convert to NT path format
	if (!IsNtAdressFormat(path))
		ntFileName = GetNtFileName(path);
	else
		ntFileName = MoveRValue(path);

	UNICODE_STRING str = { 0 };
	RtlInitUnicodeString(&str, (CharStr)(ConstCharStr)ntFileName);

	OBJECT_ATTRIBUTES attrb = { 0 };
	InitializeObjectAttributes(&attrb, &str, NULL, NULL, NULL);
	ULONG FileMode = 0;
	ULONG Disposition = 0;
	ULONG ShareMode = 0;
	ULONG Attributes = FILE_NON_DIRECTORY_FILE;

	// configure file opening and share mode
	if (mode.Read)
	{
		FileMode |= GENERIC_READ;
		ShareMode |= (ShareWithOs) ? (FILE_SHARE_WRITE | FILE_SHARE_READ) : NULL;
	}
	if (mode.Write)
	{
		FileMode |= GENERIC_WRITE;
		ShareMode |= (ShareWithOs) ? (FILE_SHARE_READ) : NULL;
	}
	if (mode.WriteThrought)
	{
		Attributes |= FILE_WRITE_THROUGH;
	}
	if (mode.Delete)
	{
		FileMode |= DELETE;
	}

	// configure file opening policy
	switch (policy)
	{
	case Seed::Core::IO::FS::File::OpenDisposition::Superseed:
		Disposition = FILE_SUPERSEDE;
		break;
	case Seed::Core::IO::FS::File::OpenDisposition::Create:
		Disposition = FILE_CREATE;
		break;
	case Seed::Core::IO::FS::File::OpenDisposition::Open:
		Disposition = FILE_OPEN;
		break;
	case Seed::Core::IO::FS::File::OpenDisposition::OpenIf:
		Disposition = FILE_OPEN_IF;
		break;
	case Seed::Core::IO::FS::File::OpenDisposition::Overwrite:
		Disposition = FILE_OVERWRITE;
		break;
	case Seed::Core::IO::FS::File::OpenDisposition::OverwriteIf:
		Disposition = FILE_OVERWRITE_IF;
		break;
	default:
		break;
	}

	IO_STATUS_BLOCK ioblk = { 0 };

	//call NT service function
	NTSTATUS status = NtCreateFile(
		&hFile,
		FileMode,
		&attrb,
		&ioblk,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		ShareMode,
		Disposition,
		Attributes,
		NULL,
		NULL);

	// process nt service return status code
	if (!NT_SUCCESS(status)) {
		switch (status)
		{
		case STATUS_OBJECT_NAME_NOT_FOUND:
		case STATUS_NO_SUCH_FILE:
			Throw(FileNotExistException(ExceptionPosition, ntFileName));
		case STATUS_OBJECT_NAME_COLLISION:
			Throw(FileExistException(ExceptionPosition, ntFileName));
		case STATUS_SHARING_VIOLATION:
			Throw(FileBusyException(ExceptionPosition, ntFileName));
		case STATUS_FILE_IS_A_DIRECTORY:
			Throw(DirectoryIsNotFileException(ExceptionPosition, ntFileName));
		case STATUS_FILE_CORRUPT_ERROR:
			Throw(FileIsCoruptedException(ExceptionPosition, ntFileName));
		default:
			Throw(GenericFileException(ExceptionPosition, ntFileName, L"Generic exception"));
		}
	}
}

void Seed::Core::IO::FS::Details::BaseAsyncFile::InitAsyncRead(IRPBase*&& irp, void* buffer, SizeT position, SizeT size, void* ioStatusBlk)
{
	LARGE_INTEGER offset = { 0 };
	offset.QuadPart = position;

	NTSTATUS status = NtReadFile(
		this->hFile, 
		NULL,
		NULL,
		irp, 
		(PIO_STATUS_BLOCK)ioStatusBlk, 
		buffer, 
		(ULONG)size, 
		&offset, 
		NULL);

	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
}

void Seed::Core::IO::FS::Details::BaseAsyncFile::InitAsyncWrite(IRPBase*&& irp, void* buffer, SizeT position, SizeT size, void* ioStatusBlk)
{
	LARGE_INTEGER offset = { 0 };
	offset.QuadPart = position;

	NTSTATUS status = NtWriteFile(
		this->hFile, 
		NULL,
		NULL,
		irp, 
		(PIO_STATUS_BLOCK)ioStatusBlk, 
		buffer, 
		(ULONG)size, 
		&offset,
		NULL);

	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
}

void Seed::Core::IO::FS::Details::BaseAsyncFile::DeleteFile()
{
	if (!(mode.Delete))
		Throw(FileModeNoSupportedByOperationException(ExceptionPosition, ntFileName));

	FILE_COMPLETION_INFORMATION info = { 0 };
	IO_STATUS_BLOCK ioblk = { 0 };
	NTSTATUS status = 0;

	FILE_DISPOSITION_INFORMATION dispositionInformation{ TRUE };
	
	// set deleting on file handl closing
	status = NtSetInformationFile(
		hFile,
		&ioblk,
		&dispositionInformation,
		sizeof(FILE_DISPOSITION_INFORMATION),
		FileDispositionInformation
	);

	if (!NT_SUCCESS(status))
		Throw(GenericFileException(ExceptionPosition, ntFileName, L"Can't close file"));
}
