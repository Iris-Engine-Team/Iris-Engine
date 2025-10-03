using System.Numerics;


public class Quaternion
{
    public Quaternion(float _number)
    {
        m_W = m_X = m_Y = m_Z = _number;
    }

    public Quaternion(float _w, float _x, float _y, float _z)
    {
        m_W = _w;
        m_X = _x;
        m_Y = _y;
        m_Z = _z;
    }

    public Quaternion(Quaternion _quaternion)
    {
        m_W = _quaternion.W;
        m_X = _quaternion.X;
        m_Y = _quaternion.Y;
        m_Z = _quaternion.Z;
    }

    public Quaternion(float _angle, Vector3 _axis)
    {
        if(_axis.IsNull())
        {
            m_W = float.NaN;
            m_X = float.NaN;
            m_Y = float.NaN;
            m_Z = float.NaN;
        }
        else
        {
            _axis = _axis.Normalize();

            float halfAngle = _angle * 0.5f;
            float sinHalfAngle = MathF.Sin(halfAngle);

            m_W = MathF.Cos(halfAngle);
            m_X = _axis[0] * sinHalfAngle;
            m_Y = _axis[1] * sinHalfAngle;
            m_Z = _axis[2] * sinHalfAngle;
        }
    }

    public Quaternion(float _angleX, float _angleY, float _angleZ)
    {
        float halfX = _angleX * 0.5f;
        float halfY = _angleY * 0.5f;
        float halfZ = _angleZ * 0.5f;

        float cosX = MathF.Cos(halfX);
        float sinX = MathF.Sin(halfX);
        float cosY = MathF.Cos(halfY);
        float sinY =  MathF.Sin(halfY);
        float cosZ = MathF.Cos(halfZ);
        float sinZ =  MathF.Sin(halfZ);

        m_W = cosX * cosY * cosZ - sinX * sinY * sinZ;
        m_X = sinX * cosY * cosZ + cosX * sinY * sinZ;
        m_Y = cosX * sinY * cosZ - sinX * cosY * sinZ;
        m_Z = cosX * cosY * sinZ + sinX * sinY * cosZ;
    }

    public Quaternion(Vector3 _angles)
    {
        float halfX = _angles[0] * 0.5f;
        float halfY = _angles[1] * 0.5f;
        float halfZ = _angles[2] * 0.5f;

        float cosX = MathF.Cos(halfX);
        float sinX = MathF.Sin(halfX);
        float cosY = MathF.Cos(halfY);
        float sinY = MathF.Sin(halfY);
        float cosZ = MathF.Cos(halfZ);
        float sinZ = MathF.Sin(halfZ);

        m_W = cosX * cosY * cosZ - sinX * sinY * sinZ;
        m_X = sinX * cosY * cosZ + cosX * sinY * sinZ;
        m_Y = cosX * sinY * cosZ - sinX * cosY * sinZ;
        m_Z = cosX * cosY * sinZ + sinX * sinY * cosZ;
    }

    public float GetReal()
    {
        return m_W;
    }

    public Vector3 GetImaginary()
    {
        return new Vector3(m_X, m_Y, m_Z);
    }

    public Vector3 GetRotationAxis()
    {
        float sin_half_angle = MathF.Sqrt(1f - m_W * m_W);

        if (sin_half_angle < MathsTools.EPSILON)
            return new Vector3(0f, 0f, 0f);
        else
            return new Vector3(m_X / sin_half_angle, m_Y / sin_half_angle, m_Z / sin_half_angle);
           
    }

    public float GetRotation()
    {
        return 2f * MathF.Acos(m_W);
    }

    public float[] GetData()
    {
        float[] data = new float[4];
        data[0] = m_W;
        data[1] = m_X;
        data[2] = m_Y;
        data[3] = m_Z;
        return data;
    }

    public bool IsNull()
    {
        double epsi = MathsTools.EPSILON;
        return MathF.Abs(m_W) < epsi && MathF.Abs(m_X) < epsi && MathF.Abs(m_Y) < epsi && MathF.Abs(m_Z) < epsi;
    }

    public bool IsUnit()
    {
        return MathF.Abs(Norm() - 1f) < MathsTools.EPSILON;
    }

    bool IsPureReal()
    {
        double epsi = MathsTools.EPSILON;
        return MathF.Abs(m_W) >= epsi && MathF.Abs(m_X) < epsi && MathF.Abs(m_Y) < epsi && MathF.Abs(m_Z) < epsi;
    }

