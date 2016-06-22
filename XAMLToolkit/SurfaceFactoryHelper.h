#pragma once

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
namespace CompositionToolkit
{
namespace XAMLToolkit
{
    [Windows::Foundation::Metadata::WebHostHiddenAttribute]
    public ref class SurfaceFactoryHelper sealed
    {
    public:
        static IAsyncOperation<CompositionDrawingSurface^>^ CreateSurfaceFromUIElementAsync(SurfaceFactory^ factory, UIElement^ element);
        static IAsyncOperation<CompositionDrawingSurface^>^ CreateSurfaceFromUIElementAsync(SurfaceFactory^ factory, UIElement^ element, Size size);
    private:
        SurfaceFactoryHelper();
    };
}
}
}
}