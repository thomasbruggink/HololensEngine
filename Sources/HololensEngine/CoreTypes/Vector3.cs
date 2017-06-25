namespace CoreTypes
{
    /// <summary>
    /// Helper class to move in a space
    /// </summary>
    public sealed class Vector3
    {
        public float X { get; set; }
        public float Y { get; set; }
        public float Z { get; set; }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3 Plus(Vector3 rhs)
        {
            return new Vector3(X + rhs.X, Y + rhs.Y, Z + rhs.Z);
        }
    }
}
