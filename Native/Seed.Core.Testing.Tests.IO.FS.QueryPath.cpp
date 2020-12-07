// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Seed.Core.IO.FS.h"

#include "Seed.Core.Testing.Tests.IO.FS.QueryPath.h"

bool Seed::Core::Testing::Test::IO::FS::API::QueryPath::Excecute() {
	Asserts::AssertTrue([] () InlineLambda
	{
		return Core::IO::FS::QueryFilesInDirectory(L".\\").Any([] (const Core::IO::FS::FSObjectInformation& object) InlineLambda
		{
			return object.Name.Containe(L"Seed.CoreLog.log");
		});
	}, ExceptionPosition);

	return true;
}
