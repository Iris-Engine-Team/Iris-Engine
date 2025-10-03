using System;
using System.Runtime.InteropServices;

public class Script : IComponent
{
    public Script(IntPtr _scriptPtr) : base(_scriptPtr) { }

    public virtual void Awake() { }

    public virtual void Start() { }

    public virtual void Update() { }

    public virtual void FixedUpdate() { }

    public virtual void Destroy() { }

    public virtual void OnCollisionEnter(RigidBody _rigidBody) { }

    public virtual void OnCollisionStay(RigidBody _rigidBody) { }

    public virtual void OnCollisionExit(RigidBody _rigidBody) { }

    public virtual void OnTriggerEnter(RigidBody _rigidBody) { }

    public virtual void OnTriggerStay(RigidBody _rigidBody) { }

    public virtual void OnTriggerExit(RigidBody _rigidBody) { }
}