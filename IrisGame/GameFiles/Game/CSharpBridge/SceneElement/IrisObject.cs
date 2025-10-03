using System;
using System.Runtime.InteropServices;

public class IrisObject
{
    private IntPtr ObjPtr = IntPtr.Zero;

    public IrisObject(IntPtr _objPtr)
    {
        ObjPtr = _objPtr;
    }

    public void SetActive(bool _activate) { cSetActive(ObjPtr, _activate); }
    public void Destroy() { cDestroy(ObjPtr); }
    public int GetID() { return cGetID(ObjPtr); }
    public void SetName(string _name)
    {
        IntPtr namePtr = Marshal.StringToHGlobalAnsi(_name);
        cSetName(ObjPtr, namePtr);
        Marshal.FreeHGlobal(namePtr);
    }
    public string GetName() { return InfoReader.ToString(cGetName(ObjPtr)); }
    public Camera GetCamera() { return new Camera(cGetCameraComponent(ObjPtr)); }
    public Collider GetCollider() { return new Collider(cGetColliderComponent(ObjPtr)); }
    public Light GetLight() { return new Light(cGetLightComponent(ObjPtr)); }
    public Model GetModel() { return new Model(cGetModelComponent(ObjPtr)); }
    public RigidBody GetRigidBody() { return new RigidBody(cGetRigidBodyComponent(ObjPtr)); }
    public Transform GetTransform() { return new Transform(cGetTransformComponent(ObjPtr)); }
    public Script GetScript() { return new Script(cGetScriptComponent(ObjPtr)); }
    public Camera AddCamera() { return new Camera(cAddCameraComponent(ObjPtr)); }
    public Collider AddCollider() { return new Collider(cAddColliderComponent(ObjPtr)); }
    public Light AddLight() { return new Light(cAddLightComponent(ObjPtr)); }
    public Model AddModel() { return new Model(cAddModelComponent(ObjPtr)); }
    public RigidBody AddRigidBody() { return new RigidBody(cAddRigidBodyComponent(ObjPtr)); }
    public Script AddScript() { return new Script(cAddScriptComponent(ObjPtr)); }
    public void RemoveCamera() { cRemoveCameraComponent(ObjPtr); }
    public void RemoveCollider() { cRemoveColliderComponent(ObjPtr); }
    public void RemoveLight() { cRemoveLightComponent(ObjPtr); }
    public void RemoveModel() { cRemoveModelComponent(ObjPtr); }
    public void RemoveRigidBody() { cRemoveRigidBodyComponent(ObjPtr); }
    public void RemoveScript() { cRemoveScriptComponent(ObjPtr); }


    [DllImport("IrisEngine")]
    private static extern void cSetActive(IntPtr _obj, bool _activate);

    [DllImport("IrisEngine")]
    private static extern void cDestroy(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern int cGetID(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern void cSetName(IntPtr _obj, IntPtr _name);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetName(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetCameraComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetColliderComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetLightComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetModelComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetRigidBodyComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetTransformComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetScriptComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cAddCameraComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cAddColliderComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cAddLightComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cAddModelComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cAddRigidBodyComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern IntPtr cAddScriptComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern void cRemoveCameraComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern void cRemoveColliderComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern void cRemoveLightComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern void cRemoveModelComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern void cRemoveRigidBodyComponent(IntPtr _obj);

    [DllImport("IrisEngine")]
    private static extern void cRemoveScriptComponent(IntPtr _obj);
}