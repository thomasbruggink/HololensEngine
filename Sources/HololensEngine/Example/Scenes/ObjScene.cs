using CoreTypes;
using HololensEngineCore;
using System;

namespace Example.Scenes
{
    public class ObjScene : IScene
    {
        private readonly Engine _engine;
        private Mesh _mesh, _light;
        private Vector3 _rotation, _meshLocation, _lightLocation, _lightDirection;
        private double _lightIncr;

        public ObjScene(Engine engine)
        {
            _engine = engine;
        }

        public void Draw()
        {
            _mesh.Draw();
            _light.Draw();
        }

        public void Load()
        {
            var meshReader = new ObjReader("Assets\\Teapot.obj");
            meshReader.Load();

            var lightMeshReader = new ObjReader("Assets\\Sphere.obj");
            lightMeshReader.Load();

            _mesh = new Mesh(new Vector3(0, -0.5f, -3.0f), meshReader, _engine.GetWindow().GetDrawEngine(), Color.White());
            _mesh.AmbientLight(0.15f);
            _mesh.Scale().X = 0.05f;
            _mesh.Scale().Y = 0.05f;
            _mesh.Scale().Z = 0.05f;
            _rotation = _mesh.Rotation();
            //_rotation.Z += 180;
            //_rotation.Y += 90;
            _meshLocation = _mesh.Location();

            _light = new Mesh(new Vector3(0, 0.0f, 0.0f), lightMeshReader, _engine.GetWindow().GetDrawEngine(), Color.Orange());
            _light.AmbientLight(1.0f);
            _light.Scale().X = 0.1f;
            _light.Scale().Y = 0.1f;
            _light.Scale().Z = 0.1f;
            _lightLocation = _light.Location();

            _lightDirection = _mesh.LightDirection();
        }

        public void Update(double gametime)
        {
            _lightIncr += gametime / 16.66666;
            var newX = (float)Math.Sin(_lightIncr * 0.025);
            _lightDirection.X = (float)Math.Sin(_lightIncr * 0.025);
            _lightDirection.Y = (float)Math.Sin(_lightIncr * 0.025);
            _lightDirection.Z = (float)Math.Sin(_lightIncr * 0.025);

            _lightLocation.X = _meshLocation.X + _lightDirection.X;
            _lightLocation.Y = _meshLocation.Y + _lightDirection.Y;
            _lightLocation.Z = _meshLocation.Z + _lightDirection.Z;

            _rotation.Y += (float)(gametime / 16.66666) / 1.0f;
        }
    }
}
