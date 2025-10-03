using System;
using System.Runtime.InteropServices;

public class Model : IComponent
{
    public Model(IntPtr _modelPtr) : base(_modelPtr) { }

    public void SetMesh(string _meshName)
    {
        IntPtr namePtr = Marshal.StringToHGlobalAnsi(_meshName);
        cSetMesh(ComponentPtr, namePtr);
        Marshal.FreeHGlobal(namePtr);
    }

    [DllImport("IrisEngine")]
    private static extern void cSetMesh(IntPtr _modelRenderer, IntPtr _meshName);
}