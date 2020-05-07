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
