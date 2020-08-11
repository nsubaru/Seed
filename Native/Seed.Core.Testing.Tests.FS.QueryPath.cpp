// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Seed.Core.FS.h"

#include "Seed.Core.Testing.Tests.FS.QueryPath.h"

bool Seed::Core::Testing::Test::FS::API::QueryPath::Excecute() {
	Asserts::AssertTrue([] () InlineLambda
	{
		return Core::FS::QueryFilesInDirectory(L".\\").Any([] (const Core::FS::FSObjectInformation& object) InlineLambda
		{
			return object.Name.Containe(L"Seed.CoreLog.log");
		});
	});

	return true;
}
