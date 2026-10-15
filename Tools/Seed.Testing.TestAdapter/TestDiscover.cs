using Microsoft.VisualStudio.TestPlatform.ObjectModel;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Adapter;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Logging;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Xml.Linq;

namespace Seed.Testing.TestAdapter
{
    /// <summary>
    /// Discovers native Seed ITest registrations through the matching runtime's live RTTI and KMS endpoint.
    /// </summary>
    [FileExtension(".exe")]
    [DefaultExecutorUri(TestExecutor.ExecutorUriString)]
    [Category("native")]
    public sealed class TestDiscover : ITestDiscoverer
    {
        /// <summary>
        /// Discovers tests in supplied native Seed images and reports complete cases to Test Explorer.
        /// </summary>
        /// <param name="sources">Executable paths supplied by the test platform.</param>
        /// <param name="discoveryContext">Platform discovery settings.</param>
        /// <param name="logger">Platform diagnostic sink.</param>
        /// <param name="discoverySink">Sink for discovered native cases.</param>
        public void DiscoverTests(IEnumerable<string> sources, IDiscoveryContext discoveryContext, IMessageLogger logger, ITestCaseDiscoverySink discoverySink)
        {
            foreach (string source in sources)
            {
                if (!AdapterHost.IsSeedImage(source))
                {
                    continue;
                }

                try
                {
                    foreach (var test in FindTests(source))
                    {
                        discoverySink.SendTestCase(test);
                    }
                }
                catch (Exception exception)
                {
                    logger.SendMessage(TestMessageLevel.Error, $"Seed discovery failed for {source}: {exception}");
                }
            }
        }

        internal static List<TestCase> FindTests(string source)
        {
            string output = AdapterHost.NewOutputPath();
            
            AdapterHost.Invoke("discover", source, "-", output, null, null);

            return XDocument.Load(output).Descendants("test").Select(test => Create(source, (string)test.Attribute("name")!)).ToList();
        }

        internal static TestCase Create(string source, string name) => new TestCase(name.Replace("::", "."), new Uri(TestExecutor.ExecutorUriString), source)
        {
            DisplayName = name
        };
    }
}
