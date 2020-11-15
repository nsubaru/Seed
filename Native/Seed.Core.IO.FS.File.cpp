// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.IO.FS.File.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

using namespace Seed;

Containers::List<IO::FS::FSObjectInformation> IO::FS::QueryFilesInDirectory(const String& Directory) {
	NTSTATUS status;
	Containers::List<FSObjectInformation> files;
	String fullDirectoryName = GetNtFileName(Directory);
	OBJECT_ATTRIBUTES attrbs;
	IO_STATUS_BLOCK io_blk = { 0 };
	UNICODE_STRING str;

	RtlInitUnicodeStringEx(&str, (CharStr)(ConstCharStr)fullDirectoryName);
	InitializeObjectAttributes(&attrbs, &str, OBJ_CASE_INSENSITIVE, NULL, NULL);
	HANDLE directoryHandle;

	status = NtCreateFile(
		&directoryHandle,
		FILE_LIST_DIRECTORY | FILE_TRAVERSE | SYNCHRONIZE,
		&attrbs,
		&io_blk,
		NULL,
		NULL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE,
		NULL,
		0
	);

	if (!NT_SUCCESS(status)) {
		Throw(NtException(ExceptionPosition, status));
	}

	PFILE_DIRECTORY_INFORMATION data = (PFILE_DIRECTORY_INFORMATION)Memory::Heap().AllignedAllocate(1Mb, Memory::DataSizeT(8));
	PFILE_DIRECTORY_INFORMATION ptr;
	
	SizeT TryAllocating = 1;

	try {
		do {
			status = NtQueryDirectoryFile(
				directoryHandle,
				NULL,
				NULL,
				NULL,
				&io_blk,
				data,
				10240,
				FileDirectoryInformation,
				FALSE,
				NULL,
				FALSE
			);

			if (status == STATUS_NO_MORE_FILES)
				break;

			if ((status == STATUS_SUCCESS && io_blk.Information == 0)
				|| status == STATUS_BUFFER_OVERFLOW
				|| status == STATUS_BUFFER_TOO_SMALL)
			{
				Memory::Heap().AllignedFree(data, Memory::DataSizeT(8));
				TryAllocating++;
				data = (PFILE_DIRECTORY_INFORMATION)Memory::Heap().AllignedAllocate(Memory::DataSizeT((1Mb).Bytes * TryAllocating), Memory::DataSizeT(8));
				continue;
			}

			if (!NT_SUCCESS(status))
			{
				break;
			}

			ptr = data;
			do {
				FSObjectInformation::Types type = FSObjectInformation::Types::Uknown;

				if ((ptr->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					type = FSObjectInformation::Types::Directory;
				}
				else
				{
					type = FSObjectInformation::Types::File;
				}

				files += FSObjectInformation(
					type,
					((ptr->FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0),
					(ptr->FileAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_COMPRESSED) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_READONLY) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0,

					Memory::DataSizeT((SizeT)ptr->EndOfFile.QuadPart),
					String((PWCHAR)&ptr->FileName, ptr->FileNameLength / 2) //-V106
				);

				if (ptr->NextEntryOffset == 0)
				{
					break;
				}

				ptr = (PFILE_DIRECTORY_INFORMATION)(((Byte*)ptr) + ptr->NextEntryOffset); //-V104
			} while (true);
		} while (true);
	}
	catch (...) {
		Memory::Heap().AllignedFree(data, Memory::DataSizeT(8));
		NtClose(directoryHandle);
		ReThrow;
	}
	Memory::Heap().AllignedFree(data, Memory::DataSizeT(8));
	NtClose(directoryHandle);
	return files;
}

Containers::List<IO::FS::FSObjectInformation> IO::FS::QueryFilesInDirectory(const String& Directory, const String& WilCardPatern) {
	NTSTATUS status;
	Containers::List<FSObjectInformation> files;
	String fullDirectoryName = GetNtFileName(Directory);
	OBJECT_ATTRIBUTES attrbs;
	IO_STATUS_BLOCK io_blk = { 0 };
	UNICODE_STRING str;
	UNICODE_STRING wildcard;

	RtlInitUnicodeStringEx(&str, (CharStr)(ConstCharStr)fullDirectoryName);
	RtlInitUnicodeStringEx(&wildcard, (CharStr)(ConstCharStr)WilCardPatern);
	InitializeObjectAttributes(&attrbs, &str, OBJ_CASE_INSENSITIVE, NULL, NULL);
	HANDLE directoryHandle;

	status = NtCreateFile(
		&directoryHandle,
		FILE_LIST_DIRECTORY | FILE_TRAVERSE | SYNCHRONIZE,
		&attrbs,
		&io_blk,
		NULL,
		NULL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE,
		NULL,
		0
	);

	if (!NT_SUCCESS(status)) {
		Throw(NtException(ExceptionPosition, status));
	}

	PFILE_DIRECTORY_INFORMATION data = (PFILE_DIRECTORY_INFORMATION)Memory::Heap().AllignedAllocate(1Mb, Memory::DataSizeT(8));
	PFILE_DIRECTORY_INFORMATION ptr;

	SizeT TryAllocating = 1;

	try {
		do {
			status = NtQueryDirectoryFile(
				directoryHandle,
				NULL,
				NULL,
				NULL,
				&io_blk,
				data,
				10240,
				FileDirectoryInformation,
				FALSE,
				&wildcard,
				FALSE
			);

			if (status == STATUS_NO_MORE_FILES)
				break;

			if ((status == STATUS_SUCCESS && io_blk.Information == 0) 
				|| status == STATUS_BUFFER_OVERFLOW 
				|| status == STATUS_BUFFER_TOO_SMALL)
			{
				Memory::Heap().AllignedFree(data, Memory::DataSizeT(8));
				TryAllocating++;
				data = (PFILE_DIRECTORY_INFORMATION)Memory::Heap().AllignedAllocate(Memory::DataSizeT((1Mb).Bytes * TryAllocating), Memory::DataSizeT(8));
				continue;
			}

			if (status == STATUS_NO_SUCH_FILE) {
				break;
			}

			if (!NT_SUCCESS(status))
			{
				break;
			}

			ptr = data;
			do {
				FSObjectInformation::Types type = FSObjectInformation::Types::Uknown;

				if ((ptr->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					type = FSObjectInformation::Types::Directory;
				}
				else
				{
					type = FSObjectInformation::Types::File;
				}

				files += FSObjectInformation(
					type,
					((ptr->FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0),
					(ptr->FileAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_COMPRESSED) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_READONLY) != 0,
					(ptr->FileAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0,

					Memory::DataSizeT((SizeT)ptr->EndOfFile.QuadPart),
					String((PWCHAR)&ptr->FileName, ptr->FileNameLength / 2) //-V106
				);

				if (ptr->NextEntryOffset == 0)
				{
					break;
				}

				ptr = (PFILE_DIRECTORY_INFORMATION)(((Byte*)ptr) + ptr->NextEntryOffset); //-V104
			} while (ptr->NextEntryOffset != 0);
		} while (true);
	}
	catch (...) {
		Memory::Heap().AllignedFree(data, Memory::DataSizeT(8));
		NtClose(directoryHandle);
		ReThrow;
	}
	Memory::Heap().AllignedFree(data, Memory::DataSizeT(8));
	NtClose(directoryHandle);
	return files;
}

Seed::String IO::FS::GetNtFileName(const String& Win32Name) {
	wchar_t * buf = new wchar_t[32767];
	UNICODE_STRING str = { 0 };
	RtlGetFullPathName_U((CharStr)(ConstCharStr)Win32Name, 32767, buf, NULL);
	RtlDosPathNameToNtPathName_U(buf, &str, NULL, NULL);
	String result = str.Buffer;
	RtlFreeUnicodeString(&str);
	delete[] buf;
	return result;
}

bool Seed::Core::IO::FS::IsNtAdressFormat(const String& FileAdress) {
	return 
		FileAdress.IsStartWith(L"\\Device\\") || 
		FileAdress.IsStartWith(L"\\DosDevice\\") || 
		FileAdress.IsStartWith(L"\\SystemRoot");
}

void Seed::Core::IO::FS::File::Dispose() {
	NtClose(This->handle);
}

Seed::Core::IO::FS::File::File(String&& path, OpeningMode mode, OpenDisposition policy, bool ShareWithOs) : This(nullptr) {
	String fullPath;

	if (!IsNtAdressFormat(path))
		fullPath = GetNtFileName(path);
	else
		fullPath = MoveRValue(path);

	UNICODE_STRING str;
	RtlInitUnicodeString(&str, (CharStr)(ConstCharStr)fullPath);

	OBJECT_ATTRIBUTES attrb;
	InitializeObjectAttributes(&attrb, &str, NULL, NULL, NULL);
	ULONG FileMode = 0;
	ULONG Disposition = 0;
	ULONG ShareMode = 0;
	ULONG Attributes = FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_ALERT;

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

	HANDLE hFile = nullptr;
	IO_STATUS_BLOCK ioblk = { 0 };
	NTSTATUS status = NtCreateFile(
		&hFile,
		SYNCHRONIZE | FileMode,
		&attrb,
		&ioblk,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
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
			Throw(FileNotExistException(ExceptionPosition, fullPath));
		case STATUS_OBJECT_NAME_COLLISION:
			Throw(FileExistException(ExceptionPosition, fullPath));
		case STATUS_SHARING_VIOLATION:
			Throw(FileBusyException(ExceptionPosition, fullPath));
		case STATUS_FILE_IS_A_DIRECTORY:
			Throw(DirectoryIsNotFileException(ExceptionPosition, fullPath));
		case STATUS_FILE_CORRUPT_ERROR:
			Throw(FileIsCoruptedException(ExceptionPosition, fullPath));
		default:
			Throw(GenericFileException(ExceptionPosition, fullPath, L"Generic exception"));
		}
	}

	This = new Data(hFile, MoveRValue(fullPath), mode, policy);
}

IO::FS::File::File(const String& path, OpeningMode mode, OpenDisposition policy, bool ShareWithOs) : This(nullptr) {
	String fullPath;

	if (!IsNtAdressFormat(path))
		fullPath = GetNtFileName(path);
	else
		fullPath = path;

	UNICODE_STRING str;
	RtlInitUnicodeString(&str, (CharStr)(ConstCharStr)fullPath);

	OBJECT_ATTRIBUTES attrb;
	InitializeObjectAttributes(&attrb, &str, NULL, NULL, NULL);
	ULONG FileMode = 0;
	ULONG Disposition = 0;
	ULONG ShareMode = 0;
	ULONG Attributes = FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_ALERT;

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

	HANDLE hFile;
	IO_STATUS_BLOCK ioblk;
	NTSTATUS status = NtCreateFile(
		&hFile,
		SYNCHRONIZE | FileMode,
		&attrb,
		&ioblk,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
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
			Throw(FileNotExistException(ExceptionPosition, fullPath));
		case STATUS_OBJECT_NAME_COLLISION:
			Throw(FileExistException(ExceptionPosition, fullPath));
		case STATUS_SHARING_VIOLATION:
			Throw(FileBusyException(ExceptionPosition, fullPath));
		case STATUS_FILE_IS_A_DIRECTORY:
			Throw(DirectoryIsNotFileException(ExceptionPosition, fullPath));
		case STATUS_FILE_CORRUPT_ERROR:
			Throw(FileIsCoruptedException(ExceptionPosition, fullPath));
		default:
			Throw(GenericFileException(ExceptionPosition, fullPath, L"Generic exception"));
		}
	}

	This = new Data(hFile, MoveRValue(fullPath), mode, policy);
}

void IO::FS::File::Read(SizeT Position, Containers::Blob& buffer) {
	if (!(This->mode.Read))
		Throw(FileModeNoSupportedByOperationException(ExceptionPosition, This->name));

	NTSTATUS status;

	if (this->GetSize() - Position < buffer.Count)
		Throw(CantReadMoreDataThanInExistInFileException(ExceptionPosition, This->name));
	LARGE_INTEGER pos;
	IO_STATUS_BLOCK ioblk;
	pos.QuadPart = Position;
	status = NtReadFile(
		This->handle,
		NULL,
		NULL,
		NULL,
		&ioblk,
		(void*)(Byte*)(const Byte* const)buffer,
		(ULONG)buffer.Count, //-V202
		&pos,
		NULL
	);

	if (!NT_SUCCESS(status))
		Throw(GenericFileException(ExceptionPosition, This->name, L"Can't read from file"));
}

void IO::FS::File::Write(SizeT Position, const Containers::Blob& buffer) {
	if (!(This->mode.Write))
		Throw(FileModeNoSupportedByOperationException(ExceptionPosition, This->name));

	NTSTATUS status;

	if (this->GetSize() - Position < buffer.Count)
		Throw(CantReadMoreDataThanInExistInFileException(ExceptionPosition, This->name));
	LARGE_INTEGER pos;
	IO_STATUS_BLOCK ioblk;
	pos.QuadPart = Position;
	status = NtWriteFile(
		This->handle,
		NULL,
		NULL,
		NULL,
		&ioblk,
		(void*)(Byte*)(const Byte* const)buffer,
		(ULONG)buffer.Count, //-V202
		&pos,
		NULL
	);

	if (!NT_SUCCESS(status))
		Throw(GenericFileException(ExceptionPosition, This->name, L"Can't write to file"));
}

void IO::FS::File::Read(Containers::Blob& buffer) {
	This->syncLock.ExclusiveLock([&] () mutable
	{
		if (!(This->mode.Read))
			Throw(FileModeNoSupportedByOperationException(ExceptionPosition, This->name));

		NTSTATUS status;

		if (this->GetSize() - This->readPosition < buffer.Count)
			Throw(CantReadMoreDataThanInExistInFileException(ExceptionPosition, This->name));
		LARGE_INTEGER pos;
		IO_STATUS_BLOCK ioblk;
		pos.QuadPart = This->readPosition;
		status = NtReadFile(
			This->handle,
			NULL,
			NULL,
			NULL,
			&ioblk,
			(void*)(Byte*)(const Byte* const)buffer,
			(ULONG)buffer.Count, //-V202
			&pos,
			NULL
		);

		if (!NT_SUCCESS(status))
			Throw(GenericFileException(ExceptionPosition, This->name, L"Can't read from file"));

		This->readPosition += buffer.Count;
	});
}

void IO::FS::File::Write(const Containers::Blob& buffer) {
	This->syncLock.ExclusiveLock([&] () mutable
	{
		if (!(This->mode.Write || This->mode.WriteThrought))
			Throw(FileModeNoSupportedByOperationException(ExceptionPosition, This->name));

		NTSTATUS status;

		if (this->GetSize() < This->writePosition)
			Throw(CantReadMoreDataThanInExistInFileException(ExceptionPosition, This->name));
		LARGE_INTEGER pos;
		IO_STATUS_BLOCK ioblk;
		pos.QuadPart = This->writePosition;
		status = NtWriteFile(
			This->handle,
			NULL,
			NULL,
			NULL,
			&ioblk,
			(void*)(Byte*)(const Byte* const)buffer,
			(ULONG)buffer.Count, //-V202
			&pos,
			NULL
		);

		if (!NT_SUCCESS(status))
			Throw(GenericFileException(ExceptionPosition, This->name, L"Can't read from file"));

		This->writePosition += buffer.Count;
	});
}

void IO::FS::File::Flush() {
	IO_STATUS_BLOCK ioblk;
	NTSTATUS status = NtFlushBuffersFile(This->handle, &ioblk);

	if (!NT_SUCCESS(status))
		Throw(GenericFileException(ExceptionPosition, This->name, L"Can't flush file"));
}

SizeT IO::FS::File::GetSize() const {
	FILE_STANDARD_INFORMATION  end_pos;
	IO_STATUS_BLOCK ioblk;
	NtQueryInformationFile(
		This->handle,
		&ioblk,
		&end_pos,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation
	);
	return end_pos.EndOfFile.QuadPart;
}

void IO::FS::File::Delete()&& {
	if (!(This->mode.Delete))
		Throw(FileModeNoSupportedByOperationException(ExceptionPosition, This->name));

	FILE_DISPOSITION_INFORMATION dispositionInformation{ TRUE };
	IO_STATUS_BLOCK ioblk;
	NTSTATUS status = NtSetInformationFile(
		This->handle,
		&ioblk,
		&dispositionInformation,
		sizeof(FILE_DISPOSITION_INFORMATION),
		FileDispositionInformation
		);

	if (!NT_SUCCESS(status))
		Throw(GenericFileException(ExceptionPosition, This->name, L"Can't close file"));
}

#pragma warning(pop)
