#include "pch.h"
#include "SurfaceFactory.h"
#include "Lock.h"
#include "UriSurface.h"
#include "TextSurface.h"
#include "DeviceLostHelper.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Text;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::UI::Composition;
using namespace Microsoft::Graphics::Canvas::Text;

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    std::vector<API::SurfaceFactory> implementation::SurfaceFactory::s_surfaceFactories;
    API::Lock implementation::SurfaceFactory::s_lock = make<Lock>();

    SurfaceFactory::SurfaceFactory(
        Windows::UI::Composition::Compositor const& compositor, 
        API::SurfaceFactoryOptions const& options)
    {
        m_compositor = compositor;
        m_lock = make<Lock>();
        m_isGraphicsDeviceCreator = true;
        m_deviceLostHelper = make<DeviceLostHelper>();
        m_deviceLost = m_deviceLostHelper.DeviceLost(auto_revoke, { this, &SurfaceFactory::OnDeviceLost });

        CreateDevice(options);
    }

    SurfaceFactory::SurfaceFactory(
        Windows::UI::Composition::CompositionGraphicsDevice const& graphicsDevice, 
        API::Lock const& lock)
    {
        m_compositor = graphicsDevice.Compositor();
        m_graphicsDevice = graphicsDevice;
        m_isGraphicsDeviceCreator = false;
        if (lock == nullptr)
        {
            m_lock = make<Lock>();
        }
        else
        {
            m_lock = lock;
        }

        m_deviceReplacedToken = m_graphicsDevice.RenderingDeviceReplaced({ this, &SurfaceFactory::OnDeviceReplaced });
    }

    CompositionDrawingSurface SurfaceFactory::CreateSurface(
        Size const& size)
    {
        auto surfaceSize = size;
        if (surfaceSize.Width <= 0 && surfaceSize.Height <= 0)
        {
            // We start out with a size of 0,0 for the surface, because we don't know
            // the size of the image at this time. We resize the surface later.
            surfaceSize = { 0, 0 };
        }

        CompositionDrawingSurface surface = nullptr;

        {
            auto lockSession = m_lock.GetLockSession();
            surface = m_graphicsDevice.CreateDrawingSurface(
                surfaceSize,
                Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
        }

        return surface;
    }

    void SurfaceFactory::ResizeSurface(
        CompositionDrawingSurface const& surface, 
        Size const& size)
    {
        auto lockSession = m_lock.GetLockSession();
        CanvasComposition::Resize(surface, size);
    }

    API::UriSurface SurfaceFactory::CreateUriSurface(
        Uri const& uri, 
        Size const& size, 
        API::InterpolationMode const& interpolation)
    {
        auto uriSurface = make<UriSurface>(*this, uri, size, interpolation);
        auto ignored = uriSurface.RedrawSurfaceAsync();

        return uriSurface;
    }

    IAsyncOperation<API::UriSurface> SurfaceFactory::CreateUriSurfaceAsync(
        Uri const uri, 
        Size const size, 
        API::InterpolationMode const interpolation)
    {
        auto uriSurface = make<UriSurface>(*this, uri, size, interpolation);
        co_await uriSurface.RedrawSurfaceAsync();

        return uriSurface;
    }

    API::TextSurface SurfaceFactory::CreateTextSurface(
        hstring const& text, 
        float width, 
        float height, 
        hstring const& fontFamily, 
        float fontSize,
        FontStyle const& fontStyle, 
        API::TextHorizontalAlignment const& horizontalAlignment, 
        API::TextVerticalAlignment const& verticalAlignment, 
        API::WordWrapping const& wordWrapping, 
        API::Padding const& padding, 
        Color const& foregroundColor, 
        Color const& backgroundColor)
    {
        auto textSurface = make<TextSurface>(
            *this,
            text,
            width,
            height,
            fontFamily,
            fontSize,
            fontStyle,
            horizontalAlignment,
            verticalAlignment,
            wordWrapping,
            padding,
            foregroundColor,
            backgroundColor);
        textSurface.RedrawSurface();

        return textSurface;
    }

    void SurfaceFactory::CreateDevice(
        API::SurfaceFactoryOptions const& options)
    {
        if (m_compositor != nullptr && m_isGraphicsDeviceCreator)
        {
            if (m_canvasDevice == nullptr)
            {
                m_canvasDevice = CanvasDevice(options.UseSoftwareRenderer);

                m_deviceLostHelper.WatchDevice(m_canvasDevice);
            }

            if (m_graphicsDevice == nullptr)
            {
                m_graphicsDevice = CanvasComposition::CreateCompositionGraphicsDevice(m_compositor, m_canvasDevice);                
                // The following doesn't work because the auto revoker requires the event source
                // to support weak refrences.
                //m_deviceReplaced = m_graphicsDevice.RenderingDeviceReplaced(auto_revoke, { this, &SurfaceFactory::OnDeviceReplaced });
                m_deviceReplacedToken = m_graphicsDevice.RenderingDeviceReplaced({ this, &SurfaceFactory::OnDeviceReplaced });
            }
        }
    }

    void SurfaceFactory::OnDeviceLost(
        winrt::Windows::Foundation::IInspectable const&, 
        API::DeviceLostEventArgs const&)
    {
        bool softwareRenderer = m_canvasDevice.ForceSoftwareRenderer();

        m_canvasDevice = CanvasDevice(softwareRenderer);
        m_deviceLostHelper.WatchDevice(m_canvasDevice);

        CanvasComposition::SetCanvasDevice(m_graphicsDevice, m_canvasDevice);
    }

    void SurfaceFactory::OnDeviceReplaced(
        CompositionGraphicsDevice const&, 
        RenderingDeviceReplacedEventArgs const& args)
    {
        if (m_deviceReplacedEvent)
        {
            m_deviceReplacedEvent(*this, args);
        }
    }

    void SurfaceFactory::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            {
                auto lockSession = m_lock.GetLockSession();

                m_compositor = nullptr;
                if (m_canvasDevice != nullptr)
                {
                    m_deviceLost.revoke();
                    m_deviceLostHelper.StopWatchingCurrentDevice();
                    m_deviceLostHelper = nullptr;

                    m_canvasDevice.Close();
                    m_canvasDevice = nullptr;
                }

                if (m_graphicsDevice != nullptr)
                {
                    m_graphicsDevice.RenderingDeviceReplaced(m_deviceReplacedToken);
                    // Only dispose the composition graphics device if we own the device.
                    if (m_isGraphicsDeviceCreator)
                    {
                        m_graphicsDevice.Close();
                    }
                    m_graphicsDevice = nullptr;
                }

                {
                    auto listLockSession = s_lock.GetLockSession();
                    auto index = std::find(std::begin(s_surfaceFactories), std::end(s_surfaceFactories), *this);
                    if (index != std::end(s_surfaceFactories))
                    {
                        s_surfaceFactories.erase(index);
                    }
                }
            }
            m_lock = nullptr;
        }
    }

    API::SurfaceFactory SurfaceFactory::GetSharedSurfaceFactoryForCompositor(
        Windows::UI::Composition::Compositor const& compositor)
    {
        API::SurfaceFactory result{ nullptr };

        // We do this under a lock so that it is safe for multiple callers on different threads. The
        // SurfaceFactory itself may also be changing the list when it is being destroyed.
        {
            auto lockSession = s_lock.GetLockSession();

            // Look to see if one of the surface factories is associated with the compositor
            for (auto& surfaceFactory : s_surfaceFactories)
            {
                if (surfaceFactory.Compositor() == compositor)
                {
                    result = surfaceFactory;
                    break;
                }
            }

            // If we didn't find one, then make one and add it to the list
            if (result == nullptr)
            {
                result = implementation::SurfaceFactory::CreateFromCompositor(compositor);
                s_surfaceFactories.push_back(result);
            }
        }

        return result;
    }

    void SurfaceFactory::ClearSharedSurfaceFactories()
    {
        std::vector<API::SurfaceFactory> copiedList;

        // We do this under a lock so that it is safe for multiple callers on different threads. The
        // SurfaceFactory itself may also be changing the list when it is being destroyed.
        {
            auto lockSession = s_lock.GetLockSession();

            // Copy the list so that when we are disposing the SurfaceFactory objects they don't
            // screw with the list as we are iterating through it.
            for (auto& surfaceFactory : s_surfaceFactories)
            {
                copiedList.push_back(surfaceFactory);
            }
        }


        // Dispose all the SurfaceFactories. This has to be done outside of the lock because the
        // SurfaceFactory will request the lock when disposing, and we would dead lock;
        for (auto& surfaceFactory : s_surfaceFactories)
        {
            surfaceFactory.Close();
        }

        {
            auto lockSession = s_lock.GetLockSession();
            s_surfaceFactories.clear();
        }

        copiedList.clear();
    }

    API::SurfaceFactory SurfaceFactory::CreateFromCompositor(
        Windows::UI::Composition::Compositor const& compositor)
    {
        auto options = API::SurfaceFactoryOptions();
        options.UseSoftwareRenderer = false;

        return implementation::SurfaceFactory::CreateFromCompositor(compositor, options);
    }

    API::SurfaceFactory SurfaceFactory::CreateFromCompositor(
        Windows::UI::Composition::Compositor const& compositor, 
        API::SurfaceFactoryOptions const& options)
    {
        auto surfaceFactory = make<SurfaceFactory>(compositor, options);
        return surfaceFactory;
    }

    API::SurfaceFactory SurfaceFactory::CreateFromGraphicsDevice(
        Windows::UI::Composition::CompositionGraphicsDevice const& graphicsDevice)
    {
        auto lock = make<Lock>();
        return implementation::SurfaceFactory::CreateFromGraphicsDevice(graphicsDevice, lock);
    }

    API::SurfaceFactory SurfaceFactory::CreateFromGraphicsDevice(
        Windows::UI::Composition::CompositionGraphicsDevice const& graphicsDevice, 
        API::Lock const& lock)
    {
        auto surfaceFactory = make<SurfaceFactory>(graphicsDevice, lock);
        return surfaceFactory;
    }
}
