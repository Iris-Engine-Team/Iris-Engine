using System;
using System.Runtime.InteropServices;

public class Transform : IComponent
{
    public Transform(IntPtr _transformPtr) : base(_transformPtr) { }

    public IntPtr Ptr() { return ComponentPtr; }

    public void SetPosition(Vector3 _position)
    {
        GCHandle handle = GCHandle.Alloc(_position.GetData(), GCHandleType.Pinned);
        cSetTransformPosition(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetPosition() { return InfoReader.ToVector3(cGetTransformPosition(ComponentPtr)); }
    public void SetGlobalPosition(Vector3 _position)
    {
        GCHandle handle = GCHandle.Alloc(_position.GetData(), GCHandleType.Pinned);
        cSetTransformGlobalPosition(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetGlobalPosition() { return InfoReader.ToVector3(cGetTransformGlobalPosition(ComponentPtr)); }
    public void SetScale(Vector3 _scale)
    {
        GCHandle handle = GCHandle.Alloc(_scale.GetData(), GCHandleType.Pinned);
        cSetScale(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetScale() { return InfoReader.ToVector3(cGetScale(ComponentPtr)); }
    public void SetGlobalScale(Vector3 _scale)
    {
        GCHandle handle = GCHandle.Alloc(_scale.GetData(), GCHandleType.Pinned);
        cSetGlobalScale(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetGlobalScale() { return InfoReader.ToVector3(cGetGlobalScale(ComponentPtr)); }
    public void SetEuler(Vector3 _rotation)
    {
        GCHandle handle = GCHandle.Alloc(_rotation.GetData(), GCHandleType.Pinned);
        cSetEuler(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetEuler() { return InfoReader.ToVector3(cGetEuler(ComponentPtr)); }
    public void SetGlobalEuler(Vector3 _rotation)
    {
        GCHandle handle = GCHandle.Alloc(_rotation.GetData(), GCHandleType.Pinned);
        cSetGlobalEuler(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetGlobalEuler() { return InfoReader.ToVector3(cGetGlobalEuler(ComponentPtr)); }
    public void SetRotation(Quaternion _rotation)
    {
        GCHandle handle = GCHandle.Alloc(_rotation.GetData(), GCHandleType.Pinned);
        cSetTransformRotation(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Quaternion GetRotation() { return InfoReader.ToQuaternion(cGetTransformRotation(ComponentPtr)); }
    public void SetGlobalRotation(Quaternion _rotation)
    {
        GCHandle handle = GCHandle.Alloc(_rotation.GetData(), GCHandleType.Pinned);
        cSetGlobalRotation(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Quaternion GetGlobalRotation() { return InfoReader.ToQuaternion(cGetGlobalRotation(ComponentPtr)); }
    public Vector3 GetRight() { return InfoReader.ToVector3(cGetTransformRight(ComponentPtr)); }
    public Vector3 GetUp() { return InfoReader.ToVector3(cGetTransformUp(ComponentPtr)); }
    public Vector3 GetForward() { return InfoReader.ToVector3(cGetTransformForward(ComponentPtr)); }
    public void SetParent(Transform _parent) { cSetParent(ComponentPtr, _parent.Ptr()); }
    public Transform GetParent(Transform _parent) { return new Transform(cGetParent(ComponentPtr)); }


    [DllImport("IrisEngine")]
    private static extern void cSetTransformPosition(IntPtr _transform, IntPtr _position);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetTransformPosition(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern void cSetTransformGlobalPosition(IntPtr _transform, IntPtr _position);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetTransformGlobalPosition(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern void cSetScale(IntPtr _transform, IntPtr _scale);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetScale(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern void cSetGlobalScale(IntPtr _transform, IntPtr _scale);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetGlobalScale(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern void cSetEuler(IntPtr _transform, IntPtr _rotation);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetEuler(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern void cSetGlobalEuler(IntPtr _transform, IntPtr _rotation);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetGlobalEuler(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern void cSetTransformRotation(IntPtr _transform, IntPtr _rotation);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetTransformRotation(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern void cSetGlobalRotation(IntPtr _transform, IntPtr _rotation);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetGlobalRotation(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetTransformRight(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetTransformUp(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetTransformForward(IntPtr _transform);
        
    [DllImport("IrisEngine")]
    private static extern void cSetParent(IntPtr _transform, IntPtr _parent);
        
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetParent(IntPtr _transform);
}