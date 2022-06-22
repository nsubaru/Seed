using Seed.CTTI.ClassSerializators;
using Seed.CTTI.MetaData;
using Seed.RTTI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace Seed.Serialization
{
    public class SeedTypesSerializator : IFormatter
    {
        #region NotUsed
        public SerializationBinder? Binder { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
        public StreamingContext Context { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
        public ISurrogateSelector? SurrogateSelector { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
        #endregion

        private struct RttyTypeRecord
        {
            public string TypeName { get; set; }
            public string SerializationSchema { get; set; }
            public Guid FullTypeId { get; set; }
        }

        private List<RttyTypeRecord> AllTypesList = new List<RttyTypeRecord>();

        private Dictionary<Guid, ITypeSerializator> TypeSerializators = new Dictionary<Guid, ITypeSerializator>();

        SeedTypesSerializator(RTTISection rttiSection)
        {
            LoadRTTITypes(rttiSection);
        }

        private void LoadRTTITypes(RTTISection rttiSection)
        {
            var decoder = new TypeMetaDataDecoder();

            var types = rttiSection.AllRegisteredTypes
                .Where(x => x.IsValid)
                .Select(x => new RttyTypeRecord()
                {
                    TypeName = x.TypeName,
                    SerializationSchema = x.SerializationSchema,
                    FullTypeId = x.FullTypeId.Id
                })
                .ToList();

            AllTypesList.AddRange(types);
        }


        public object Deserialize(Stream serializationStream)
        {
            throw new NotImplementedException();
        }

        public void Serialize(Stream serializationStream, object graph)
        {
            throw new NotImplementedException();
        }
    }
}
