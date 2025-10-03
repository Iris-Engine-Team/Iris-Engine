using System;
using System.Runtime.InteropServices;

public enum LightType : int
{
    DIRECTIONAL_LIGHT = 0,
    POINT_LIGHT = 1,
    SPOT_LIGHT = 2
}

public class Light : IComponent
{
    public Light(IntPtr _lightPtr) : base(_lightPtr) { }

    public void SetColor(Vector3 _color)
    {
        GCHandle handle = GCHandle.Alloc(_color.GetData(), GCHandleType.Pinned);
        cSetColor(ComponentPtr, handle.AddrOfPinnedObject());
        handle.Free();
    }
    public Vector3 GetColor() { return InfoReader.ToVector3(cGetColor(ComponentPtr)); }
    public void SetIntensity(float _intensity) { cSetIntensity(ComponentPtr, _intensity); }
    public float GetIntensity() { return cGetIntensity(ComponentPtr); }
    public void SetNear(float _near) { cSetNear(ComponentPtr, _near); }
    public float GetNear() { return cGetNear(ComponentPtr); }
    public void SetFar(float _far) { cSetFar(ComponentPtr, _far); }
    public float GetFar() { return cGetFar(ComponentPtr); }
    public void SetCutOff(float _cutOff) { cSetCutOff(ComponentPtr, _cutOff); }
    public float GetCutOff() { return cGetCutOff(ComponentPtr); }
    public void SetOuterCutOff(float _outerCutOff) { cSetOuterCutOff(ComponentPtr, _outerCutOff); }
    public float GetOuterCutOff() { return cGetOuterCutOff(ComponentPtr); }
    public void SetLightType(LightType _type) { cSetLightType(ComponentPtr, (int)_type); }
    public LightType GetLightType() { return (LightType)cGetLightType(ComponentPtr); }


    [DllImport("IrisEngine")]
    private static extern void cSetColor(IntPtr _light, IntPtr _color);

    [DllImport("IrisEngine")]
    private static extern IntPtr cGetColor(IntPtr _light);
    
    [DllImport("IrisEngine")]
    private static extern void cSetIntensity(IntPtr _light, float _intensity);

    [DllImport("IrisEngine")]
    private static extern float cGetIntensity(IntPtr _light);
    
    [DllImport("IrisEngine")]
    private static extern void cSetNear(IntPtr _light, float _near);
    
    [DllImport("IrisEngine")]
    private static extern float cGetNear(IntPtr _light);
    
    [DllImport("IrisEngine")]
    private static extern void cSetFar(IntPtr _light, float _far);
    
    [DllImport("IrisEngine")]
    private static extern float cGetFar(IntPtr _light);
    
    [DllImport("IrisEngine")]
    private static extern void cSetCutOff(IntPtr _light, float _cutOff);

    [DllImport("IrisEngine")]
    private static extern float cGetCutOff(IntPtr _light);
    
    [DllImport("IrisEngine")]
    private static extern void cSetOuterCutOff(IntPtr _light, float _outerCutOff);

    [DllImport("IrisEngine")]
    private static extern float cGetOuterCutOff(IntPtr _light);
        
    [DllImport("IrisEngine")]
    private static extern void cSetLightType(IntPtr _light, int _type);
    
    [DllImport("IrisEngine")]
    private static extern int cGetLightType(IntPtr _light);
}