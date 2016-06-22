#include "pch.h"
#include "SurfaceFactoryHelper.h"

using namespace Robmikh::CompositionSurfaceFactory::CompositionToolkit::XAMLToolkit;
using namespace Platform;
using namespace Windows::Storage::Streams;

IAsyncOperation<CompositionDrawingSurface^>^ SurfaceFactoryHelper::CreateSurfaceFromUIElementAsync(SurfaceFactory^ factory, UIElement^ element)
{
    return CreateSurfaceFromUIElementAsync(factory, element, Size::Empty);
}

IAsyncOperation<CompositionDrawingSurface^>^ SurfaceFactoryHelper::CreateSurfaceFromUIElementAsync(SurfaceFactory^ factory, UIElement^ element, Size size)
{
    return concurrency::create_async([=]
    {
        auto xamlBitmap = ref new RenderTargetBitmap();
        return concurrency::create_task(xamlBitmap->RenderAsync(element)).then([=]
        {
            return concurrency::create_task(xamlBitmap->GetPixelsAsync()).then([=](IBuffer^ pixels)
            {
                auto pixelArray = ref new Array<byte>(pixels->Length);
                DataReader::FromBuffer(pixels)->ReadBytes(pixelArray);

                return factory->CreateSurfaceFromBytes(pixelArray, xamlBitmap->PixelWidth, xamlBitmap->PixelHeight, size);
            });
        });
    });
}

SurfaceFactoryHelper::SurfaceFactoryHelper()
{

}