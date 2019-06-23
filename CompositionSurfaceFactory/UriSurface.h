#pragma once

#include "UriSurface.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct UriSurface : UriSurfaceT<UriSurface>
    {
        UriSurface(
            API::SurfaceFactory const& surfaceFactory,
            Windows::Foundation::Uri const& uri,
            Windows::Foundation::Size const size,
            API::InterpolationMode const interpolation);
        ~UriSurface() { Close(); }

        Windows::UI::Composition::Compositor Compositor() { CheckClosed(); return m_surfaceFactory.Compositor(); }
        Robmikh::CompositionSurfaceFactory::SurfaceFactory SurfaceFactory() { CheckClosed(); return m_surfaceFactory; }
        Windows::UI::Composition::ICompositionSurface Surface() { CheckClosed(); return m_surface; }
        Windows::Foundation::Uri Source() { CheckClosed(); return m_uri; }
        Windows::Foundation::Size Size();
        Robmikh::CompositionSurfaceFactory::InterpolationMode InterpolationMode() { CheckClosed(); return m_interpolationMode; }
        Windows::Foundation::IAsyncAction RedrawSurfaceAsync();
        Windows::Foundation::IAsyncAction RedrawSurfaceAsync(Windows::Foundation::Uri const uri);
        Windows::Foundation::IAsyncAction RedrawSurfaceAsync(Windows::Foundation::Uri const uri, Windows::Foundation::Size const size);
        Windows::Foundation::IAsyncAction RedrawSurfaceAsync(Windows::Foundation::Uri const uri, Windows::Foundation::Size const size, Robmikh::CompositionSurfaceFactory::InterpolationMode const interpolation);
        void Resize(Windows::Foundation::Size const& size);
        void Close();

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

        fire_and_forget OnDeviceReplaced(winrt::Windows::Foundation::IInspectable const& sender, Windows::UI::Composition::RenderingDeviceReplacedEventArgs const& args);

    private:
        API::SurfaceFactory m_surfaceFactory{ nullptr };
        Windows::UI::Composition::CompositionDrawingSurface m_surface{ nullptr };
        Windows::Foundation::Uri m_uri{ nullptr };
        API::InterpolationMode m_interpolationMode;
        Windows::Foundation::Size m_size;
        API::SurfaceFactory::DeviceReplaced_revoker m_deviceReplaced;
        std::atomic<bool> m_closed = false;
    };
}
