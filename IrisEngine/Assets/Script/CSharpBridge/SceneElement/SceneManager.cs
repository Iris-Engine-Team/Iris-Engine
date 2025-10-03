using System;
using System.Runtime.InteropServices;

public static class SceneManager
{
    private static IntPtr SceneManagerPtr = IntPtr.Zero;

    [UnmanagedCallersOnly]
    public static void Init(IntPtr _ptr)
    {
        SceneManagerPtr = _ptr;
    }

    public static void SwitchScene(string _name)
    {
        IntPtr namePtr = Marshal.StringToHGlobalAnsi(_name);
        cSwitchScene(SceneManagerPtr, namePtr);
        Marshal.FreeHGlobal(namePtr);
    }

    [DllImport("IrisEngine")]
    private static extern void cSwitchScene(IntPtr _sceneManagerPtr, IntPtr _name);
}