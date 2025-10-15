using Scriban;
using System.CommandLine;
using System.Diagnostics;
using System.Reflection;
using Scriban.Runtime;
using UAssetAPI.UnrealTypes;

var inputOption = new Option<FileInfo>("-i", "--input")
{
    Required = true,
    Description = "Input template file to render"
};
inputOption.AcceptLegalFilePathsOnly();
inputOption.AcceptExistingOnly();

var outputOption = new Option<FileInfo>("-o", "--output")
{
    Required = false,
    Description = "Output file to write rendered template to",
};
outputOption.AcceptLegalFilePathsOnly();

var actionOption = new Option<RenderAction>("-r", "--render-action")
{
    Required = true,
};

var genCppOption = new Option<bool>("--generate-cpp")
{
    Description = "Generate C++ code if set, C otherwise",
    DefaultValueFactory = x => false,
};

var assembly = Assembly.GetExecutingAssembly();
var fvi = FileVersionInfo.GetVersionInfo(assembly.Location);
var rootCommand = new RootCommand($"UAssetAPI-DNNE.CodeGen {fvi.FileVersion}")
{
    inputOption,
    actionOption,
    outputOption,
    genCppOption,
};

rootCommand.SetAction(parseResult =>
{
    var file = parseResult.GetValue(inputOption);
    if (file == null)
    {
        throw new NullReferenceException("Invalid fileOption.");
    }

    var genCpp = parseResult.GetValue(genCppOption);
    var enumType = genCpp ? "enum class" : "enum";

    var outFile = parseResult.GetValue(outputOption);

    var renderAction = parseResult.GetValue(actionOption);

    Console.WriteLine($@"Rendering template: '{file.FullName}'");
    var template = Template.ParseLiquid(File.ReadAllText(file.FullName));

    switch (renderAction)
    {
        case RenderAction.EngineVersion:
            var script = new ScriptObject();
            script.Import(typeof(EngineVersionTuple));

            var evs = Enum.GetNames<EngineVersion>().Zip(Enum.GetValues<EngineVersion>().Cast<int>());
            var engineVersions = new List<EngineVersionTuple>();
            foreach (var (name, value) in evs)
            {
                engineVersions.Add(new EngineVersionTuple(name, value));
            }

            script.Add("EngineVersions", engineVersions.ToArray());
            script.Add("GenerateCPP", genCpp);
            script.Add("EnumType", enumType);
            var context = new LiquidTemplateContext();
            context.PushGlobal(script);

            var result = template.Render(context);

            if (outFile == null)
            {
                Console.WriteLine(result);
            }
            else
            {
                Console.WriteLine(@$"Writing: '{outFile.FullName}'");
                var dir = Path.GetDirectoryName(outFile.FullName);
                if (dir == null)
                {
                    throw new NullReferenceException("Output file directory is null.");
                }

                Directory.CreateDirectory(dir);
                File.WriteAllText(outFile.FullName, result);
            }

            break;
        default:
            throw new ArgumentOutOfRangeException($"Invalid RenderAction: {renderAction}.");
    }
});

return rootCommand.Parse(args).Invoke();

internal record EngineVersionTuple(string Name, int Value);

internal enum RenderAction
{
    EngineVersion,
};