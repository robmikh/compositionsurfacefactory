#pragma once
#include "SurfaceFactoryOptions.h"
#include "TextSurfaceEnums.h"
#include "ImageEnums.h"
#include "DeviceLostHelper.h"

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
    ref class SurfaceFactory;
    ref class Lock;
    ref class UriSurface;
    ref class TextSurface;
    value struct Padding;
    value struct SurfaceFactoryOptions;

    [Windows::Foundation::Metadata::WebHostHiddenAttribute]
    public ref class SurfaceFactory sealed
    {
    public:
		static SurfaceFactory^ GetSharedSurfaceFactoryForCompositor(Compositor^ compositor);
		static void ClearSharedSurfaceFactories();
        [Windows::Foundation::Metadata::DefaultOverloadAttribute]
        static SurfaceFactory^ CreateFromCompositor(Compositor^ compositor);
        static SurfaceFactory^ CreateFromCompositor(Compositor^ compositor, SurfaceFactoryOptions options);
        [Windows::Foundation::Metadata::DefaultOverloadAttribute]
        static SurfaceFactory^ CreateFromGraphicsDevice(CompositionGraphicsDevice^ graphicsDevice);
        static SurfaceFactory^ CreateFromGraphicsDevice(CompositionGraphicsDevice^ graphicsDevice, Lock^ lock);

        property Compositor^ Compositor { WUC::Compositor^ get() { return m_compositor; } }
        property CompositionGraphicsDevice^ GraphicsDevice { CompositionGraphicsDevice^ get() { return m_graphicsDevice; }}
        property Lock^ DrawingLock { Lock^ get() { return m_drawingLock; }}

        CompositionDrawingSurface^ CreateSurface(Size size);
		void ResizeSurface(CompositionDrawingSurface^ surface, Size size);

		UriSurface^ CreateUriSurface();
        UriSurface^ CreateUriSurface(Uri^ uri);
        UriSurface^ CreateUriSurface(Uri^ uri, Size size);
        UriSurface^ CreateUriSurface(Uri^ uri, Size size, InterpolationMode interpolation);

        IAsyncOperation<UriSurface^>^ CreateUriSurfaceAsync(Uri^ uri);
        IAsyncOperation<UriSurface^>^ CreateUriSurfaceAsync(Uri^ uri, Size size);
        IAsyncOperation<UriSurface^>^ CreateUriSurfaceAsync(Uri^ uri, Size size, InterpolationMode interpolation);

		TextSurface^ CreateTextSurface();
        TextSurface^ CreateTextSurface(Platform::String^ text);
        TextSurface^ CreateTextSurface(Platform::String^ text,
                                       float width,
                                       float height,
                                       Platform::String^ fontFamily,
                                       float fontSize,
                                       WUT::FontStyle fontStyle,
                                       TextHorizontalAlignment horizontalAlignment,
                                       TextVerticalAlignment verticalAlignment,
                                       CompositionSurfaceFactory::WordWrapping wordWrapping,
                                       CompositionSurfaceFactory::Padding padding,
                                       Windows::UI::Color foregroundColor,
                                       Windows::UI::Color backgroundColor);

        virtual ~SurfaceFactory();
    private:
        SurfaceFactory(WUC::Compositor^ compositor, SurfaceFactoryOptions options);
        SurfaceFactory(CompositionGraphicsDevice^ graphicsDevice, Lock^ lock);

        void Uninitialize();

        void OnRenderingDeviceReplaced(Windows::UI::Composition::CompositionGraphicsDevice ^sender, Windows::UI::Composition::RenderingDeviceReplacedEventArgs ^args);
        void OnDeviceLost(Platform::Object^ sender, DeviceLostEventArgs^ args);

        void CreateDevice(SurfaceFactoryOptions options);
        void RaiseDeviceReplacedEvent(RenderingDeviceReplacedEventArgs ^args);     
    public:
        event EventHandler<RenderingDeviceReplacedEventArgs^>^ DeviceReplaced;
    private:
        WUC::Compositor^ m_compositor;
        CanvasDevice^ m_canvasDevice;
        CompositionGraphicsDevice^ m_graphicsDevice;
        Lock^ m_drawingLock;
        bool m_isGraphicsDeviceCreator;

		DeviceLostHelper^ m_deviceLostHelper;
		Windows::Foundation::EventRegistrationToken OnDeviceLostHandler;
		Windows::Foundation::EventRegistrationToken OnRenderingDeviceReplacedHandler;

		static IVector<SurfaceFactory^>^ s_surfaceFactories;
		static Lock^ s_listLock;
    };
}
}