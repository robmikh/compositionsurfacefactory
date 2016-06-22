#pragma once

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
    [Windows::Foundation::Metadata::WebHostHiddenAttribute]
    public ref class UriSurface sealed
    {
    public:
        property Compositor^ Compositor { WUC::Compositor^ get() { return m_surfaceFactory->Compositor; } }
        property SurfaceFactory^ SurfaceFactory { CompositionSurfaceFactory::SurfaceFactory^ get() { return m_surfaceFactory; }}
        property ICompositionSurface^ Surface { ICompositionSurface^ get() { return m_surface; }}
        property Uri^ Source { Uri^ get() { return m_uri; }}
        property Size Size
        {
            WF::Size get()
            {
                if (m_surface != nullptr)
                {
                    return m_surface->Size;
                }
                else
                {
                    return Windows::Foundation::Size::Empty;
                }
            }
        }

        IAsyncAction^ RedrawSurface();
        IAsyncAction^ RedrawSurface(Uri^ uri);
        IAsyncAction^ RedrawSurface(Uri^ uri, Windows::Foundation::Size size);
        void Resize(Windows::Foundation::Size size);

        virtual ~UriSurface();
    private:
        UriSurface(CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory, Uri^ uri, Windows::Foundation::Size size);

        void Uninitialize();

        void OnDeviceReplacedEvent(Platform::Object ^sender, Windows::UI::Composition::RenderingDeviceReplacedEventArgs ^args);
        concurrency::task<void> RedrawSurfaceWorker(Uri^ uri, WF::Size size);
    internal:
        static UriSurface^ Create(CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory, Uri^ uri, Windows::Foundation::Size size);
    private:
        CompositionSurfaceFactory::SurfaceFactory^ m_surfaceFactory;
        CompositionDrawingSurface^ m_surface;
        Uri^ m_uri;

        EventRegistrationToken OnDeviceReplacedHandler;
    };
}
}