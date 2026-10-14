using Microsoft.VisualStudio.TestPlatform.ObjectModel.Adapter;
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.PortableExecutable;
using System.Text;
using System.Threading;

namespace Seed.Testing.TestAdapter
{
    internal static class AdapterHost
    {
        internal static bool IsSeedImage(string source)
        {
            try
            {
                using (var file = File.OpenRead(source))
                using (var reader = new PEReader(file))
                {
                    var subsystem = reader.PEHeaders.PEHeader?.Subsystem;
                    if (subsystem != Subsystem.Native && subsystem != Subsystem.WindowsCui && subsystem != Subsystem.WindowsGui)
                    {
                        return false;
                    }

                    var section = reader.GetSectionData(".type");

                    return section.Length > 0 && Encoding.ASCII.GetString(section.GetContent().ToArray()).Contains("SEED_RXX_RTTI\0");
                }
            }
            catch (Exception exception) when (exception is IOException || exception is BadImageFormatException)
            {
                return false;
            }
        }

        internal static string NewOutputPath()
        {
            string directory = Path.Combine(Path.GetTempPath(), "Seed.TestAdapter", Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(directory);

            return Path.Combine(directory, "result.xml");
        }

        // Windows command-line quoting, including trailing slashes before a quote.
        private static string Quote(string value)
        {
            var text = new StringBuilder("\"");
            int slashes = 0;
            foreach (char character in value)
            {
                if (character == '\\')
                {
                    ++slashes;
                    continue;
                }

                if (character == '"')
                {
                    text.Append('\\', slashes * 2 + 1);
                }
                else
                {
                    text.Append('\\', slashes);
                }

                slashes = 0;
                text.Append(character);
            }

            return text.Append('\\', slashes * 2).Append('"').ToString();
        }

        internal static void Invoke(string mode, string source, string selection, string output, CancellationToken? cancellation, Action<int>? attach)
        {
            string assemblyDirectory = Path.GetDirectoryName(typeof(AdapterHost).GetTypeInfo().Assembly.Location)!;
            string host = Path.Combine(assemblyDirectory, "host", "Seed.Testing.Host.exe");
            if (!File.Exists(host))
            {
                throw new FileNotFoundException("The Seed .NET 10 test host is missing; build or restore the complete test adapter package.", host);
            }

            using (var process = new Process())
            {
                process.StartInfo = new ProcessStartInfo(host, string.Join(" ", new[]
                {
                    mode, Path.GetFullPath(source), selection, output
                }.Select(Quote)))
                {
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    RedirectStandardInput = true
                };
                var diagnostics = new StringBuilder();
                var gate = new object();
                Exception? attachmentError = null;
                process.OutputDataReceived += (_, item) =>
                {
                    if (item.Data is null)
                    {
                        return;
                    }

                    lock (gate)
                    {
                        diagnostics.AppendLine(item.Data);
                        if (diagnostics.Length > 65536)
                        {
                            diagnostics.Remove(0, diagnostics.Length - 65536);
                        }
                    }

                    if (mode == "run" && item.Data.StartsWith("SEED_PROCESS ", StringComparison.Ordinal))
                    {
                        try
                        {
                            attach?.Invoke(int.Parse(item.Data.Substring(13)));
                            process.StandardInput.WriteLine("run");
                        }
                        catch (Exception exception)
                        {
                            attachmentError = exception;
                            process.StandardInput.WriteLine("cancel");
                        }
                    }
                };
                process.ErrorDataReceived += (_, item) =>
                {
                    if (item.Data is not null)
                    {
                        lock (gate)
                        {
                            diagnostics.AppendLine(item.Data);
                            if (diagnostics.Length > 65536)
                            {
                                diagnostics.Remove(0, diagnostics.Length - 65536);
                            }
                        }
                    }
                };
                process.Start();
                process.BeginOutputReadLine();
                process.BeginErrorReadLine();
                var elapsed = Stopwatch.StartNew();
                bool cancelSent = false;
                while (!process.WaitForExit(100))
                {
                    if (!cancelSent && (cancellation?.IsCancellationRequested == true || elapsed.Elapsed > TimeSpan.FromMinutes(16)))
                    {
                        try
                        {
                            process.StandardInput.WriteLine("cancel");
                        }
                        catch (IOException)
                        {
                        }

                        cancelSent = true;
                        elapsed.Restart();
                    }

                    if (cancelSent && elapsed.Elapsed > TimeSpan.FromSeconds(40))
                    {
                        throw new TimeoutException($"Seed test host did not finish owned-process cleanup. Host PID: {process.Id}; artifacts: {output}");
                    }
                }

                process.WaitForExit();
                if (attachmentError is not null)
                {
                    throw new InvalidOperationException("Native debugger attachment failed.", attachmentError);
                }

                cancellation?.ThrowIfCancellationRequested();
                if (process.ExitCode != 0 || !File.Exists(output))
                {
                    throw new InvalidOperationException($"Seed test host failed ({process.ExitCode}); artifacts: {output}\n{diagnostics}");
                }
            }
        }
    }
}
