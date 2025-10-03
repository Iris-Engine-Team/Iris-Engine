using System;
using System.Runtime.InteropServices;

public static class Scene
{
    private static IntPtr ScenePtr = IntPtr.Zero;

    [UnmanagedCallersOnly]
    public static void Init(IntPtr ptr)
    {
        ScenePtr = ptr;
    }

    public static string GetSceneName() { return InfoReader.ToString(cGetSceneName(ScenePtr)); }
    public static IrisObject Instantiate(Vector3 _position, Transform? _parent = null, string _name = "")
    {
        IntPtr namePtr = Marshal.StringToHGlobalAnsi(_name);
        GCHandle handle = GCHandle.Alloc(_position.GetData(), GCHandleType.Pinned);
        IntPtr parentPtr = _parent?.Ptr() ?? IntPtr.Zero;

        IrisObject obj = new IrisObject(cShortInstantiate(ScenePtr, handle.AddrOfPinnedObject(), parentPtr, namePtr));

        handle.Free();
        Marshal.FreeHGlobal(namePtr);

        return obj;
    }
    public static IrisObject Instantiate(Vector3 _position, Vector3 _rotation, Vector3 _scale, Transform? _parent = null, string _name = "")
    {
        IntPtr namePtr = Marshal.StringToHGlobalAnsi(_name);
        GCHandle posHandle = GCHandle.Alloc(_position.GetData(), GCHandleType.Pinned);
        GCHandle rotaHandle = GCHandle.Alloc(_rotation.GetData(), GCHandleType.Pinned);
        GCHandle scaleHandle = GCHandle.Alloc(_scale.GetData(), GCHandleType.Pinned);
        IntPtr parentPtr = _parent?.Ptr() ?? IntPtr.Zero;

        IrisObject obj = new IrisObject(cEulerInstantiate(ScenePtr, posHandle.AddrOfPinnedObject(), rotaHandle.AddrOfPinnedObject(), scaleHandle.AddrOfPinnedObject(), parentPtr, namePtr));

        scaleHandle.Free();
        rotaHandle.Free();
        posHandle.Free();
        Marshal.FreeHGlobal(namePtr);

        return obj;
    }
    public static IrisObject Instantiate(Vector3 _position, Quaternion _rotation, Vector3 _scale, Transform? _parent = null, string _name = "")
    {
        IntPtr namePtr = Marshal.StringToHGlobalAnsi(_name);
        GCHandle posHandle = GCHandle.Alloc(_position.GetData(), GCHandleType.Pinned);
        GCHandle rotaHandle = GCHandle.Alloc(_rotation.GetData(), GCHandleType.Pinned);
        GCHandle scaleHandle = GCHandle.Alloc(_scale.GetData(), GCHandleType.Pinned);
        IntPtr parentPtr = _parent?.Ptr() ?? IntPtr.Zero;

        IrisObject obj = new IrisObject(cQuaternionInstantiate(ScenePtr, posHandle.AddrOfPinnedObject(), rotaHandle.AddrOfPinnedObject(), scaleHandle.AddrOfPinnedObject(), parentPtr, namePtr));

        scaleHandle.Free();
        rotaHandle.Free();
        posHandle.Free();
        Marshal.FreeHGlobal(namePtr);

        return obj;
    }
    public static IrisObject GetObjectByID(int _objectID) { return new IrisObject(cGetObjectByID(ScenePtr, _objectID)); }
    public static IrisObject GetObjectByName(string _name) { IntPtr namePtr = Marshal.StringToHGlobalAnsi(_name); IrisObject obj = new IrisObject(cGetObjectByName(ScenePtr, namePtr)); Marshal.FreeHGlobal(namePtr); return obj; }
    //public static Camera GetMainCamera() { return new Camera(cGetMainCamera(ScenePtr)); }

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetSceneName(IntPtr _scenePtr);

    [DllImport("IrisEngine")]
    private static extern IntPtr cShortInstantiate(IntPtr _scene, IntPtr _position, IntPtr _parent, IntPtr _name);

    [DllImport("IrisEngine")]
    private static extern IntPtr cEulerInstantiate(IntPtr _scene, IntPtr _position, IntPtr _rotation, IntPtr _scale, IntPtr _parent, IntPtr _name);

    [DllImport("IrisEngine")]
    private static extern IntPtr cQuaternionInstantiate(IntPtr _scene, IntPtr _position, IntPtr _rotation, IntPtr _scale, IntPtr _parent, IntPtr _name);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetObjectByID(IntPtr _scene, int _objectID);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetObjectByName(IntPtr _scene, IntPtr _name);

    //[DllImport("IrisEngine")]
    //private static extern IntPtr cGetMainCamera(IntPtr _scene);
}