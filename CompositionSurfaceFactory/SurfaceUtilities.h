#pragma once

#include "SurfaceUtilities.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct SurfaceUtilities
    {
        SurfaceUtilities() = delete;

        static void FillSurfaceWithColor(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, Windows::UI::Color const& color, Windows::Foundation::Size const& size);
        static void FillSurfaceWithColor(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, Windows::UI::Color const& color);
        static void FillSurfaceWithDirect3DSurface(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& direct3DSurface, Windows::Foundation::Size const& size, Robmikh::CompositionSurfaceFactory::InterpolationMode const& interpolation);
        static void FillSurfaceWithDirect3DSurface(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& direct3DSurface, Windows::Foundation::Size const& size);
        static void FillSurfaceWithDirect3DSurface(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& direct3DSurface);
        static Windows::Foundation::IAsyncAction FillSurfaceWithUriAsync(Robmikh::CompositionSurfaceFactory::SurfaceFactory const surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const surface, Windows::Foundation::Uri const uri, Windows::Foundation::Size const size, Robmikh::CompositionSurfaceFactory::InterpolationMode const interpolation);
        static Windows::Foundation::IAsyncAction FillSurfaceWithUriAsync(Robmikh::CompositionSurfaceFactory::SurfaceFactory const surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const surface, Windows::Foundation::Uri const uri, Windows::Foundation::Size const size);
        static Windows::Foundation::IAsyncAction FillSurfaceWithUriAsync(Robmikh::CompositionSurfaceFactory::SurfaceFactory const surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const surface, Windows::Foundation::Uri const uri);
        static void FillSurfaceWithBytes(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, array_view<uint8_t const> bytes, int32_t widthInPixels, int32_t heightInPixels, Windows::Foundation::Size const& size, Robmikh::CompositionSurfaceFactory::InterpolationMode const& interpolation);
        static void FillSurfaceWithBytes(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, array_view<uint8_t const> bytes, int32_t widthInPixels, int32_t heightInPixels, Windows::Foundation::Size const& size);
        static void FillSurfaceWithBytes(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, array_view<uint8_t const> bytes, int32_t widthInPixels, int32_t heightInPixels);

    private:
        static void FillSurfaceWithCanvasBitmap(Robmikh::CompositionSurfaceFactory::SurfaceFactory const& surfaceFactory, Windows::UI::Composition::CompositionDrawingSurface const& surface, Microsoft::Graphics::Canvas::CanvasBitmap const& bitmap, Windows::Foundation::Size const& size, Robmikh::CompositionSurfaceFactory::InterpolationMode const& interpolation);
    };
}

namespace winrt::Robmikh::CompositionSurfaceFactory::factory_implementation
{
    struct SurfaceUtilities : SurfaceUtilitiesT<SurfaceUtilities, implementation::SurfaceUtilities>
    {
    };
}
