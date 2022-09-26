using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Seed.Serialization
{
    public interface ISeedSerializator
    {
        public void Serialize(dynamic data, Stream serializationStream);
        public void Deserialize(ref dynamic data, Stream serializationStream);

        public List<TypeRecord> GetSupportedTypes();
    }
}
