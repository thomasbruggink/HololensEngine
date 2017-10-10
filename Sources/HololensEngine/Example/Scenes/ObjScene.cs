using CoreTypes;
using HololensEngineCore;

namespace Example.Scenes
{
    public class ObjScene : IScene
    {
        private readonly Engine _engine;
        private Mesh _mesh;
        private Vector3 _rotation;

        public ObjScene(Engine engine)
        {
            _engine = engine;
        }

        public void Draw()
        {
            _mesh.Draw();
        }

        public void Load()
        {
            var objReader = new ObjReader("Assets\\Cube.obj");
            objReader.Load();
            _mesh = new Mesh(new Vector3(0, 0, -4.0f), objReader, _engine.GetWindow().GetDrawEngine());
            _rotation = _mesh.Rotation();
            _rotation.Z += 180;
        }

        public void Update(double gametime)
        {
            _rotation.Y += (float)(gametime / 16.66666) / 2.0f;
        }
    }
}
