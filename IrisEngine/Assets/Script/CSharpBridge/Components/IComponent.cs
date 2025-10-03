using System;
using System.Runtime.InteropServices;

public class IComponent
{
	protected IntPtr ComponentPtr;

	public IComponent(IntPtr _cameraPtr)
	{
		ComponentPtr = _cameraPtr;
	}

	public void SetEnable(bool _activate) { cSetEnable(ComponentPtr, _activate); }
	public bool IsEnable() { return cIsEnable(ComponentPtr); }
	public IrisObject GetObject() { return new IrisObject(cGetObject(ComponentPtr)); }

	[DllImport("IrisEngine")]
	private static extern void cSetEnable(IntPtr _handle, bool _activate);

	[DllImport("IrisEngine")]
	private static extern bool cIsEnable(IntPtr _handle);

	[DllImport("IrisEngine", CallingConvention = CallingConvention.Cdecl)]
    private static extern IntPtr cGetObject(IntPtr _handle);
}