    bool IsPureImaginary()
    {
        double epsi = MathsTools.EPSILON;
        return MathF.Abs(m_W) < epsi && (MathF.Abs(m_X) >= epsi || MathF.Abs(m_Y) >= epsi || MathF.Abs(m_Z) >= epsi);
    }

    Quaternion Opposite()
    {
        return new Quaternion(-m_W, -m_X, -m_Y, -m_Z);
    }

    Quaternion Conjugation()
    {
        return new Quaternion(m_W, -m_X, -m_Y, -m_Z);
    }

    Quaternion Inverse()
    {
        float squaredNorm = SquaredNorm();

        if (squaredNorm < MathsTools.EPSILON)
        {
            return new Quaternion(float.NaN);
        }
        return new Quaternion(m_W / squaredNorm, -m_X / squaredNorm, -m_Y / squaredNorm, -m_Z / squaredNorm);
    }

    Quaternion Normalize()
    {
        float myNorm = Norm();

        if (myNorm >= MathsTools.EPSILON)
        {
            float w = m_W / myNorm;
            float x = m_X / myNorm;
            float y = m_Y / myNorm;
            float z = m_Z / myNorm;

            return new Quaternion(w, x, y, z);
        }

        return new Quaternion(float.NaN);
    }

    float SquaredNorm()
    {
        return m_W * m_W + m_X * m_X + m_Y * m_Y + m_Z * m_Z;
    }

    float Norm()
    {
        return MathF.Sqrt(m_W * m_W + m_X * m_X + m_Y * m_Y + m_Z * m_Z);
    }

    float DotProduct(Quaternion _quaternion)
    {
        return m_W * _quaternion.m_W + m_X * _quaternion.m_X + m_Y * _quaternion.m_Y + m_Z * _quaternion.m_Z;
    }

    Vector3 Rotate(Vector3 _vector, Quaternion _quaternion)
    {
        return (_quaternion * new Quaternion(0f, _vector[0], _vector[1], _vector[2]) * _quaternion.Conjugation()).GetImaginary();
    }

    Vector3 RotateInverse(Vector3 _vector, Quaternion _quaternion)
    {
        return (_quaternion.Conjugation() * new Quaternion(0f, _vector[0], _vector[1], _vector[2]) * _quaternion).GetImaginary();
    }

    Vector3 ToEulerAngle()
    {
        Quaternion quaternion = Normalize();
        float pitch, yaw, roll = 0;

        float y = 2f * (quaternion.m_Y * quaternion.m_Z + quaternion.m_W * quaternion.m_X);
        float x = quaternion.m_W * quaternion.m_W - quaternion.m_X * quaternion.m_X - quaternion.m_Y * quaternion.m_Y + quaternion.m_Z * quaternion.m_Z;

        if (MathF.Abs(x) < MathsTools.EPSILON && MathF.Abs(y) < MathsTools.EPSILON)
            pitch = 2f * MathF.Atan2(quaternion.m_X, quaternion.m_W);
        else
            pitch = MathF.Atan2(y, x);

        yaw = -2f * (quaternion.m_X * quaternion.m_Z - quaternion.m_W * quaternion.m_Y);

        if (yaw < -1f)
            yaw = -1f;
        else if (yaw > 1f)
            yaw = 1f;
        yaw = MathF.Asin(yaw);

        y = 2f * (quaternion.m_X * quaternion.m_Y + quaternion.m_W * quaternion.m_Z);
        x = quaternion.m_W * quaternion.m_W + quaternion.m_X * quaternion.m_X - quaternion.m_Y * quaternion.m_Y - quaternion.m_Z * quaternion.m_Z;

        if (MathF.Abs(x) >= MathsTools.EPSILON && MathF.Abs(y) >= MathsTools.EPSILON) //avoid atan2(0,0) - handle singularity - Matiis
            roll = MathF.Atan2(y, x);

        return new Vector3(pitch, yaw, roll);
    }

