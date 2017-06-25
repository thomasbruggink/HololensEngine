using CoreTypes;

namespace Example.Scenes
{
    /// <summary>
    /// Exmaple class implements IScene to be rendered
    /// </summary>
    public class ExampleScene : IScene
    {
        private Vector3 _locationCube;
        private Vector3 _rotationCube;
        private HololensEngineCore.ColoredCube _cube;

        private Vector3 _locationPrettyCube;
        private Vector3 _rotationPrettyCube;
        private HololensEngineCore.ColoredCube _prettyCube;

        private Vector3 _locationTexturedCube;
        private Vector3 _rotationTexturedCube;
        private HololensEngineCore.TexturedCube _texturedCube;

        private CubeSize _size;

        private readonly HololensEngineCore.Engine _engine;

        public ExampleScene(HololensEngineCore.Engine engine)
        {
            //Store the engine so we can load meshes
            _engine = engine;
        }

        public void Load()
        {
            //Hololens renders in meters so 0.2f is 20cm
            _size = new CubeSize(0.2f, 0.2f, 0.2f);

            //Hold 10cm between the cubes
            _locationCube = new Vector3(0, 0, -2.0f);
            _locationPrettyCube = new Vector3(0.3f, 0.0f, -2.0f);
            _locationTexturedCube = new Vector3(-0.3f, 0.0f, -2.0f);

            //Color the center cube red
            _cube = new HololensEngineCore.ColoredCube(_locationCube, _size, Color.Red(), _engine.GetWindow().GetDrawEngine());
            
            //Load the pretty cube without an initial color
            _prettyCube = new HololensEngineCore.ColoredCube(_locationPrettyCube, _size, _engine.GetWindow().GetDrawEngine());

            //Load the image 
            var imageData = ImageData.LoadImage("Assets\\logo.jpg");
            _texturedCube = new HololensEngineCore.TexturedCube(_locationTexturedCube, _size, _engine.GetWindow().GetDrawEngine(), imageData);

            //Get the rotation vector
            _rotationCube = _cube.Rotation();
            _rotationPrettyCube = _prettyCube.Rotation();
            _rotationTexturedCube = _texturedCube.Rotation();

            //Modify the colors of the pretty cube
            //Colors in directx go from 0 to 1 which can be translated in bytes from 0 to 255
            _prettyCube.ChangeColor(0, new Color(0, 0, 0));
            _prettyCube.ChangeColor(1, new Color(0, 0, 1));
            _prettyCube.ChangeColor(2, new Color(0, 1, 0));
            _prettyCube.ChangeColor(3, new Color(0, 1, 1));
            _prettyCube.ChangeColor(4, new Color(1, 0, 0));
            _prettyCube.ChangeColor(5, new Color(1, 0, 0));
            _prettyCube.ChangeColor(6, new Color(1, 1, 0));
            _prettyCube.ChangeColor(7, new Color(1, 1, 1));
        }

        public void Update(double gametime)
        {
            //Rotate over y every tick. If holding steady 60fps every tick will contain ~16.6.
            _rotationCube.Y += (float)(gametime / 16.66666);
            _rotationPrettyCube.Y += (float)(gametime / 16.66666);
            _rotationTexturedCube.Y += (float)(gametime / 16.66666);
        }

        public void Draw()
        {
            _cube.Draw();
            _prettyCube.Draw();
            _texturedCube.Draw();
        }
    }
}
