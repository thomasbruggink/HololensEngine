namespace CoreTypes
{
    /// <summary>
    /// Helper class for rending with colors
    /// </summary>
    public sealed class Color
    {
        public int R { get; set; }
        public int G { get; set; }
        public int B { get; set; }
        public int A { get; set; }

        public Color()
        {
            R = 0;
            G = 0;
            B = 0;
            A = 0;
        }

        public Color(int r, int g, int b)
        {
            R = r;
            G = g;
            B = b;
            A = 0;
        }

        public Color(int r, int g, int b, int a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public static Color Red()
        {
            return new Color(255, 0, 0, 0);
        }

        public static Color Transparent()
        {
            return new Color(0, 0, 0, 255);
        }
    }
}
