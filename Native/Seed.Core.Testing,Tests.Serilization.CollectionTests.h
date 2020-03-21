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
                namespace Serilization
                {
                    class StringSerilization final : public ITest {
                        virtual bool Excecute() override;
                    };

					class ArraySerilization final : public ITest {
                        virtual bool Excecute() override;
                    };

					class ListSerilization final : public ITest {
                        virtual bool Excecute() override;
                    };

					class DynamicQueue final : public ITest {
                        virtual bool Excecute() override;
                    };

					class FixedArray final : public ITest {
                        virtual bool Excecute() override;
                    };

					class FixedQueue final : public ITest {
                        virtual bool Excecute() override;
                    };

					class Dictionary final : public ITest {
                        virtual bool Excecute() override;
                    };

					class Map final : public ITest {
                        virtual bool Excecute() override;
                    };

					class PriorityQueue final : public ITest {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}