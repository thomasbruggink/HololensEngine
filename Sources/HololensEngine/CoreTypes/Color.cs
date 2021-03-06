﻿namespace CoreTypes
{
    /// <summary>
    /// Helper class for rending with colors
    /// </summary>
    public sealed class Color
    {
        public float R { get; set; }
        public float G { get; set; }
        public float B { get; set; }
        public float A { get; set; }

        public Color()
        {
            R = 0;
            G = 0;
            B = 0;
            A = 1;
        }

        public Color(float r, float g, float b)
        {
            R = r;
            G = g;
            B = b;
            A = 1;
        }

        public Color(float r, float g, float b, float a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public Color(Color color, float a)
        {
            R = color.R;
            G = color.G;
            B = color.B;
            A = a;
        }

        public static Color Red()
        {
            return new Color(255.0f / 255.0f, 0, 0, 255.0f / 255.0f);
        }

        public static Color DrawRed()
        {
            return new Color(128.0f / 255.0f, 0, 0, 255.0f / 255.0f);
        }

        public static Color White()
        {
            return new Color(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);
        }

        public static Color Creme()
        {
            return new Color(237.0f / 255.0f, 226.0f / 255.0f, 194.0f / 255.0f, 255.0f / 255.0f);
        }

        public static Color DarkBrown()
        {
            return new Color(89.0f / 255.0f, 45.0f / 255.0f, 0, 255.0f / 255.0f);
        }

        public static Color Brown()
        {
            return new Color(128.0f / 255.0f, 64.0f / 255.0f, 0, 255.0f / 255.0f);
        }

        public static Color Yellow()
        {
            return new Color(255.0f / 255.0f, 242.0f / 255.0f, 0, 255.0f / 255.0f);
        }

        public static Color Orange()
        {
            return new Color(255.0f / 255.0f, 128.0f / 255.0f, 64.0f / 255.0f, 255.0f / 255.0f);
        }

        public static Color Transparent()
        {
            return new Color(0, 0, 0, 0);
        }
    }
}
