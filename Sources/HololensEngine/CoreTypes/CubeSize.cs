namespace CoreTypes
{
    /// <summary>
    /// The size of the cube
    /// </summary>
    public sealed class CubeSize
    {
        public float Width { get; set; }
        public float Height { get; set; }
        public float Depth { get; set; }

        public CubeSize(float width, float height, float depth)
        {
            Width = width;
            Height = height;
            Depth = depth;
        }
    }
}
