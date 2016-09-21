#pragma once

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
    [Windows::Foundation::Metadata::WebHostHiddenAttribute]
    public ref class TextSurfaceRedrawnEventArgs sealed
    {
    public :
        property TextSurface^ Surface
        {
            TextSurface^ get() { return m_surface; }
        }

        property SurfaceFactory^ SurfaceFactory
        {
            CompositionSurfaceFactory::SurfaceFactory^ get() { return m_surfaceFactory; }
        }
    internal:
        static TextSurfaceRedrawnEventArgs^ Create(TextSurface^ surface, CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory);
    private:
        TextSurfaceRedrawnEventArgs(TextSurface^ surface, CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory);

        TextSurface^ m_surface;
        CompositionSurfaceFactory::SurfaceFactory^ m_surfaceFactory;
    };

    [Windows::Foundation::Metadata::WebHostHiddenAttribute]
    public ref class TextSurface sealed
    {
    public:
        property Compositor^ Compositor { WUC::Compositor^ get() { return m_surfaceFactory->Compositor; } }
        property SurfaceFactory^ SurfaceFactory { CompositionSurfaceFactory::SurfaceFactory^ get() { return m_surfaceFactory; }}
        property ICompositionSurface^ Surface { ICompositionSurface^ get() { return m_surface; }}
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

        property Platform::String^ Text
        {
            Platform::String^ get() { return m_text; }
            void set(Platform::String^ value)
            {
                m_text = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property float Width
        {
            float get() { return m_width; };
            void set(float value)
            {
                m_width = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property float Height
        {
            float get() { return m_height; }
            void set(float value)
            {
                m_height = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property Platform::String^ FontFamily
        {
            Platform::String^ get() { return m_textFormat->FontFamily; }
            void set(Platform::String^ value)
            {
                m_textFormat->FontFamily = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property float FontSize
        {
            float get() { return m_textFormat->FontSize; }
            void set(float value)
            {
                m_textFormat->FontSize = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property FontStyle FontStyle
        {
            WUT::FontStyle get() { return m_textFormat->FontStyle; }
            void set(WUT::FontStyle value)
            {
                m_textFormat->FontStyle = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property TextHorizontalAlignment HorizontalAlignment
        {
            TextHorizontalAlignment get() { return GetTextHorizontalAlignment(m_textFormat->HorizontalAlignment); }
            void set(TextHorizontalAlignment value)
            {
                m_textFormat->HorizontalAlignment = GetCanvasHorizontalAlignment(value);
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property TextVerticalAlignment VerticalAlignment
        {
            TextVerticalAlignment get() { return GetTextVerticalAlignment(m_textFormat->VerticalAlignment); }
            void set(TextVerticalAlignment value)
            {
                m_textFormat->VerticalAlignment = GetCanvasVerticalAlignment(value);
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property WordWrapping WordWrapping
        {
            CompositionSurfaceFactory::WordWrapping get() { return GetWordWrapping(m_textFormat->WordWrapping); }
            void set(CompositionSurfaceFactory::WordWrapping value)
            {
                m_textFormat->WordWrapping = GetCanvasWordWrapping(value);
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        property Padding Padding
        {
            CompositionSurfaceFactory::Padding get() { return m_padding; }
            void set(CompositionSurfaceFactory::Padding value)
            {
                m_padding = value;
                RedrawSurface();
            }
        }

        property Windows::UI::Color ForegroundColor
        {
            Windows::UI::Color get() { return m_foregroundColor; }
            void set(Windows::UI::Color value)
            {
                m_foregroundColor = value;
                RedrawSurface();
            }
        }

        property Windows::UI::Color BackgroundColor
        {
            Windows::UI::Color get() { return m_backgroundColor; }
            void set(Windows::UI::Color value)
            {
                m_backgroundColor = value;
                RedrawSurface();
            }
        }

        void RedrawSurface();
        event EventHandler<TextSurfaceRedrawnEventArgs^>^ SurfaceRedrawn;

        virtual ~TextSurface();
    private:
        static CanvasHorizontalAlignment GetCanvasHorizontalAlignment(TextHorizontalAlignment value);
        static TextHorizontalAlignment GetTextHorizontalAlignment(CanvasHorizontalAlignment value);
        static CanvasVerticalAlignment GetCanvasVerticalAlignment(TextVerticalAlignment value);
        static TextVerticalAlignment GetTextVerticalAlignment(CanvasVerticalAlignment value);
        static CanvasWordWrapping GetCanvasWordWrapping(CompositionSurfaceFactory::WordWrapping value);
        static CompositionSurfaceFactory::WordWrapping GetWordWrapping(CanvasWordWrapping value);

        TextSurface(CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory, 
                    Platform::String^ text);
        TextSurface(CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory,
                    Platform::String^ text,
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

        void Initialize(float width,
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
        void Uninitialize();

        void OnDeviceReplacedEvent(Platform::Object ^sender, Windows::UI::Composition::RenderingDeviceReplacedEventArgs ^args);
        void UpdateTextLayout();
    internal:
        static TextSurface^ Create(CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory,
                                   Platform::String^ text);
        static TextSurface^ Create(CompositionSurfaceFactory::SurfaceFactory^ surfaceFactory,
                                   Platform::String^ text,
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
    private:
        CompositionSurfaceFactory::SurfaceFactory^ m_surfaceFactory;
        CompositionDrawingSurface^ m_surface;
        Platform::String^ m_text;
        float m_width;
        float m_height;
        CanvasTextFormat^ m_textFormat;
        CompositionSurfaceFactory::Padding m_padding;
        Windows::UI::Color m_foregroundColor;
        Windows::UI::Color m_backgroundColor;
        CanvasTextLayout^ m_textLayout;

		Windows::Foundation::EventRegistrationToken OnDeviceReplacedHandler;
    };
}
}