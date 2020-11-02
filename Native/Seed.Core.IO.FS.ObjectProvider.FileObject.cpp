// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.IO.FS.ObjectProvider.FileObject.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

using namespace Seed;
using namespace Seed::Core::IO::FS;

void Seed::Core::IO::FS::ObjectProvider::FileObject::Delete() {
	OBJECT_ATTRIBUTES attrbs;
	UNICODE_STRING str;

	auto fullPath = this->This->partition.PartitionPath + this->RelativePath;
	RtlInitUnicodeStringEx(&str, (CharStr) (ConstCharStr) fullPath);
	InitializeObjectAttributes(&attrbs, &str, NULL, NULL, NULL);

	NTSTATUS status = NtDeleteFile(&attrbs);

	if (!NT_SUCCESS(status))
	{
		Throw(NtException(ExceptionPosition, status));
	}
}