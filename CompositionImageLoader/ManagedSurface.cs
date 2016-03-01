using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI.Composition;

namespace Robmikh.Util.CompositionImageLoader
{
    public interface IManagedSurface : IDisposable
    {
        ICompositionSurface Surface { get; }
        IAsyncAction RedrawSurface();
        IAsyncAction RedrawSurface(Uri uri);
    }

    class ManagedSurface : IManagedSurface
    {
        private IImageLoaderInternal _imageLoader;
        private CompositionDrawingSurface _surface;
        private Uri _uri;

        public ICompositionSurface Surface
        {
            get
            {
                return _surface;
            }
        }

        public ManagedSurface(IImageLoaderInternal imageLoader, Uri uri)
        {
            _imageLoader = imageLoader;
            _uri = uri;
            _surface = _imageLoader.CreateSurface();

            _imageLoader.DeviceReplacedEvent += OnDeviceReplaced;
        }

        private async void OnDeviceReplaced(object sender, object e)
        {
            await RedrawSurface();
        }

        public IAsyncAction RedrawSurface()
        {
            return RedrawSurfaceWorker(_uri).AsAsyncAction();
        }

        public IAsyncAction RedrawSurface(Uri uri)
        {
            return RedrawSurfaceWorker(uri).AsAsyncAction();
        }

        private async Task RedrawSurfaceWorker(Uri uri)
        {
            _uri = uri;
            await _imageLoader.DrawSurface(_surface, _uri);
        }

        public void Dispose()
        {
            _surface.Dispose();
            _imageLoader.DeviceReplacedEvent -= OnDeviceReplaced;
            _surface = null;
            _imageLoader = null;
            _uri = null;
        }
    }
}
