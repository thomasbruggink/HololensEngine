using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace CoreTypes
{
    public sealed class ObjReader
    {
        private readonly string _file;
        public IList<Vector3> Vertice { get; }
        public IList<Vector3> Normals { get; }
        public IList<ulong> Indices { get; }
        public CubeSize Size { get; set; }

        public int VertexCount { get { return Vertice.Count; } }
        public int NormalCount { get { return Normals.Count; } }
        public int IndexCount { get { return Indices.Count; } }

        public ObjReader(string file)
        {
            _file = file;
            Vertice = new List<Vector3>();
            Normals = new List<Vector3>();
            Indices = new List<ulong>();
        }

        public void Load()
        {
            var data = File.ReadAllLines(_file);
            var mostLeftX = float.MinValue;
            var mostRightX = float.MinValue;
            var mostLeftZ = float.MinValue;
            var mostRightZ = float.MinValue;
            var mostLeftY = float.MinValue;
            var mostRightY = float.MinValue;
            foreach (var line in data.Where(l => !l.StartsWith("#")))
            {
                if (line.StartsWith("vn"))
                {
                    var splitted = line.Split(' ');
                    var x = float.Parse(splitted[1]);
                    var y = float.Parse(splitted[2]);
                    var z = float.Parse(splitted[3]);
                    var w = 1.0f;
                    if (splitted.Count() > 4)
                        w = float.Parse(splitted[4]);
                    Normals.Add(new Vector3(x, y, z));
                }
                else if(line.StartsWith("v"))
                {
                    var splitted = line.Split(' ');
                    var x = float.Parse(splitted[1]);
                    if (mostLeftX == float.MinValue || x < mostLeftX)
                        mostLeftX = x;
                    if (mostRightX == float.MinValue || x > mostRightX)
                        mostRightX = x;

                    var y = float.Parse(splitted[2]);
                    if (mostLeftY == float.MinValue || y < mostLeftY)
                        mostLeftY = y;
                    if (mostRightY == float.MinValue || y > mostRightY)
                        mostRightY = y;

                    var z = float.Parse(splitted[3]);
                    if (mostLeftZ == float.MinValue || z < mostLeftZ)
                        mostLeftZ = z;
                    if (mostRightZ == float.MinValue || z > mostRightZ)
                        mostRightZ = z;

                    var w = 1.0f;
                    if (splitted.Count() > 4)
                        w = float.Parse(splitted[4]);
                    Vertice.Add(new Vector3(x, y, z));
                }
                else if(line.StartsWith("f"))
                {
                    var splitted = line.Split(' ');
                    var v1 = ulong.Parse(splitted[1].Split('/').First());
                    var v2 = ulong.Parse(splitted[2].Split('/').First());
                    var v3 = ulong.Parse(splitted[3].Split('/').First());
                    Indices.Add(v1 - 1);
                    Indices.Add(v2 - 1);
                    Indices.Add(v3 - 1);
                }
            }
            Size = new CubeSize(mostRightX - mostLeftX, mostRightY - mostLeftY, mostRightZ - mostLeftZ);
        }
    }
}