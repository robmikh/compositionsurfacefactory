#include "pch.h"
#include "SurfaceUtilities.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::UI::Composition;

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    void SurfaceUtilities::FillSurfaceWithColor(
        API::SurfaceFactory const& surfaceFactory, 
        CompositionDrawingSurface const& surface, 
        Color const& color, 
        Size const& size)
    {
        auto lock = surfaceFactory.DrawingLock();

        // Make the size at least 1x1
        auto newSize = size;
        if (newSize.Width <= 0 && newSize.Height <= 0)
        {
            newSize = { 1, 1 };
        }

        // Resize the surface
        surfaceFactory.ResizeSurface(surface, newSize);

        // Clear the surface with the desired color
        // Because the drawing is done asynchronously and multiple threads could
        // be trying to get access to the device/surface at the same time, we need
        // to do any device/surface work under a lock.
        {
            auto lockSession = lock.GetLockSession();

            auto session = CanvasComposition::CreateDrawingSession(surface);
            session.Clear(color);
        }
    }

    void SurfaceUtilities::FillSurfaceWithColor(
        API::SurfaceFactory const& surfaceFactory, 
        CompositionDrawingSurface const& surface,
        Color const& color)
    {
        FillSurfaceWithColor(
            surfaceFactory,
            surface,
            color,
            { 0, 0 });
    }

    void SurfaceUtilities::FillSurfaceWithDirect3DSurface(
        API::SurfaceFactory const& surfaceFactory, 
        CompositionDrawingSurface const& surface, 
        IDirect3DSurface const& direct3DSurface, 
        Size const& size, 
        InterpolationMode const& interpolation)
    {
        auto lock = surfaceFactory.DrawingLock();
        auto canvasDevice = CanvasComposition::GetCanvasDevice(surfaceFactory.GraphicsDevice());

        auto bitmap = CanvasBitmap::CreateFromDirect3D11Surface(canvasDevice, direct3DSurface);

        implementation::SurfaceUtilities::FillSurfaceWithCanvasBitmap(surfaceFactory, surface, bitmap, size, interpolation);
    }

    void SurfaceUtilities::FillSurfaceWithDirect3DSurface(
        API::SurfaceFactory const& surfaceFactory, 
        CompositionDrawingSurface const& surface, 
        IDirect3DSurface const& direct3DSurface, 
        Size const& size)
    {
        FillSurfaceWithDirect3DSurface(
            surfaceFactory,
            surface,
            direct3DSurface,
            size,
            API::InterpolationMode::Linear);
    }

    void SurfaceUtilities::FillSurfaceWithDirect3DSurface(
        API::SurfaceFactory const& surfaceFactory, 
        CompositionDrawingSurface const& surface, 
        IDirect3DSurface const& direct3DSurface)
    {
        FillSurfaceWithDirect3DSurface(
            surfaceFactory,
            surface,
            direct3DSurface,
            { 0, 0 });
    }

    IAsyncAction SurfaceUtilities::FillSurfaceWithUriAsync(
        API::SurfaceFactory const surfaceFactory, 
        CompositionDrawingSurface const surface, 
        Uri const uri, 
        Size const size, 
        InterpolationMode const interpolation)
    {
        auto lock = surfaceFactory.DrawingLock();
        auto canvasDevice = CanvasComposition::GetCanvasDevice(surfaceFactory.GraphicsDevice());

        auto bitmap = co_await CanvasBitmap::LoadAsync(canvasDevice, uri);

        implementation::SurfaceUtilities::FillSurfaceWithCanvasBitmap(surfaceFactory, surface, bitmap, size, interpolation);
    }

    IAsyncAction SurfaceUtilities::FillSurfaceWithUriAsync(
        API::SurfaceFactory const surfaceFactory, 
        CompositionDrawingSurface const surface, 
        Uri const uri, 
        Size const size)
    {
        co_await FillSurfaceWithUriAsync(
            surfaceFactory,
            surface,
            uri,
            size,
            API::InterpolationMode::Linear);
    }

    IAsyncAction SurfaceUtilities::FillSurfaceWithUriAsync(
        API::SurfaceFactory const surfaceFactory, 
        CompositionDrawingSurface const surface, 
        Uri const uri)
    {
        co_await FillSurfaceWithUriAsync(
            surfaceFactory,
            surface,
            uri,
            { 0, 0 });
    }

    void SurfaceUtilities::FillSurfaceWithBytes(
        API::SurfaceFactory const& surfaceFactory, 
        CompositionDrawingSurface const& surface, 
        array_view<uint8_t const> bytes,
        int32_t widthInPixels, 
        int32_t heightInPixels, 
        Size const& size, 
        API::InterpolationMode const& interpolation)
    {
        auto lock = surfaceFactory.DrawingLock();
        auto canvasDevice = CanvasComposition::GetCanvasDevice(surfaceFactory.GraphicsDevice());

        auto bitmap = CanvasBitmap::CreateFromBytes(canvasDevice, bytes, widthInPixels, heightInPixels, DirectXPixelFormat::B8G8R8A8UIntNormalized);

        implementation::SurfaceUtilities::FillSurfaceWithCanvasBitmap(surfaceFactory, surface, bitmap, size, interpolation);
    }

    void SurfaceUtilities::FillSurfaceWithBytes(
        API::SurfaceFactory const& surfaceFactory, 
        CompositionDrawingSurface const& surface, 
        array_view<uint8_t const> bytes,
        int32_t widthInPixels, 
        int32_t heightInPixels, 
        Size const& size)
    {
        FillSurfaceWithBytes(
            surfaceFactory,
            surface,
            bytes,
            widthInPixels,
            heightInPixels,
            size,
            API::InterpolationMode::Linear);
    }

    void SurfaceUtilities::FillSurfaceWithBytes(
        API::SurfaceFactory const& surfaceFactory, 
        CompositionDrawingSurface const& surface, 
        array_view<uint8_t const> bytes,
        int32_t widthInPixels, 
        int32_t heightInPixels)
    {
        FillSurfaceWithBytes(
            surfaceFactory,
            surface,
            bytes,
            widthInPixels,
            heightInPixels,
            { 0, 0 });
    }

    void SurfaceUtilities::FillSurfaceWithCanvasBitmap(
        API::SurfaceFactory const& surfaceFactory,
        CompositionDrawingSurface const& surface,
        CanvasBitmap const& bitmap,
        Size const& size,
        API::InterpolationMode const& interpolation)
    {
        auto lock = surfaceFactory.DrawingLock();

        // Determine the size of our bitmap
        Size bitmapSize = bitmap.Size();

        // If the caller did not specify a valid size, then make it the same size as the bitmap
        auto newSize = size;
        if (newSize.Width <= 0 && newSize.Height <= 0)
        {
            newSize = bitmapSize;
        }

        // Resize the surface
        surfaceFactory.ResizeSurface(surface, newSize);

        // Draw the bitmap into the surface
        // Because the drawing is done asynchronously and multiple threads could
        // be trying to get access to the device/surface at the same time, we need
        // to do any device/surface work under a lock.
        {
            auto lockSession = lock.GetLockSession();

            {
                auto session = CanvasComposition::CreateDrawingSession(surface);
                Rect surfaceRect = { 0, 0, newSize.Width, newSize.Height };
                Rect bitmapRect = { 0, 0, bitmapSize.Width, bitmapSize.Height };
                CanvasImageInterpolation canvasInterpolation = static_cast<CanvasImageInterpolation>(interpolation);
                session.Clear(Colors::Transparent());
                session.DrawImage(bitmap, surfaceRect, bitmapRect, 1.0f, canvasInterpolation);
            }
        }
    }
}
