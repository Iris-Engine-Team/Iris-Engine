using System;
using System.Runtime.InteropServices;

public class Collider : IComponent
{
    public Collider(IntPtr _colliderPtr): base(_colliderPtr) { }

    public void SetOffset(Vector3 _offset)
    {
        GCHandle handle = GCHandle.Alloc(_offset.GetData(), GCHandleType.Pinned);
        cSetOffset(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetOffset() { return InfoReader.ToVector3(cGetOffset(ComponentPtr)); }

    [DllImport("IrisEngine")]
    private static extern void cSetOffset(IntPtr _collider, IntPtr _offset);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetOffset(IntPtr _collider);
}