#include "pch.h"
#include "UriSurface.h"
#include "SurfaceUtilities.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Composition;

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    UriSurface::UriSurface(
        API::SurfaceFactory const& surfaceFactory,
        Uri const& uri,
        Windows::Foundation::Size const size,
        API::InterpolationMode const interpolation)
    {
        m_surfaceFactory = surfaceFactory;
        m_uri = uri;
        m_size = size;
        m_interpolationMode = interpolation;

        m_surface = m_surfaceFactory.CreateSurface(m_size);
        m_deviceReplaced = m_surfaceFactory.DeviceReplaced(auto_revoke, { this, &UriSurface::OnDeviceReplaced });
    }

    Windows::Foundation::Size UriSurface::Size()
    {
        CheckClosed();
        if (m_surface != nullptr)
        {
            return m_surface.Size();
        }
        else
        {
            return { 0, 0 };
        }
    }

    Windows::Foundation::IAsyncAction UriSurface::RedrawSurfaceAsync()
    {
        CheckClosed();
        co_await RedrawSurfaceAsync(m_uri);
    }

    Windows::Foundation::IAsyncAction UriSurface::RedrawSurfaceAsync(Windows::Foundation::Uri const uri)
    {
        CheckClosed();
        co_await RedrawSurfaceAsync(m_uri, m_size);
    }

    Windows::Foundation::IAsyncAction UriSurface::RedrawSurfaceAsync(Windows::Foundation::Uri const uri, Windows::Foundation::Size const size)
    {
        CheckClosed();
        co_await RedrawSurfaceAsync(m_uri, m_size, m_interpolationMode);
    }

    Windows::Foundation::IAsyncAction UriSurface::RedrawSurfaceAsync(Windows::Foundation::Uri const uri, Windows::Foundation::Size const size, Robmikh::CompositionSurfaceFactory::InterpolationMode const interpolation)
    {
        CheckClosed();
        m_uri = uri;
        m_interpolationMode = interpolation;
        m_size = size;

        if (m_uri != nullptr)
        {
            co_await implementation::SurfaceUtilities::FillSurfaceWithUriAsync(m_surfaceFactory, m_surface, m_uri, m_size, m_interpolationMode);
        }
        else
        {
            implementation::SurfaceUtilities::FillSurfaceWithColor(m_surfaceFactory, m_surface, Colors::Transparent(), m_size);
        }
    }

    void UriSurface::Resize(Windows::Foundation::Size const& size)
    {
        CheckClosed();
        m_surfaceFactory.ResizeSurface(m_surface, size);
    }

    void UriSurface::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_deviceReplaced.revoke();
            m_surface.Close();
            m_surface = nullptr;
            m_surfaceFactory = nullptr;
            m_uri = nullptr;
        }
    }

    fire_and_forget UriSurface::OnDeviceReplaced(
        winrt::Windows::Foundation::IInspectable const&, 
        RenderingDeviceReplacedEventArgs const& args)
    {
        co_await RedrawSurfaceAsync();
    }
}
