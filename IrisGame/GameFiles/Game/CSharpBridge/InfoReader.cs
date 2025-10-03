using System;
using System.Runtime.InteropServices;

public class InfoReader
{
    public static string ToString(IntPtr _string)
    {
        string  myString = Marshal.PtrToStringUTF8(_string);
        return myString is null ? "" : myString;
    }

    public static Vector3 ToVector3(IntPtr _vector)
    {
        float[] vector = new float[3];
        Marshal.Copy(_vector, vector, 0, 3);
        return new Vector3(vector[0], vector[1], vector[2]);
    }

    public static Quaternion ToQuaternion(IntPtr _quaternion)
    {
        float[] quaternion = new float[4];
        Marshal.Copy(_quaternion, quaternion, 0, 4);
        return new Quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
    }
}