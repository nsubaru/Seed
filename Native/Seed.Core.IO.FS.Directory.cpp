#include "Seed.Core.IO.FS.Directory.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
// This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

void Seed::Core::IO::FS::Directory::Dispose()
{
    if (This->hDir != NULL)
    {
        NtClose(This->hDir);
        This->hDir = NULL;
    }
}

Seed::Core::IO::FS::Directory::Directory(String&& path, OpeningMode mode, OpenDisposition policy, bool ShareWithOs)
{
    String fullPath;

	if (!IsNtAdressFormat(path))
		fullPath = GetNtFileName(path);
	else
		fullPath = MoveRValue(path);

	UNICODE_STRING str;
	RtlInitUnicodeString(&str, const_cast<CharStr>(static_cast<ConstCharStr>(fullPath)));

	OBJECT_ATTRIBUTES attrb;
	InitializeObjectAttributes(&attrb, &str, NULL, NULL, NULL);
	ULONG FileMode = 0;
	ULONG Disposition = 0;
	ULONG ShareMode = 0;
	ULONG Attributes = FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_ALERT;

	if (mode & EOpeningMode::Read)
	{
		FileMode |= GENERIC_READ;
		ShareMode |= (ShareWithOs) ? (FILE_SHARE_WRITE | FILE_SHARE_READ) : NULL;
	}
	if (mode & EOpeningMode::Write)
	{
		FileMode |= GENERIC_WRITE;
		ShareMode |= (ShareWithOs) ? (FILE_SHARE_READ) : NULL;
	}
	if (mode & EOpeningMode::WriteThrought)
	{
		Attributes |= FILE_WRITE_THROUGH;
	}
	if (mode & EOpeningMode::Delete)
	{
		FileMode |= DELETE;
	}

	switch (policy)
	{
	case OpenDisposition::Superseed:
		Disposition = FILE_SUPERSEDE;
		break;
	case OpenDisposition::Create:
		Disposition = FILE_CREATE;
		break;
	case OpenDisposition::Open:
		Disposition = FILE_OPEN;
		break;
	case OpenDisposition::OpenIf:
		Disposition = FILE_OPEN_IF;
		break;
	case OpenDisposition::Overwrite:
		Disposition = FILE_OVERWRITE;
		break;
	case OpenDisposition::OverwriteIf:
		Disposition = FILE_OVERWRITE_IF;
		break;
	default:
		break;
	}

	HANDLE hFile = nullptr;
	IO_STATUS_BLOCK ioblk = { 0 };
	NTSTATUS status = NtCreateFile(
		&hFile,
		SYNCHRONIZE | FileMode,
		&attrb,
		&ioblk,
		NULL,
		FILE_ATTRIBUTE_DIRECTORY,
		ShareMode,
		Disposition,
		Attributes,
		NULL,
		NULL);

	if (!NT_SUCCESS(status)) {
		switch (status)
		{
		case STATUS_OBJECT_NAME_NOT_FOUND:
		case STATUS_NO_SUCH_FILE:
			Throw(FileNotExistException(SourcePosition, fullPath));
		case STATUS_OBJECT_NAME_COLLISION:
			Throw(FileExistException(SourcePosition, fullPath));
		case STATUS_SHARING_VIOLATION:
			Throw(FileBusyException(SourcePosition, fullPath));
		case STATUS_FILE_IS_A_DIRECTORY:
			Throw(DirectoryIsNotFileException(SourcePosition, fullPath));
		case STATUS_FILE_CORRUPT_ERROR:
			Throw(FileIsCoruptedException(SourcePosition, fullPath));
		case STATUS_OBJECT_PATH_NOT_FOUND:
			Throw(PathNotFoundException(SourcePosition, fullPath));
		default:
			Throw(GenericFileException(SourcePosition, fullPath, L"Generic exception"));
		}
	}

	This = new Data(hFile, MoveRValue(fullPath), mode, policy, ShareWithOs);
}

#pragma warning(pop)