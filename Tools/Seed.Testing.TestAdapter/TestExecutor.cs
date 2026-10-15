using Microsoft.VisualStudio.TestPlatform.ObjectModel;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Adapter;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Logging;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading;
using System.Xml.Linq;

namespace Seed.Testing.TestAdapter
{
    /// <summary>
    /// Executes selected native Seed tests and maps their NUnit results into Visual Studio Test Explorer.
    /// </summary>
    [ExtensionUri(ExecutorUriString)]
    public sealed class TestExecutor : ITestExecutor2
    {
        /// <summary>
        /// Versioned executor identity for Seed native test cases.
        /// </summary>
        public const string ExecutorUriString = "executor://SeedTestRunner/v1";

        private readonly CancellationTokenSource canceled = new CancellationTokenSource();

        /// <summary>
        /// Cancels the active owned test session, allowing its host to shut down the native process.
        /// </summary>
        public void Cancel() => canceled.Cancel();

        /// <summary>
        /// Runs all discovered native tests from each supplied Seed executable.
        /// </summary>
        /// <param name="sources">Native executable paths.</param>
        /// <param name="runContext">Platform run and debugger settings.</param>
        /// <param name="frameworkHandle">Platform result and diagnostic sink.</param>
        public void RunTests(IEnumerable<string>? sources, IRunContext? runContext, IFrameworkHandle? frameworkHandle)
        {
            if (sources is null || runContext is null || frameworkHandle is null)
            {
                throw new ArgumentNullException("Native test execution requires sources, context and a framework handle.");
            }

            foreach (string source in sources.Where(AdapterHost.IsSeedImage))
            {
                try
                {
                    RunTests(TestDiscover.FindTests(source), runContext, frameworkHandle);
                }
                catch (Exception exception)
                {
                    frameworkHandle.SendMessage(TestMessageLevel.Error, exception.ToString());
                }
            }
        }

        /// <summary>
        /// Runs selected cases grouped by their matching native image, requiring a result for every case.
        /// </summary>
        /// <param name="tests">Cases selected by Test Explorer or the platform filter.</param>
        /// <param name="runContext">Platform run and debugger settings.</param>
        /// <param name="frameworkHandle">Platform result and diagnostic sink.</param>
        public void RunTests(IEnumerable<TestCase>? tests, IRunContext? runContext, IFrameworkHandle? frameworkHandle)
        {
            if (tests is null || runContext is null || frameworkHandle is null)
            {
                throw new ArgumentNullException("Native test execution requires cases, context and a framework handle.");
            }

            foreach (var group in tests.GroupBy(test => test.Source))
            {
                var filter = runContext.GetTestCaseFilter(new[]
                {
                    "FullyQualifiedName", "Name"
                }, name => name == "FullyQualifiedName" ? TestCaseProperties.FullyQualifiedName : name == "Name" ? TestCaseProperties.DisplayName : null);

                var selected = group
                    .GroupBy(test => test.FullyQualifiedName)
                    .Select(items => items.First())
                    .Where(test => filter is null || filter.MatchTestCase(test, name => name == "FullyQualifiedName" ? test.FullyQualifiedName : name == "Name" ? test.DisplayName : null))
                    .ToArray();

                if (selected.Length == 0)
                {
                    continue;
                }

                foreach (var test in selected)
                {
                    frameworkHandle.RecordStart(test);
                }

                try
                {
                    string output = AdapterHost.NewOutputPath();
                    string selection = Path.Combine(Path.GetDirectoryName(output)!, "selection.xml");

                    new XDocument(new XElement("tests", selected.Select(test => new XElement("test", new XAttribute("name", test.FullyQualifiedName.Replace(".", "::")))))).Save(selection);
                    
                    Action<int>? attach = null;

                    if (runContext.IsBeingDebugged)
                    {
                        if (!(frameworkHandle is IFrameworkHandle2 debugger))
                        {
                            throw new InvalidOperationException("The test platform cannot attach the native debugger.");
                        }

                        attach = processId =>
                        {
                            if (!debugger.AttachDebuggerToProcess(processId))
                            {
                                throw new InvalidOperationException("Debugger attachment was rejected.");
                            }
                        };
                    }

                    AdapterHost.Invoke("run", group.Key, selection, output, canceled.Token, attach);

                    var results = XDocument.Load(output).Descendants("test-case").ToDictionary(test => (string)test.Attribute("fullname")!);

                    var mapped = selected.Select(test =>
                    {
                        if (!results.TryGetValue(test.FullyQualifiedName.Replace(".", "::"), out var native))
                        {
                            throw new InvalidDataException("A selected native result is missing.");
                        }

                        var result = new TestResult(test)
                        {
                            Outcome         = (string?)native.Attribute("result") == "Passed" ? TestOutcome.Passed : (string?)native.Attribute("result") == "Skipped" && native.Element("reason")?.Element("message")?.Value == "Returned false" ? TestOutcome.Skipped : TestOutcome.Failed,
                            ErrorMessage    = native.Element("failure")?.Element("message")?.Value ?? native.Element("reason")?.Element("message")?.Value,
                            ErrorStackTrace = native.Element("failure")?.Element("stack-trace")?.Value,
                            Duration        = TimeSpan.FromSeconds(double.Parse((string?)native.Attribute("duration") ?? "0", CultureInfo.InvariantCulture))
                        };

                        return result;
                    }).ToArray();

                    foreach (var result in mapped)
                    {
                        frameworkHandle.RecordResult(result);
                        frameworkHandle.RecordEnd(result.TestCase, result.Outcome);
                    }
                }
                catch (Exception exception)
                {
                    frameworkHandle.SendMessage(TestMessageLevel.Error, exception.ToString());

                    foreach (var test in selected)
                    {
                        var result = new TestResult(test)
                        {
                            Outcome         = TestOutcome.Failed,
                            ErrorMessage    = exception.Message,
                            ErrorStackTrace = exception.ToString()
                        };

                        frameworkHandle.RecordResult(result);
                        frameworkHandle.RecordEnd(test, result.Outcome);
                    }
                }
            }
        }

        /// <summary>
        /// Uses explicit attachment to the native process instead of attaching to the managed host.
        /// </summary>
        /// <param name="sources">Executable paths supplied by the platform.</param>
        /// <param name="runContext">Current run settings.</param>
        /// <returns>False because tests execute inside the separately launched native image.</returns>
        public bool ShouldAttachToTestHost(IEnumerable<string>? sources, IRunContext runContext) => false;

        /// <summary>
        /// Uses explicit attachment to the native process for selected cases.
        /// </summary>
        /// <param name="tests">Selected native cases.</param>
        /// <param name="runContext">Current run settings.</param>
        /// <returns>False because the managed host does not execute the native test methods.</returns>
        public bool ShouldAttachToTestHost(IEnumerable<TestCase>? tests, IRunContext runContext) => false;
    }
}
