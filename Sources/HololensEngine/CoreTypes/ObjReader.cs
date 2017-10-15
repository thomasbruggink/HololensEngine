using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace CoreTypes
{
    public sealed class ObjReader
    {
        private readonly string _file;
        public IList<VertexTextureNormal> Vertice { get; }
        public IList<Vector2> Textures { get; }
        public IList<Vector3> Normals { get; }
        public IList<ulong> Indices { get; }
        public CubeSize Size { get; set; }

        public int VertexCount { get { return Vertice.Count; } }
        public int NormalCount { get { return Normals.Count; } }
        public int IndexCount { get { return Indices.Count; } }

        public ObjReader(string file)
        {
            _file = file;
            Vertice = new List<VertexTextureNormal>();
            Textures = new List<Vector2>();
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
                    var blocks = GetBlocks("vn", line);
                    var i = float.Parse(blocks[0]);
                    var j = float.Parse(blocks[1]);
                    var k = float.Parse(blocks[2]);
                    Normals.Add(new Vector3(i, j, k));
                }
                else if(line.StartsWith("vt"))
                {
                    //Texture
                }
                else if(line.StartsWith("v"))
                {
                    var blocks = GetBlocks("v", line);

                    var x = float.Parse(blocks[0]);
                    if (mostLeftX == float.MinValue || x < mostLeftX)
                        mostLeftX = x;
                    if (mostRightX == float.MinValue || x > mostRightX)
                        mostRightX = x;

                    var y = float.Parse(blocks[1]);
                    if (mostLeftY == float.MinValue || y < mostLeftY)
                        mostLeftY = y;
                    if (mostRightY == float.MinValue || y > mostRightY)
                        mostRightY = y;

                    var z = float.Parse(blocks[2]);
                    if (mostLeftZ == float.MinValue || z < mostLeftZ)
                        mostLeftZ = z;
                    if (mostRightZ == float.MinValue || z > mostRightZ)
                        mostRightZ = z;

                    var w = 1.0f;
                    if (blocks.Count > 3)
                        w = float.Parse(blocks[3]);
                    Vertice.Add(new VertexTextureNormal
                    {
                        Vertex = new Vector3(x, y, z)
                    });
                }
                else if(line.StartsWith("f"))
                {
                    var points = ReadF(line);
                    for (var i = 0; i < 3; i++)
                    {
                        var point = points[i];
                        //obj data is 1 based
                        Indices.Add(point.vertex - 1);
                        //Skip textures for now
                        Vertice[(int)point.vertex - 1].Normal = Normals[(int)point.normal - 1];
                    }
                    if(points.Count == 4)
                    {
                        for(var i = 0; i < 4; i++)
                        {
                            var point = points[i];
                            //obj data is 1 based
                            Indices.Add(point.vertex - 1);
                            //Skip textures for now
                            Vertice[(int)point.vertex - 1].Normal = Normals[(int)point.normal - 1];

                            if (i == 0)
                                i++;
                        }
                    }
                }
            }
            Size = new CubeSize(Math.Abs(mostRightX) - Math.Abs(mostLeftX), Math.Abs(mostRightY) - Math.Abs(mostLeftY), Math.Abs(mostRightZ) - Math.Abs(mostLeftZ));
        }

        private List<(ulong vertex, ulong normal)> ReadF(string line)
        {
            return GetBlocks("f", line).Select(input =>
            {
                var split = input.Split('/');
                var v = ulong.Parse(split[0]);
                //var t = ulong.Parse(split[1]);
                var n = ulong.Parse(split[2]);
                return (v, n);
            }).ToList();
        }
        
        private List<string> GetBlocks(string type, string inputLine)
        {
            var result = new List<string>();
            inputLine = inputLine.Substring(type.Length);
            var data = string.Empty;
            for (var i = 0; i < inputLine.Length; i++)
            {
                if (inputLine[i] == ' ' && data != string.Empty)
                {
                    result.Add(data);
                    data = string.Empty;
                }
                else if (inputLine[i] != ' ')
                    data += inputLine[i];
            }
            if (data != string.Empty)
                result.Add(data);
            return result;
        }
    }
}