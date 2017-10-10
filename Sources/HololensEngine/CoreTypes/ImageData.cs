using ImageSharp;
using System;
using System.IO;

namespace CoreTypes
{
    public enum ImageDataFormat
    {
        R8G8B8 = 3,
        R8G8B8A8 = 4
    }

    /// <summary>
    /// Contains image information used to call the texturedcube
    /// </summary>
    public sealed class ImageData
    {
        public int Width { get; set; }
        public int Height { get; set; }
        public ImageDataFormat Format { get; set; }
        public byte[] Data { get; set; }

        public static ImageData LoadImage(string fileName)  
        {
            using (FileStream stream = File.OpenRead(fileName))
            { 
                var image = Image.Load(stream);
                var imageData = new ImageData()
                {
                    Width = image.Width,
                    Height = image.Height,
                    //Force the image to be read with an alpha value
                    Format = ImageDataFormat.R8G8B8A8
                };
                int bpp = (int)imageData.Format;
                //Calculate how many pixels there are in the image
                int pixelCount = imageData.Width * imageData.Height;
                imageData.Data = new byte[pixelCount * bpp];
                //Copy all the pixels to a format understood by the engine
                for (var i = 0; i < pixelCount; i++)
                {
                    var imageDataOffset = i * bpp;
                    imageData.Data[imageDataOffset] = image.Pixels[i].R;
                    imageData.Data[imageDataOffset + 1] = image.Pixels[i].G;
                    imageData.Data[imageDataOffset + 2] = image.Pixels[i].B;
                    imageData.Data[imageDataOffset + 3] = image.Pixels[i].A;
                }
                return imageData;
            }
        }
    }
}
