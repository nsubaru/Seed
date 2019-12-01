#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.NewSerilization.h"

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
                    class LogicSerilization : public ITest {
                        virtual bool Excecute() override;
                    };

                    class NumericSerilization : public ITest {
                        virtual bool Excecute() override;
                    };

                    class FloatPointSerilization : public ITest {
                        virtual bool Excecute() override;
                    };

                    class CharacterSerilization : public ITest {
                        virtual bool Excecute() override;
                    };

                    class CFixedArraySerilization : public ITest {
                        virtual bool Excecute() override;
                    };

                    class PrimitiveMetaData : public ITest {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}