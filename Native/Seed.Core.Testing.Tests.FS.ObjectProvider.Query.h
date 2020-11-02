#pragma once
#include "Seed.Core.Testing.Test.h"

namespace Seed {
	inline namespace Core {
		namespace Testing::Test::IO::FS::ObjectProvider {
			class QueryWindowsFolder final : public TestClass<QueryWindowsFolder> {
				virtual bool Excecute() override;
			};

			class QueryUsersFolder final : public TestClass<QueryWindowsFolder> {
				virtual bool Excecute() override;
			};

			class QuerySystem32Folder final : public TestClass<QuerySystem32Folder> {
				virtual bool Excecute() override;
			};
		}
	}
}