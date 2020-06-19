#pragma once
#include "Seed.Core.Testing.Test.h"

namespace Seed {
	inline namespace Core {
		namespace Testing::Test::FS::ObjectProvider {
			class QueryWindowsFolder final : public TestClass<QueryWindowsFolder> {
				virtual bool Excecute() override;
			};

			class QueryUsersFolder final : public TestClass<QueryWindowsFolder> {
				virtual bool Excecute() override;
			};
		}
	}
}