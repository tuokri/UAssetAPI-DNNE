using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;
using UAssetAPI;
using UAssetAPI.UnrealTypes;

namespace UAssetAPI_DNNE;

// ReSharper disable once ClassNeverInstantiated.Global
public class Exports
{
    [SuppressMessage("ReSharper", "InconsistentNaming")]
    [SuppressMessage("ReSharper", "NotAccessedField.Global")]
    public unsafe struct CsResult
    {
        public void* value = null;
        public char* error = null;
        public byte hasError = 0;

        public CsResult()
        {
        }
    };

    [UnmanagedCallersOnly(
        CallConvs = [typeof(System.Runtime.CompilerServices.CallConvCdecl)],
        EntryPoint = "UAssetAPI_MyExport")
    ]
    public static int MyExport(int a)
    {
        return a + 1;
    }

    [DNNE.C99DeclCode(
        """
        #include "uassetapi_dnne/result.h"
        #include "uassetapi_dnne/engineversion.h"
        """
    )]
    [UnmanagedCallersOnly(
        CallConvs = [typeof(System.Runtime.CompilerServices.CallConvCdecl)],
        EntryPoint = "UAssetAPI_ToJson")
    ]
    [return: DNNE.C99Type("CsResult*")]
    public static unsafe CsResult* ToJson(int engineVersion)
    {
        var result = new CsResult();

        var str = new string("wtf lol?");
        fixed (char* err = str)
        {
            result.error = err;
        }

        result.hasError = 0;
        result.error = null;

        var todoAsset = new UAsset("todo", EngineVersion.UNKNOWN);
        var json = todoAsset.SerializeJson(Newtonsoft.Json.Formatting.Indented);

        var data = json.ToCharArray();
        fixed (char* charData = data)
        {
            result.value = charData;
        }

        return &result;
    }
}