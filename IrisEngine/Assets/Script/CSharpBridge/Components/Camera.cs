using System;
using System.Runtime.InteropServices;

public class Camera : IComponent
{
    public Camera(IntPtr _cameraPtr): base(_cameraPtr) { }

    public Vector3 GetCameraForward() { return InfoReader.ToVector3(cGetCameraForward(ComponentPtr)); }
    public void ActivateOrtho(bool _activate) { cActivateOrthoMode(ComponentPtr, _activate); }

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetCameraForward(IntPtr _camera);
    
    [DllImport("IrisEngine")]
    private static extern void cActivateOrthoMode(IntPtr _camera, bool _activate);
}