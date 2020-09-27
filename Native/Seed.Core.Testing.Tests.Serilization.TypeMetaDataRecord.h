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
                    class TypeMetadataRecordCreate final //: public TestClass<TypeMetadataRecordCreate> 
                    {
                        virtual bool Excecute();
                    };

                    class TypeMetaDataRecordDecode final //: public TestClass<TypeMetaDataRecordDecode> 
                    {
                        virtual bool Excecute();
                    };

                    class TypeMetaDataRecordSerelize final //: public TestClass<TypeMetaDataRecordSerelize> 
                    {
                        virtual bool Excecute();
                    };
                }
            }
        }
    }
}