using Robmikh.CompositionSurfaceFactory.CompositionToolkit.XAMLToolkit;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI.Composition;
using Windows.UI.Xaml;

namespace Robmikh.CompositionSurfaceFactory.CompositionToolkit.XAMLToolkit.Extensions
{
    public static class SurfaceFactoryExtensions
    {
        public static IAsyncOperation<CompositionDrawingSurface> CreateSurfaceFromUIElementAsync(this SurfaceFactory factory, UIElement element)
        {
            return SurfaceFactoryHelper.CreateSurfaceFromUIElementAsync(factory, element);
        }

        public static IAsyncOperation<CompositionDrawingSurface> CreateSurfaceFromUIElementAsync(this SurfaceFactory factory, UIElement element, Size size)
        {
            return SurfaceFactoryHelper.CreateSurfaceFromUIElementAsync(factory, element, size);
        }
    }
}
