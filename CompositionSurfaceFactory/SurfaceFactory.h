#pragma once
#include "SurfaceFactoryOptions.h"
#include "TextSurfaceEnums.h"
#include "ImageEnums.h"

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
    ref class SurfaceFactory;
    ref class SharedLock;
    ref class UriSurface;
    ref class TextSurface;
    value struct Padding;
    value struct SurfaceFactoryOptions;

    [Windows::Foundation::Metadata::WebHostHiddenAttribute]
    public ref class SurfaceFactory sealed
    {
    public:
        [Windows::Foundation::Metadata::DefaultOverloadAttribute]
        static SurfaceFactory^ CreateFromCompositor(Compositor^ compositor);
        static SurfaceFactory^ CreateFromCompositor(Compositor^ compositor, SurfaceFactoryOptions options);
        [Windows::Foundation::Metadata::DefaultOverloadAttribute]
        static SurfaceFactory^ CreateFromGraphicsDevice(CompositionGraphicsDevice^ graphicsDevice);
        static SurfaceFactory^ CreateFromGraphicsDevice(CompositionGraphicsDevice^ graphicsDevice, SharedLock^ lock);

        property Compositor^ Compositor { WUC::Compositor^ get() { return m_compositor; } }
        property CompositionGraphicsDevice^ GraphicsDevice { CompositionGraphicsDevice^ get() { return m_graphicsDevice; }}
        property SharedLock^ DrawingLock { SharedLock^ get() { return m_drawingLock; }}

        CompositionDrawingSurface^ CreateSurface(Size size);

        CompositionDrawingSurface^ CreateSurfaceFromUri(Uri^ uri);
        CompositionDrawingSurface^ CreateSurfaceFromUri(Uri^ uri, Size size);
        CompositionDrawingSurface^ CreateSurfaceFromUri(Uri^ uri, Size size, InterpolationMode interpolation);

        IAsyncOperation<CompositionDrawingSurface^>^ CreateSurfaceFromUriAsync(Uri^ uri);
        IAsyncOperation<CompositionDrawingSurface^>^ CreateSurfaceFromUriAsync(Uri^ uri, Size size);
        IAsyncOperation<CompositionDrawingSurface^>^ CreateSurfaceFromUriAsync(Uri^ uri, Size size, InterpolationMode interpolation);

        CompositionDrawingSurface^ CreateSurfaceFromBytes(const Platform::Array<byte>^ bytes, int widthInPixels, int heightInPixels);
        CompositionDrawingSurface^ CreateSurfaceFromBytes(const Platform::Array<byte>^ bytes, int widthInPixels, int heightInPixels, Size size);
        CompositionDrawingSurface^ CreateSurfaceFromBytes(const Platform::Array<byte>^ bytes, int widthInPixels, int heightInPixels, Size size, InterpolationMode interpolation);

        UriSurface^ CreateUriSurface(Uri^ uri);
        UriSurface^ CreateUriSurface(Uri^ uri, Size size);
        UriSurface^ CreateUriSurface(Uri^ uri, Size size, InterpolationMode interpolation);

        IAsyncOperation<UriSurface^>^ CreateUriSurfaceAsync(Uri^ uri);
        IAsyncOperation<UriSurface^>^ CreateUriSurfaceAsync(Uri^ uri, Size size);
        IAsyncOperation<UriSurface^>^ CreateUriSurfaceAsync(Uri^ uri, Size size, InterpolationMode interpolation);

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
        SurfaceFactory(CompositionGraphicsDevice^ graphicsDevice, SharedLock^ lock);

        void Uninitialize();

        void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args);
        void OnRenderingDeviceReplaced(Windows::UI::Composition::CompositionGraphicsDevice ^sender, Windows::UI::Composition::RenderingDeviceReplacedEventArgs ^args);
        void OnDeviceLost(Microsoft::Graphics::Canvas::CanvasDevice ^sender, Platform::Object ^args);

        void CreateDevice(SurfaceFactoryOptions options);
        void RaiseDeviceReplacedEvent(RenderingDeviceReplacedEventArgs ^args);     
        void DrawBitmap(CompositionDrawingSurface^ surface, CanvasBitmap^ canvasBitmap, Size size, InterpolationMode interpolation);
    internal:
        concurrency::task<void> DrawSurface(CompositionDrawingSurface^ surface, Uri^ uri, Size size, InterpolationMode interpolation) __resumable;
        void ResizeSurface(CompositionDrawingSurface^ surface, Size size);
    public:
        event EventHandler<RenderingDeviceReplacedEventArgs^>^ DeviceReplaced;
    private:
        WUC::Compositor^ m_compositor;
        CanvasDevice^ m_canvasDevice;
        CompositionGraphicsDevice^ m_graphicsDevice;
        SharedLock^ m_drawingLock;
        bool m_isCanvasDeviceCreator;
        bool m_isGraphicsDeviceCreator;

        EventRegistrationToken OnDeviceLostHandler;
        EventRegistrationToken OnRenderingDeviceReplacedHandler;
        EventRegistrationToken OnDisplayContentsInvalidatedHandler;
    };
}
}