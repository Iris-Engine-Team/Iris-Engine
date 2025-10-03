public class ButtonRotate : Script
{
    public ButtonRotate(IntPtr _templatetPtr) : base(_templatetPtr) { }

    private IrisObject trident;

    public override void Awake()
    {
     
    }

    public override void Start()
    {
        trident = Scene.GetObjectByName("Trident");
    }

    public override void Update()
    {
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
        if (_rigidBody.GetObject().GetName() != "Player")
            return;

        Transform t = trident.GetTransform();
        Vector3 rota = t.GetEuler();
        if (GetObject().GetName() == "ButtonX")
            t.SetEuler(new Vector3(rota[0] + 30f, rota[1], rota[2]));
        else
            t.SetEuler(new Vector3(rota[0], rota[1] + 30f, rota[2]));
    }

    public override void OnTriggerStay(RigidBody _rigidBody)
    {
        
    }

    public override void OnTriggerExit(RigidBody _rigidBody)
    {
        
    }
}
