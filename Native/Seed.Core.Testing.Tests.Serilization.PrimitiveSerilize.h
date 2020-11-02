#pragma once
#include "Seed.Core.Testing.Base.h"

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
                    class LogicSerilization : public TestClass<LogicSerilization> {
                        virtual bool Excecute() override;
                    };

                    class NumericSerilization : public TestClass<NumericSerilization> {
                        virtual bool Excecute() override;
                    };

                    class FloatPointSerilization : public TestClass<FloatPointSerilization> {
                        virtual bool Excecute() override;
                    };

                    class CharacterSerilization : public TestClass<CharacterSerilization> {
                        virtual bool Excecute() override;
                    };

                    class CFixedArraySerilization : public TestClass<CFixedArraySerilization> {
                        virtual bool Excecute() override;
                    };

                    class PrimitiveMetaData : public TestClass<PrimitiveMetaData> {
                        virtual bool Excecute() override;
                    };

                    class DataSizeTSerilization : public TestClass<DataSizeTSerilization> {
                        virtual bool Excecute() override;
                    };

                    class DataSizeTMetaData : public TestClass<DataSizeTMetaData> {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}