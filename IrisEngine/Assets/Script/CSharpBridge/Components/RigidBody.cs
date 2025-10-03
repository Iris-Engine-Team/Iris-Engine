using System;
using System.Runtime.InteropServices;

public class RigidBody : IComponent
{
    public RigidBody(IntPtr _rigidBodyPtr) : base(_rigidBodyPtr) { }

    public void SetPosition(Vector3 _position)
    {
        GCHandle handle = GCHandle.Alloc(_position.GetData(), GCHandleType.Pinned);
        cSetRigidBodyPosition(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetPosition() { return InfoReader.ToVector3(cGetRigidBodyPosition(ComponentPtr)); }
    public void SetPhysiquePosition() { cSetPhysiquePosition(ComponentPtr); }
    public void SetTargetPosition(Vector3 _position)
    {
        GCHandle handle = GCHandle.Alloc(_position.GetData(), GCHandleType.Pinned);
        cSetTargetPosition(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public void SetRotation(Quaternion _rotation)
    {
        GCHandle handle = GCHandle.Alloc(_rotation.GetData(), GCHandleType.Pinned);
        cSetRigidBodyRotation(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Quaternion GetRotation() { return InfoReader.ToQuaternion(cGetRigidBodyRotation(ComponentPtr)); }
    public void SetVelocity(Vector3 _velocity)
    {
        GCHandle handle = GCHandle.Alloc(_velocity.GetData(), GCHandleType.Pinned);
        cSetVelocity(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetVelocity() { return InfoReader.ToVector3(cGetVelocity(ComponentPtr)); }
    public void AddForce(Vector3 _force)
    {
        GCHandle handle = GCHandle.Alloc(_force.GetData(), GCHandleType.Pinned);
        cAddForce(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public void SetPosInterpolation(bool _activate) { cSetPosInterpolation(ComponentPtr, _activate); }
    public void SetRotationConstraint(Vector3 _constraints)
    {
        GCHandle handle = GCHandle.Alloc(_constraints.GetData(), GCHandleType.Pinned);
        cSetRotationConstraint(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }


    [DllImport("IrisEngine")]
    private static extern void cSetRigidBodyPosition(IntPtr _rigidBody, IntPtr _position);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetRigidBodyPosition(IntPtr _rigidBody);
        
    [DllImport("IrisEngine")]
    private static extern void cSetPhysiquePosition(IntPtr _rigidBody);
        
    [DllImport("IrisEngine")]
    private static extern void cSetTargetPosition(IntPtr _rigidBody, IntPtr _position);
    
    [DllImport("IrisEngine")]
    private static extern void cSetRigidBodyRotation(IntPtr _rigidBody, IntPtr _rotation);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetRigidBodyRotation(IntPtr _rigidBody);
        
    [DllImport("IrisEngine")]
    private static extern void cSetVelocity(IntPtr _rigidBody, IntPtr _velocity);
    
    [DllImport("IrisEngine")]
    private static extern IntPtr cGetVelocity(IntPtr _rigidBody);
        
    [DllImport("IrisEngine")]
    private static extern void cAddForce(IntPtr _rigidBody, IntPtr _force);
    
    [DllImport("IrisEngine")]
    private static extern void cSetPosInterpolation(IntPtr _rigidBody, bool _activate);
    
    [DllImport("IrisEngine")]
    private static extern void cSetRotationConstraint(IntPtr _rigidBody, IntPtr _constraints);
}