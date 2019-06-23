#pragma once

#include "SurfaceFactory.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct SurfaceFactory : SurfaceFactoryT<SurfaceFactory>
    {
        SurfaceFactory(
            Windows::UI::Composition::Compositor const& compositor,
            Robmikh::CompositionSurfaceFactory::SurfaceFactoryOptions const& options);
        SurfaceFactory(
            Windows::UI::Composition::CompositionGraphicsDevice const& graphicsDevice,
            Robmikh::CompositionSurfaceFactory::Lock const& lock);
        ~SurfaceFactory() { Close(); }

        Windows::UI::Composition::Compositor Compositor() { CheckClosed(); return m_compositor; }
        Windows::UI::Composition::CompositionGraphicsDevice GraphicsDevice() { CheckClosed(); return m_graphicsDevice; }
        Robmikh::CompositionSurfaceFactory::Lock DrawingLock() { CheckClosed(); return m_lock; }
        Windows::UI::Composition::CompositionDrawingSurface CreateSurface(Windows::Foundation::Size const& size);
        void ResizeSurface(Windows::UI::Composition::CompositionDrawingSurface const& surface, Windows::Foundation::Size const& size);
        Robmikh::CompositionSurfaceFactory::UriSurface CreateUriSurface() { CheckClosed(); return CreateUriSurface(nullptr); }
        Robmikh::CompositionSurfaceFactory::UriSurface CreateUriSurface(Windows::Foundation::Uri const& uri) { CheckClosed(); return CreateUriSurface(uri, { 0, 0 }); }
        Robmikh::CompositionSurfaceFactory::UriSurface CreateUriSurface(Windows::Foundation::Uri const& uri, Windows::Foundation::Size const& size) { CheckClosed(); return CreateUriSurface(uri, size, API::InterpolationMode::Linear); }
        Robmikh::CompositionSurfaceFactory::UriSurface CreateUriSurface(Windows::Foundation::Uri const& uri, Windows::Foundation::Size const& size, Robmikh::CompositionSurfaceFactory::InterpolationMode const& interpolation);
        Windows::Foundation::IAsyncOperation<Robmikh::CompositionSurfaceFactory::UriSurface> CreateUriSurfaceAsync(Windows::Foundation::Uri const uri) { CheckClosed(); return co_await CreateUriSurfaceAsync(uri, { 0, 0 }); }
        Windows::Foundation::IAsyncOperation<Robmikh::CompositionSurfaceFactory::UriSurface> CreateUriSurfaceAsync(Windows::Foundation::Uri const uri, Windows::Foundation::Size const size) { CheckClosed();  return co_await CreateUriSurfaceAsync(uri, size, API::InterpolationMode::Linear); }
        Windows::Foundation::IAsyncOperation<Robmikh::CompositionSurfaceFactory::UriSurface> CreateUriSurfaceAsync(Windows::Foundation::Uri const uri, Windows::Foundation::Size const size, Robmikh::CompositionSurfaceFactory::InterpolationMode const interpolation);
        Robmikh::CompositionSurfaceFactory::TextSurface CreateTextSurface() { CheckClosed(); return CreateTextSurface(L""); }
        Robmikh::CompositionSurfaceFactory::TextSurface CreateTextSurface(hstring const& text) 
        {
            CheckClosed(); 
            return CreateTextSurface(
                text, 
                0, 
                0, 
                L"Segoe UI", 
                14.0f, 
                Windows::UI::Text::FontStyle::Normal,
                API::TextHorizontalAlignment::Left, 
                API::TextVerticalAlignment::Top, 
                API::WordWrapping::NoWrap, 
                { 0, 0, 0, 0 }, 
                Windows::UI::Colors::Black(), 
                Windows::UI::Colors::Transparent());
        }
        Robmikh::CompositionSurfaceFactory::TextSurface CreateTextSurface(hstring const& text, float width, float height, hstring const& fontFamily, float fontSize, Windows::UI::Text::FontStyle const& fontStyle, Robmikh::CompositionSurfaceFactory::TextHorizontalAlignment const& horizontalAlignment, Robmikh::CompositionSurfaceFactory::TextVerticalAlignment const& verticalAlignment, Robmikh::CompositionSurfaceFactory::WordWrapping const& wordWrapping, Robmikh::CompositionSurfaceFactory::Padding const& padding, Windows::UI::Color const& foregroundColor, Windows::UI::Color const& backgroundColor);
        winrt::event_token DeviceReplaced(Windows::Foundation::EventHandler<Windows::UI::Composition::RenderingDeviceReplacedEventArgs> const& handler) { CheckClosed();  return m_deviceReplacedEvent.add(handler); }
        void DeviceReplaced(winrt::event_token const& token) noexcept { CheckClosed(); m_deviceReplacedEvent.remove(token); }
        void Close();

        static Robmikh::CompositionSurfaceFactory::SurfaceFactory GetSharedSurfaceFactoryForCompositor(Windows::UI::Composition::Compositor const& compositor);
        static void ClearSharedSurfaceFactories();
        static Robmikh::CompositionSurfaceFactory::SurfaceFactory CreateFromCompositor(Windows::UI::Composition::Compositor const& compositor);
        static Robmikh::CompositionSurfaceFactory::SurfaceFactory CreateFromCompositor(Windows::UI::Composition::Compositor const& compositor, Robmikh::CompositionSurfaceFactory::SurfaceFactoryOptions const& options);
        static Robmikh::CompositionSurfaceFactory::SurfaceFactory CreateFromGraphicsDevice(Windows::UI::Composition::CompositionGraphicsDevice const& graphicsDevice);
        static Robmikh::CompositionSurfaceFactory::SurfaceFactory CreateFromGraphicsDevice(Windows::UI::Composition::CompositionGraphicsDevice const& graphicsDevice, Robmikh::CompositionSurfaceFactory::Lock const& lock);

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

        void CreateDevice(API::SurfaceFactoryOptions const& options);
        void OnDeviceLost(winrt::Windows::Foundation::IInspectable const& sender, API::DeviceLostEventArgs const& args);
        void OnDeviceReplaced(Windows::UI::Composition::CompositionGraphicsDevice const& sender, Windows::UI::Composition::RenderingDeviceReplacedEventArgs const& args);

    private:
        Windows::UI::Composition::Compositor m_compositor{ nullptr };
        Microsoft::Graphics::Canvas::CanvasDevice m_canvasDevice{ nullptr };
        Windows::UI::Composition::CompositionGraphicsDevice m_graphicsDevice{ nullptr };
        API::Lock m_lock{ nullptr };
        bool m_isGraphicsDeviceCreator;

        API::DeviceLostHelper m_deviceLostHelper{ nullptr };
        API::DeviceLostHelper::DeviceLost_revoker m_deviceLost;
        winrt::event_token m_deviceReplacedToken;
        winrt::event<Windows::Foundation::EventHandler<Windows::UI::Composition::RenderingDeviceReplacedEventArgs>> m_deviceReplacedEvent;
        
        std::atomic<bool> m_closed = false;

        static std::vector<API::SurfaceFactory> s_surfaceFactories;
        static API::Lock s_lock;
    };
}

namespace winrt::Robmikh::CompositionSurfaceFactory::factory_implementation
{
    struct SurfaceFactory : SurfaceFactoryT<SurfaceFactory, implementation::SurfaceFactory>
    {
    };
}
