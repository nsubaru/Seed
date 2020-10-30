#pragma once
#include "Seed.Core.Memory.DataSize.h"
#include "Seed.Core.CTTI.TypeTraits.StructRemoteDefinition.h"

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			namespace TypeTraits {
				template<>
				struct StructCttiShema<Seed::Core::Memory::DataSizeT> : Core::TypeTraits::LogicTrue {
					StructCttiDef(Seed::Core::Memory::DataSizeT,
						DefStructFieldData(Seed::Core::Memory::DataSizeT, Bytes)
					);
				};
			}
		}
	}
}