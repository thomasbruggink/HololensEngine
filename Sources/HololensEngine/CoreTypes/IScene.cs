namespace CoreTypes
{
    public interface IScene
    {
        void Load();
        void Update(double gametime);
        void Draw();
    }
}
