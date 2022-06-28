using Seed.CTTI.ClassSerializators;
using Seed.CTTI.MetaData;
using Seed.RTTI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
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

        SeedSerializatorFactory serializatorFactory;

        private ISeedSerializator serializator;

        private List<RttyTypeRecord> ValidTypesList = new List<RttyTypeRecord>();

        private Dictionary<Guid, ITypeSerializator> TypeSerializators = new Dictionary<Guid, ITypeSerializator>();

        SeedTypesSerializator(RTTISection rttiSection)
        {
            serializatorFactory = new SeedSerializatorFactory(rttiSection);
            LoadRTTIAndFindClrTypes(rttiSection);
        }

        private void LoadRTTIAndFindClrTypes(RTTISection rttiSection)
        {
            foreach (var item in rttiSection.AllRegisteredTypes.Where(x => x.IsValid).ToList())
            {
                var ClrType = serializatorFactory.BuildClrType(item, rttiSection);

                if (ClrType == null)
                {
                    continue;
                }

                var typeRecord = new RttyTypeRecord
                {
                    TypeName = item.TypeName,
                    Type = item.TypeClass,
                    SerializationSchema = item.SerializationSchema,
                    FullTypeId = item.FullTypeId.Id,
                    ClrType = ClrType
                };

                ValidTypesList.Add(typeRecord);

                typeof(SeedSerializatorFactory).GetMethod("AddType")
                    .MakeGenericMethod(ClrType)
                    .Invoke(serializatorFactory, new object[] { typeRecord });
            }
        }

        public object Deserialize(Stream serializationStream)
        {
            return serializator.Deserialize(serializationStream);
        }

        public void Serialize(Stream serializationStream, object graph)
        {
            if (!ValidTypesList.Any(x => x.ClrType == graph.GetType()))
            {
                serializator.Serialize(graph, serializationStream);
            }
        }
    }
}
