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
                    class StringSerilization final : public TestClass<StringSerilization> {
                        virtual bool Excecute() override;
                    };

					class ArraySerilization final : public TestClass<ArraySerilization> {
                        virtual bool Excecute() override;
                    };

					class ListSerilization final : public TestClass<ListSerilization> {
                        virtual bool Excecute() override;
                    };

					class DynamicQueue final : public TestClass<DynamicQueue> {
                        virtual bool Excecute() override;
                    };

					class FixedArray final : public TestClass<FixedArray> {
                        virtual bool Excecute() override;
                    };

					class FixedQueue final : public TestClass<FixedQueue> {
                        virtual bool Excecute() override;
                    };

					class Dictionary final : public TestClass<Dictionary> {
                        virtual bool Excecute() override;
                    };

					class Map final : public TestClass<Map> {
                        virtual bool Excecute() override;
                    };

					class PriorityQueue final : public TestClass<PriorityQueue> {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}