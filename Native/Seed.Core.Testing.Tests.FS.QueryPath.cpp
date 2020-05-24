// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.FS.QueryPath.h"
#include "Seed.Core.FS.h"

bool Seed::Core::Testing::Test::FS::QueryPath::Excecute() {
	Asserts::AssertTrue([] ()
	{
		return Core::FS::QueryFilesInDirectory(L".\\").Any([] (const Core::FS::FSObject& object)
		{
			return object.Name.Containe(L"Seed.CoreLog.log");
		});
	});

	return true;
}
