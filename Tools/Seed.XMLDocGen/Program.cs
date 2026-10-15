using Seed.XMLDocGen;

var root = Path.GetFullPath(args.Length > 0 ? args[0] : Directory.GetCurrentDirectory());
var output = Path.GetFullPath(args.Length > 1 ? args[1] : Path.Combine(root, "Documentation"));

await DocumentationGenerator.GenerateAsync(root, output);
