using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Seed.Serialization
{
    public interface ICustomClassSerelizator
    {
        Type ClrType { get; set; }

        string TypeName { get; set; }

        Guid FullTypeId { get; set; }

        void Serialize(dynamic data, Stream serializationStream);

        object Deserialize(Stream serializationStream);
    }
}
