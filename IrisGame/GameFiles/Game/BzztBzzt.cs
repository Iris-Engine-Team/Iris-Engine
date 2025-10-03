using System.Numerics;

public class BzztBzzt : Script
{
    public BzztBzzt(IntPtr _templatetPtr) : base(_templatetPtr) { }

    private Light compLight;
    float counting = 0f;

    public override void Awake()
    {

    }

    public override void Start()
    {
        compLight = GetObject().GetLight();
    }

    public override void Update()
    {
        counting += (float)Time.GetDeltaTime() * 0.2f;
        compLight.SetColor(new Vector3(MathF.Sin(counting), MathF.Cos(counting + 3.14f), MathF.Sin(counting + 3.14f * 0.5f)));
    }

    public override void FixedUpdate()
    {

    }

    public override void Destroy()
    {

    }

    public override void OnCollisionEnter(RigidBody _rigidBody)
    {

    }

    public override void OnCollisionStay(RigidBody _rigidBody)
    {

    }

    public override void OnCollisionExit(RigidBody _rigidBody)
    {

    }

    public override void OnTriggerEnter(RigidBody _rigidBody)
    {

    }

    public override void OnTriggerStay(RigidBody _rigidBody)
    {

    }

    public override void OnTriggerExit(RigidBody _rigidBody)
    {

    }
}
