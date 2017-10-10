namespace CoreTypes
{
    /// <summary>
    /// Helper class to move in a plane
    /// </summary>
    public sealed class Vector2
    {
        public float X { get; set; }
        public float Y { get; set; }

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public Vector2 Plus(Vector2 rhs)
        {
            return new Vector2(X + rhs.X, Y + rhs.Y);
        }

        public static Vector2 Zero()
        {
            return new Vector2(0.0f, 0.0f);
        }
    }
}
