using System;
using System.Runtime.InteropServices;

public static class Time
{
    private static IntPtr TimePtr = IntPtr.Zero;

    [UnmanagedCallersOnly]
    public static void Init(IntPtr _ptr)
    {
        TimePtr = _ptr;
    }

    public static double GetDeltaTime() { return cGetDeltaTime(TimePtr); }
    public static void SetFixedDeltaTime(double _fixedDeltaTime) { cSetFixedDeltaTime(TimePtr, _fixedDeltaTime); }
    public static double GetFixedDeltaTime() { return cGetFixedDeltaTime(TimePtr); }

    [DllImport("IrisEngine")]
    private static extern double cGetDeltaTime(IntPtr _timePtr);

    [DllImport("IrisEngine")]
    private static extern void cSetFixedDeltaTime(IntPtr _timePtr, double _fixedDeltaTime);

    [DllImport("IrisEngine")]
    private static extern double cGetFixedDeltaTime(IntPtr _timePtr);
}