public class Vector3
{
    public Vector3(float _number = 0f)
    {
        m_X = m_Y = m_Z = _number;
    }

    public Vector3(float _x, float _y, float _z)
    {
        m_X = _x;
        m_Y = _y;
        m_Z = _z;
    }

    public Vector3(Vector3 _vector)
    {
        m_X = _vector.m_X;
        m_Y = _vector.m_Y;
        m_Z = _vector.m_Z;
    }

    public Vector3(Vector3 _vector1, Vector3 _vector2)
    {
        m_X = _vector2.m_X - _vector1.m_X;
        m_Y = _vector2.m_Y - _vector1.m_Y;
        m_Z = _vector2.m_Z - _vector1.m_Z;
    }

    public float[] GetData()
    {
        float[] data = new float[3];
        data[0] = m_X;
        data[1] = m_Y;
        data[2] = m_Z;
        return data;
    }

    public bool IsNull()
    {
        double epsi = MathsTools.EPSILON;
        return MathF.Abs(m_X) < epsi && MathF.Abs(m_Y) < epsi && MathF.Abs(m_Z) < epsi;
    }


    public bool IsUnit()
    {
        return MathF.Abs(Norm() - 1f) < MathsTools.EPSILON;
    }

    public Vector3 Opposite()
    {
        return new Vector3(-m_X, -m_Y, -m_Z);
    }

    public Vector3 Inverse()
    {
        if (IsNull())
            return new Vector3(float.NaN);

        float x = m_X;
        if (x == 0)
            x = float.NaN;
        else
            x = 1f / x;


        float y = m_Y;
        if (y == 0)
            y = float.NaN;
        else
            y = 1f / y;


        float z = m_Z;
        if (z == 0)
            z = float.NaN;
        else
            z = 1f / z;

        return new Vector3(x, y, z);
    }

    public Vector3 MidPoint(Vector3 _vector)
    {
        return new Vector3((m_X + _vector.m_X) * 0.5f
            , (m_Y + _vector.m_Y) * 0.5f
            , (m_Z + _vector.m_Z) * 0.5f);
    }

    public Vector3 Normalize()
    {
        float myNorm = Norm();

        if (myNorm >= MathsTools.EPSILON)
        {
            float _x = m_X / myNorm;
            float _y = m_Y / myNorm;
            float _z = m_Z / myNorm;

            return new Vector3(_x, _y, _z);
        }

        return new Vector3(float.NaN);
    }

    public float Distance(Vector3 _vector)
    {
        return MathF.Sqrt((m_X - _vector.m_X) * (m_X - _vector.m_X) 
            + (m_Y - _vector.m_Y) * (m_Y - _vector.m_Y) 
            + (m_Z - _vector.m_Z) * (m_Z - _vector.m_Z));
    }

    public float SquaredNorm()
    {
        return m_X * m_X + m_Y * m_Y + m_Z * m_Z;
    }

    public float Norm()
    {
        return MathF.Sqrt(m_X * m_X + m_Y * m_Y + m_Z * m_Z);
    }   

    public float DotProduct(Vector3 _vector)
    {
        return m_X * _vector.m_X + m_Y * _vector.m_Y + m_Z * _vector.m_Z;
    }

    public Vector3 CrossProduct(Vector3 _vector)
    {
        return new Vector3(m_Y * _vector.m_Z - m_Z * _vector.m_Y, m_Z * _vector.m_X - m_X * _vector.m_Z, m_X * _vector.m_Y - m_Y * _vector.m_X);
    }

    public float GetAngle(Vector3 _vector)
    {
        if (Norm() * _vector.Norm() >= MathsTools.EPSILON)
        {
            float angle = DotProduct(_vector);

            if (angle > 1f)
                angle = 1f;

            else if (angle < -1f)
                angle = -1f;

            return MathF.Acos(angle);
        }

        return float.NaN;
    }

    public Vector3 Translate(Vector3 _vector)
    {
        return new Vector3(m_X + _vector.m_X, m_Y + _vector.m_Y, m_Z + _vector.m_Z);
    }

    public Vector3 Scale(float _scaleValue, Vector3 _anchor)
    {
        return new Vector3(_anchor.m_X + (m_X - _anchor.m_X) * _scaleValue, _anchor.m_Y + (m_Y - _anchor.m_Y) * _scaleValue, _anchor.m_Z + (m_Z - _anchor.m_Z) * _scaleValue);
    }

    public Vector3 Rotate(float _angle, Vector3 _axis, Vector3 _anchor)
    {
        float s = MathF.Sin(_angle);
        float c = 1 - MathF.Cos(_angle);

        float x = m_X - _anchor.m_X;
        float y = m_Y - _anchor.m_Y;
        float z = m_Z - _anchor.m_Z;

        float _x = x * (1 + c * (_axis.m_X * _axis.m_X - 1)) + y * (-s * _axis.m_Z + c * _axis.m_X * _axis.m_Y) + z * (s * _axis.m_Y + c * _axis.m_X * _axis.m_Z);
        float _y = x * (s * _axis.m_Z + c * _axis.m_X * _axis.m_Y) + y * (1 + c * (_axis.m_Y * _axis.m_Y - 1)) + z * (-s * _axis.m_X + c * _axis.m_Y * _axis.m_Z);
        float _z = x * (-s * _axis.m_Y + c * _axis.m_X * _axis.m_Z) + y * (s * _axis.m_X + c * _axis.m_Y * _axis.m_Z) + z * (1 + c * (_axis.m_Z * _axis.m_Z - 1));

        return new Vector3(_x + _anchor.m_X, _y + _anchor.m_Y, _z + _anchor.m_Z);
    }