    public static Quaternion operator +(Quaternion _quaternion1, Quaternion _quaternion2)
    {
        return new Quaternion(_quaternion1.W + _quaternion2.W, _quaternion1.X + _quaternion2.X, _quaternion1.Y + _quaternion2.Y, _quaternion1.Z + _quaternion2.Z);
    }
    public static Quaternion operator +(Quaternion _quaternion, float _scalar)
    {
        return new Quaternion(_quaternion.W + _scalar, _quaternion.X + _scalar, _quaternion.Y + _scalar, _quaternion.Z + _scalar);
    }
    public static Quaternion operator -(Quaternion _quaternion1, Quaternion _quaternion2)
    {
        return new Quaternion(_quaternion1.W - _quaternion2.W, _quaternion1.X - _quaternion2.X, _quaternion1.Y - _quaternion2.Y, _quaternion1.Z - _quaternion2.Z);
    }
    public static Quaternion operator -(Quaternion _quaternion1, float _scalar)
    {
        return new Quaternion(_quaternion1.W - _scalar, _quaternion1.X - _scalar, _quaternion1.Y - _scalar, _quaternion1.Z - _scalar);
    }
    public static Quaternion operator *(Quaternion _quaternion1, Quaternion _quaternion2)
    {
        return new Quaternion(
            _quaternion1.W * _quaternion2.W - _quaternion1.X * _quaternion2.X - _quaternion1.Y * _quaternion2.Y - _quaternion1.Z * _quaternion2.Z,
            _quaternion1.X * _quaternion2.W + _quaternion1.W * _quaternion2.X - _quaternion1.Z * _quaternion2.Y + _quaternion1.Y * _quaternion2.Z,
            _quaternion1.Y * _quaternion2.W + _quaternion1.Z * _quaternion2.X + _quaternion1.W * _quaternion2.Y - _quaternion1.X * _quaternion2.Z,
            _quaternion1.Z * _quaternion2.W - _quaternion1.Y * _quaternion2.X + _quaternion1.X * _quaternion2.Y + _quaternion1.W * _quaternion2.Z
        );
    }

    public static Quaternion operator *(Quaternion _quaternion, float _scalar)
    {
        return new Quaternion(_quaternion.W * _scalar, _quaternion.X * _scalar, _quaternion.Y * _scalar, _quaternion.Z * _scalar);
    }

    public static Quaternion operator /(Quaternion _quaternion1, Quaternion _quaternion2)
    {
        return new Quaternion(_quaternion1 * _quaternion2.Inverse());
    }

    public static Quaternion operator /(Quaternion _quaternion, float _scalar)
    {
        if (MathF.Abs(_scalar) >= MathsTools.EPSILON)
            return new Quaternion(_quaternion.W / _scalar, _quaternion.X / _scalar, _quaternion.Y / _scalar, _quaternion.Z / _scalar);

        return new Quaternion(float.NaN);
    }

    public override string ToString()
    {
        return "{ " + m_W.ToString() + " " + m_X.ToString() + " " + m_Y.ToString() + " " + m_Z.ToString() + " }";
    }
    public override int GetHashCode()
    {
        return this.ToString().GetHashCode();
    }

    public override bool Equals(object o)
    {
        if (o is Quaternion q)
            return q.W == m_W && q.X == m_X && q.Y == m_Y && q.Z == m_Z;

        return false;
    }

    public static bool operator ==(Quaternion _quaternion1, Quaternion _quaternion2)
    {
        return MathF.Abs(_quaternion1.W - _quaternion2.W) < MathsTools.EPSILON && MathF.Abs(_quaternion1.X - _quaternion2.X) < MathsTools.EPSILON 
            && MathF.Abs(_quaternion1.Y - _quaternion2.Y) < MathsTools.EPSILON && MathF.Abs(_quaternion1.Z - _quaternion2.Z) < MathsTools.EPSILON;
    }

    public static bool operator !=(Quaternion _quaternion1, Quaternion _quaternion2)
    {
        return MathF.Abs(_quaternion1.W - _quaternion2.W) >= MathsTools.EPSILON || MathF.Abs(_quaternion1.X - _quaternion2.X) >= MathsTools.EPSILON 
            || MathF.Abs(_quaternion1.Y - _quaternion2.Y) >= MathsTools.EPSILON || MathF.Abs(_quaternion1.Z - _quaternion2.Z) >= MathsTools.EPSILON;
    }

    public float this[int key]
    {
        get
        {
            if (key == 0)
                return m_W;
            else if (key == 1)
                return m_X;
            else if (key == 2)
                return m_Y;
            else if (key == 3)
                return m_Z;

            return float.NaN;
        }
        set
        {
            if (key == 0)
                m_W = value;
            else if (key == 1)
                m_X = value;
            else if (key == 2)
                m_Y = value;
            else if (key == 3)
                m_Z = value;
        }
    }

    public float W { set { m_W = value; } get { return m_W; } }
    public float X { set { m_X = value; } get { return m_X; } }
    public float Y { set { m_Y = value; } get { return m_Y; } }
    public float Z { set { m_Z = value; } get { return m_Z; } }

    private float m_W = 0f;
    private float m_X = 0f;
    private float m_Y = 0f;
    private float m_Z = 0f;

    public static Quaternion s_Identity = new Quaternion(1f, 0f, 0f, 0f);
}