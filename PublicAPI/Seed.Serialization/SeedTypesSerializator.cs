#nullable enable

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Seed.CTTI;
using Seed.CTTI.ClassSerializators;
using Seed.CTTI.MetaData;
using Seed.RTTI;
using System;
using System.Collections.Generic;
using System.Diagnostics;
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

        private List<RttiTypeRecord> ValidTypesList = new List<RttiTypeRecord>();

        private Dictionary<Guid, ITypeSerializator> TypeSerializators = new Dictionary<Guid, ITypeSerializator>();

        public SeedTypesSerializator(RTTISection rttiSection)
        {
            serializatorFactory = new SeedSerializatorFactory(rttiSection);
            LoadRTTIAndFindClrTypes(rttiSection);
            var code = serializatorFactory.GetSerializatorClassCode();

            MetadataReference[] references = new MetadataReference[]
            {
                MetadataReference.CreateFromFile(typeof(System.Object).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(ISeedSerializator).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(DynamicObject).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(UniversalTypeIdentificator).Assembly.Location),
                MetadataReference.CreateFromFile(typeof(ETypes).Assembly.Location)
            };

            var comp = CSharpCompilation.Create(
                assemblyName: Path.GetRandomFileName(),
                syntaxTrees: new[] { CSharpSyntaxTree.ParseText(code) },
                references: references,
                options: new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary)
            );

            using var ms = new MemoryStream();

            var result = comp.Emit(ms);

            if (!result.Success)
            {
                var failures = result.Diagnostics.Where(diagnostic => diagnostic.IsWarningAsError || diagnostic.Severity == DiagnosticSeverity.Error);

                foreach (Diagnostic diagnostic in failures)
                {
                    Debug.WriteLine($"{diagnostic.Id} - {diagnostic.GetMessage()}");
                }

                throw new Exception("Failed compiler serilization code");
            }
            else
            {

            }

            serializator = comp.Assembly.GetTypeByMetadataName("Seed.Serializator.SeedSerializator") as ISeedSerializator;
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

                var typeRecord = new RttiTypeRecord
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
            throw new NotImplementedException();
            //return serializator.Deserialize(serializationStream);
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