    public static Vector3 Lerp(Vector3 _vector1, Vector3 _vector2, float _t)
    {
        return _vector1 * (1.0f - _t) + _vector2 * _t;
    }

    public static Vector3 operator +(Vector3 _vector1, Vector3 _vector2)
    {
        return new Vector3(_vector1.X + _vector2.X, _vector1.Y + _vector2.Y, _vector1.Z + _vector2.Z);
    }

    public static Vector3 operator +(Vector3 _vector1, float _scalar)
    {
        return new Vector3(_vector1.X + _scalar, _vector1.Y + _scalar, _vector1.Z + _scalar);
    }

    public static Vector3 operator -(Vector3 _vector1, Vector3 _vector2)
    {
        return new Vector3(_vector1.X - _vector2.X, _vector1.Y - _vector2.Y, _vector1.Z - _vector2.Z);
    }

    public static Vector3 operator -(Vector3 _vector1, float _scalar)
    {
        return new Vector3(_vector1.X - _scalar, _vector1.Y - _scalar, _vector1.Z - _scalar);
    }

    public static Vector3 operator* (Vector3 _vector1, Vector3 _vector2)
    {
        return new Vector3(_vector1.X * _vector2.X, _vector1.Y * _vector2.Y, _vector1.Z * _vector2.Z);
    }

    public static Vector3 operator *(Vector3 _vector1, float _scalar)
    {
        return new Vector3(_vector1.X * _scalar, _vector1.Y * _scalar, _vector1.Z * _scalar);
    }

    public static Vector3 operator /(Vector3 _vector1, Vector3 _vector2)
    {
        return _vector1 * _vector2.Inverse();
    }
    public static Vector3 operator /(Vector3 _vector1, float _scalar)
    {
        if (MathF.Abs(_scalar) >= MathsTools.EPSILON)
            return new Vector3(_vector1.X / _scalar, _vector1.Y / _scalar, _vector1.Z / _scalar);

        return new Vector3(float.NaN);
    }

    public override int GetHashCode()
    {
        return this.ToString().GetHashCode();
    }

    public override bool Equals(object o)
    {
        if (o is Vector3 v)
            return v.X == m_X && v.Y == m_Y && v.Z == m_Z;

        return false;
    }

    public static bool operator == (Vector3 _vector1, Vector3 _vector2)
    {
        return MathF.Abs(_vector1.X - _vector2.X) < MathsTools.EPSILON && MathF.Abs(_vector1.Y - _vector2.Y) < MathsTools.EPSILON && MathF.Abs(_vector1.Z - _vector2.Z) < MathsTools.EPSILON;
    }

    public static bool operator !=(Vector3 _vector1, Vector3 _vector2)
    {
        return MathF.Abs(_vector1.X - _vector2.X) >= MathsTools.EPSILON || MathF.Abs(_vector1.Y - _vector2.Y) >= MathsTools.EPSILON || MathF.Abs(_vector1.Z - _vector2.Z) >= MathsTools.EPSILON;
    }

    public float this[int key]
    {
            
        get 
        {
            if (key == 0)
                return m_X;
            else if (key == 1)
                return m_Y;
            else if (key == 2)
                return m_Z;
                
            return float.NaN;
        }
        set 
        {
            if (key == 0)
                m_X = value;
            else if (key == 1)
                m_Y = value;
            else if (key == 2)
                m_Z = value;
        }
    }
    public override string ToString()
    {
        return "{ " + m_X.ToString() + " " + m_Y.ToString() + " " + m_Z.ToString() + " }";
    }

    public float X { set { m_X = value; } get { return m_X; } }
    public float Y { set { m_Y = value; } get { return m_Y; } }
    public float Z { set { m_Z = value; } get { return m_Z; } }

    private float m_X = 0f;
    private float m_Y = 0f;
    private float m_Z = 0f;

    public static Vector3 s_Zero = new Vector3(0f);
    public static Vector3 s_One = new Vector3(1f);
    public static Vector3 s_Up = new Vector3(0f, 1f, 0f);
    public static Vector3 s_Down = new Vector3(0f, -1f, 0f);
    public static Vector3 s_Forward = new Vector3(0f, 0f, 1f);
    public static Vector3 s_Back = new Vector3(0f, 0f, -1f);
    public static Vector3 s_Right = new Vector3(1f, 0f, 0f);
    public static Vector3 s_Left = new Vector3(-1f, 0f, 0f);
}