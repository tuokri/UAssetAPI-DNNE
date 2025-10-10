using System.Runtime.InteropServices;
using UAssetAPI;

namespace UAssetAPI_DNNE;

public class Exports
{
    [UnmanagedCallersOnly(EntryPoint = "UAssetAPI_MyExport")]
    public static int MyExport(int a)
    {
        return a;
    }
}