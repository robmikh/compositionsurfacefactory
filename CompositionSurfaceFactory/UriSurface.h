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
        property InterpolationMode InterpolationMode { CompositionSurfaceFactory::InterpolationMode get() { return m_interpolationMode; }}

        IAsyncAction^ RedrawSurfaceAsync();
        IAsyncAction^ RedrawSurfaceAsync(Uri^ uri);
        IAsyncAction^ RedrawSurfaceAsync(Uri^ uri, Windows::Foundation::Size size);
        IAsyncAction^ RedrawSurfaceAsync(Uri^ uri, Windows::Foundation::Size size, CompositionSurfaceFactory::InterpolationMode interpolation);
        void Resize(Windows::Foundation::Size size);

        virtual ~UriSurface();
    private:
        UriSurface(CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory, Uri^ uri, Windows::Foundation::Size size, CompositionSurfaceFactory::InterpolationMode interpolation);

        void Uninitialize();

        void OnDeviceReplacedEvent(Platform::Object ^sender, Windows::UI::Composition::RenderingDeviceReplacedEventArgs ^args);
        concurrency::task<void> RedrawSurfaceWorker(Uri^ uri, WF::Size size, CompositionSurfaceFactory::InterpolationMode interpolation);
    internal:
        static UriSurface^ Create(CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory, Uri^ uri, Windows::Foundation::Size size, CompositionSurfaceFactory::InterpolationMode interpolation);
    private:
        CompositionSurfaceFactory::SurfaceFactory^ m_surfaceFactory;
        CompositionDrawingSurface^ m_surface;
        Uri^ m_uri;
        CompositionSurfaceFactory::InterpolationMode m_interpolationMode;
        WF::Size m_desiredSize;

		Windows::Foundation::EventRegistrationToken OnDeviceReplacedHandler;
    };
}
}