using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.UI.Composition;
using Robmikh.Util.CompositionImageLoader;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Graphics.DirectX;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media.Imaging;

namespace Robmikh.Util.CompositionImageLoader.XamlExtensions
{
    public static class ImageLoaderExtensions
    {
        public static IAsyncOperation<CompositionDrawingSurface> LoadImageFromUIElementAsync(this IImageLoader imageLoader, UIElement element)
        {
            return LoadImageFromUIElementAsyncWorker(imageLoader, element, Size.Empty).AsAsyncOperation<CompositionDrawingSurface>();
        }

        public static IAsyncOperation<CompositionDrawingSurface> LoadImageFromUIElementAsync(this IImageLoader imageLoader, UIElement element, Size size)
        {
            return LoadImageFromUIElementAsyncWorker(imageLoader, element, size).AsAsyncOperation<CompositionDrawingSurface>();
        }

        private static async Task<CompositionDrawingSurface> LoadImageFromUIElementAsyncWorker(IImageLoader imageLoader, UIElement element, Size size)
        {
            var xamlBitmap = new RenderTargetBitmap();
            await xamlBitmap.RenderAsync(element);
            var pixels = await xamlBitmap.GetPixelsAsync();

            return imageLoader.LoadImageFromBytes(pixels.ToArray(), xamlBitmap.PixelWidth, xamlBitmap.PixelHeight, size);
        }
    }
}
