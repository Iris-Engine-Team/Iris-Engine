using System;
using System.Reflection;
using System.Runtime.Loader;
using System.Collections.Generic;
using System.Runtime.InteropServices;

public static class ScriptFactory
{
    [UnmanagedCallersOnly]
    public static IntPtr CreateScriptInstance(IntPtr _scriptHandle, IntPtr _typeNamePtr)
    {
        string typeName = InfoReader.ToString(_typeNamePtr);
        Type type = Type.GetType(typeName);

        if (type == null || !typeof(Script).IsAssignableFrom(type))
        {
            return IntPtr.Zero;
        }

        object[] args = { _scriptHandle };
        Script script = (Script)Activator.CreateInstance(type, args)!;
        IntPtr handle = (IntPtr)GCHandle.Alloc(script);
        
        return handle;
    }

    [UnmanagedCallersOnly]
    public static void DestroyScriptInstance(IntPtr _scriptPtr)
    {
        GCHandle handle = GCHandle.FromIntPtr(_scriptPtr);
        handle.Free();
    }

    [UnmanagedCallersOnly]
    public static void CallAwake(IntPtr _scriptPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.Awake();
    }

    [UnmanagedCallersOnly]
    public static void CallStart(IntPtr _scriptPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.Start();
    }

    [UnmanagedCallersOnly]
    public static void CallUpdate(IntPtr _scriptPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.Update();
    }

    [UnmanagedCallersOnly]
    public static void CallFixedUpdate(IntPtr _scriptPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.FixedUpdate();
    }

    [UnmanagedCallersOnly]
    public static void CallCollisionEnter(IntPtr _scriptPtr, IntPtr _rigidBodyPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.OnCollisionEnter(new RigidBody(_rigidBodyPtr));
    }

    [UnmanagedCallersOnly]
    public static void CallCollisionStay(IntPtr _scriptPtr, IntPtr _rigidBodyPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.OnCollisionStay(new RigidBody(_rigidBodyPtr));
    }

    [UnmanagedCallersOnly]
    public static void CallCollisionExit(IntPtr _scriptPtr, IntPtr _rigidBodyPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.OnCollisionExit(new RigidBody(_rigidBodyPtr));
    }

    [UnmanagedCallersOnly]
    public static void CallTriggerEnter(IntPtr _scriptPtr, IntPtr _rigidBodyPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.OnTriggerEnter(new RigidBody(_rigidBodyPtr));
    }

    [UnmanagedCallersOnly]
    public static void CallTriggerStay(IntPtr _scriptPtr, IntPtr _rigidBodyPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.OnTriggerStay(new RigidBody(_rigidBodyPtr));
    }

    [UnmanagedCallersOnly]
    public static void CallTriggerExit(IntPtr _scriptPtr, IntPtr _rigidBodyPtr)
    {
        Script script = (Script)((GCHandle)_scriptPtr).Target!;
        script.OnTriggerExit(new RigidBody(_rigidBodyPtr));
    }
}