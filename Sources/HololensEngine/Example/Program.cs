using Example.Scenes;

namespace Example
{
    public class Program
    {
        public static void Main()
        {
            //Create the initial scene and give control to the engine
            //The engine will create the core window and attach DirectX
            var engine = new HololensEngineCore.Engine();
            var exampleScene = new ExampleScene(engine);
            var objScene = new ObjScene(engine);
            //Start the engine
            engine.Start(objScene);
        }
    }
}
