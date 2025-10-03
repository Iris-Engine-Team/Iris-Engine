public class Door : Script
    {
        public Door(IntPtr _templatetPtr) : base(_templatetPtr) { }

    private IrisObject trident;
    private IrisObject light;
    bool moving = false;
    float timer = 0f;
    bool Opened = false;

    public override void Awake()
        {

        }

        public override void Start()
        {
            trident = Scene.GetObjectByName("Trident");
            light = Scene.GetObjectByName("SpotLightShiva");
        }
        
        public void CheckTrident()
        {
        if (Opened == false)
        {

            Transform t = trident.GetTransform();
            Vector3 rota = t.GetEuler();

            if ((rota[0] == 0f || rota[0] == 360f) && (rota[1] == 270f || rota[1] == 90f))
            {
                moving = true;
                Opened = true;

                light.GetLight().SetColor(new Vector3(0f, 1f, 0f));
            }
        }
        }

        public override void Update()
        {
            if(moving)
            {
            timer += (float)Time.GetDeltaTime()*0.5f;
            if(timer >= 1f)
            {
                moving = false;
                timer = 1f;
            }

            Transform t = GetObject().GetTransform();
            Vector3 pos = t.GetPosition();

            t.SetPosition(new Vector3(pos[0], 2.3f * (1f - timer) + -7.8f * timer, pos[2]));
            }

            CheckTrident();
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
