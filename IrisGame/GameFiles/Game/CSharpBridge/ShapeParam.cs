namespace CSharpBridge
{
    enum PhysicShape
    {
        IE_NO_SHAPE = -1,
        IE_SHAPE_BOX = 0,
        IE_SHAPE_SPHERE = 1,
        IE_SHAPE_CAPSULE = 2,
    };

    class ShapeParam
    {
        public void InitBoxInfo(Vector3 _size)
        {
            m_ShapeType = PhysicShape.IE_SHAPE_BOX;
            m_CubeSize = _size;
        }

        public void InitSphereInfo(float _radius)
        {
            m_ShapeType = PhysicShape.IE_SHAPE_SPHERE;
            m_SphereRadius = _radius;
        }

        public void InitCapsuleInfo(float _radius, float _height)
        {
            m_ShapeType = PhysicShape.IE_SHAPE_CAPSULE;
            m_CapsuleRadius = _radius;
            m_CapsuleHeight = _height;
        }

        public void SetShapeType(PhysicShape _shape) { m_ShapeType = _shape; }
        public PhysicShape GetShapeType() { return m_ShapeType; }

        public float GetSphereRadius() { return m_SphereRadius; }

        public Vector3 GetBoxSize() { return m_CubeSize; }

        public float GetCapsuleRadius() { return m_CapsuleRadius; }
        public float GetCapsuleHeight() { return m_CapsuleHeight; }




        private PhysicShape m_ShapeType = PhysicShape.IE_NO_SHAPE;

        //~~~~~~~~~~~~~~~~~~//
        //	Sphere Params	//
        //~~~~~~~~~~~~~~~~~~//
        private float m_SphereRadius = 1f;

        //~~~~~~~~~~~~~~~~~~//
        //    Cube Params	//
        //~~~~~~~~~~~~~~~~~~//
        private Vector3 m_CubeSize = Vector3.s_One;

        //~~~~~~~~~~~~~~~~~~//
        //  Capsule Params	//
        //~~~~~~~~~~~~~~~~~~//
        private float m_CapsuleRadius = 1f;
        private float m_CapsuleHeight = 1f;
    };
}