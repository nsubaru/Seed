using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization
{
    public class SeedSerializableAttribute : System.Attribute
    {
        public string SeedKernelTypeName { get; private set; }

        public ETypes TypeClass { get; private set; }

        public SeedSerializableAttribute(string seedKernelTypeName, ETypes typeClass)
        {
            SeedKernelTypeName = seedKernelTypeName;
            TypeClass = typeClass;
        }
    }

    [System.AttributeUsage(System.AttributeTargets.Class |
                           System.AttributeTargets.Struct)
    ]
    public class SerializableStructAttribute : SeedSerializableAttribute
    {
        public SerializableStructAttribute(string seedKernelTypeName) 
            : base(seedKernelTypeName, ETypes.Struct)
        {}
    }

    [System.AttributeUsage(System.AttributeTargets.Class |
                           System.AttributeTargets.Struct)
    ]
    public class SerializableObjectAttribute : SeedSerializableAttribute
    {
        public SerializableObjectAttribute(string seedKernelTypeName)
             : base(seedKernelTypeName, ETypes.Struct)
        { }
    }

    [System.AttributeUsage(System.AttributeTargets.Property)]
    public class SerializableStructFieldAttribute : System.Attribute
    {
        public StructFieldInformation FieldInformation { get; private set; }
        public SerializableStructFieldAttribute(string fieldName, string fieldTypeName)
        {
            FieldInformation = new StructFieldInformation { 
                FieldName = fieldName, 
                FieldTypeName = fieldTypeName 
            };
        }
    }
}
