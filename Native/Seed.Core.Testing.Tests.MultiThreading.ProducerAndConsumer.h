#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Serilization.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Testing
        {
            namespace Test
            {
                namespace MultiThreading
                {
                    class ProducerAndConcumer final : public TestClass<ProducerAndConcumer> {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}