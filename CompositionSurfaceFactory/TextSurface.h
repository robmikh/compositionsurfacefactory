#pragma once

#include "TextSurface.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct TextSurface : TextSurfaceT<TextSurface>
    {
        TextSurface(
            API::SurfaceFactory const& surfaceFactory,
            hstring const& text,
            float width,
            float height,
            hstring const& fontFamily,
            float fontSize,
            Windows::UI::Text::FontStyle const& fontStyle,
            API::TextHorizontalAlignment const& horizontalAlignment,
            API::TextVerticalAlignment const& verticalAlignment,
            API::WordWrapping const& wordWrapping,
            API::Padding const& padding,
            Windows::UI::Color const& foregroundColor,
            Windows::UI::Color const& backgroundColor);
        ~TextSurface() { Close(); }

        Windows::UI::Composition::Compositor Compositor() { CheckClosed(); return m_surfaceFactory.Compositor(); }
        Robmikh::CompositionSurfaceFactory::SurfaceFactory SurfaceFactory() { CheckClosed(); return m_surfaceFactory; }
        Windows::UI::Composition::ICompositionSurface Surface() { CheckClosed(); return m_surface; }
        Windows::Foundation::Size Size() { CheckClosed(); return m_surface == nullptr ? Windows::Foundation::Size{ 0, 0 } : m_surface.Size(); }
        hstring Text() { CheckClosed(); return m_text; }
        void Text(hstring const& value) { CheckClosed(); m_text = value; UpdateTextLayout(); RedrawSurface(); }
        float Width() { CheckClosed(); return m_width; }
        void Width(float value) { CheckClosed(); m_width = value; UpdateTextLayout(); RedrawSurface(); }
        float Height() { CheckClosed(); return m_height; }
        void Height(float value) { CheckClosed(); m_height = value; UpdateTextLayout(); RedrawSurface(); }
        hstring FontFamily() { CheckClosed(); return m_textFormat.FontFamily(); }
        void FontFamily(hstring const& value) { CheckClosed(); m_textFormat.FontFamily(value); UpdateTextLayout(); RedrawSurface(); }
        float FontSize() { CheckClosed(); return m_textFormat.FontSize(); }
        void FontSize(float value) { CheckClosed(); m_textFormat.FontSize(value); UpdateTextLayout(); RedrawSurface(); }
        Windows::UI::Text::FontStyle FontStyle() { CheckClosed(); return m_textFormat.FontStyle(); }
        void FontStyle(Windows::UI::Text::FontStyle const& value) { CheckClosed(); m_textFormat.FontStyle(value); UpdateTextLayout(); RedrawSurface(); }
        Robmikh::CompositionSurfaceFactory::TextHorizontalAlignment HorizontalAlignment() { CheckClosed(); return static_cast<API::TextHorizontalAlignment>(m_textFormat.HorizontalAlignment()); }
        void HorizontalAlignment(Robmikh::CompositionSurfaceFactory::TextHorizontalAlignment const& value) { CheckClosed(); m_textFormat.HorizontalAlignment(static_cast<Microsoft::Graphics::Canvas::Text::CanvasHorizontalAlignment>(value)); UpdateTextLayout(); RedrawSurface(); }
        Robmikh::CompositionSurfaceFactory::TextVerticalAlignment VerticalAlignment() { CheckClosed(); return static_cast<API::TextVerticalAlignment>(m_textFormat.VerticalAlignment()); }
        void VerticalAlignment(Robmikh::CompositionSurfaceFactory::TextVerticalAlignment const& value) { CheckClosed(); m_textFormat.VerticalAlignment(static_cast<Microsoft::Graphics::Canvas::Text::CanvasVerticalAlignment>(value)); UpdateTextLayout(); RedrawSurface(); }
        Robmikh::CompositionSurfaceFactory::WordWrapping WordWrapping() { CheckClosed(); return static_cast<API::WordWrapping>(m_textFormat.WordWrapping()); }
        void WordWrapping(Robmikh::CompositionSurfaceFactory::WordWrapping const& value) { CheckClosed(); m_textFormat.WordWrapping(static_cast<Microsoft::Graphics::Canvas::Text::CanvasWordWrapping>(value)); UpdateTextLayout(); RedrawSurface(); }
        Robmikh::CompositionSurfaceFactory::Padding Padding() { CheckClosed(); return m_padding; }
        void Padding(Robmikh::CompositionSurfaceFactory::Padding const& value) { CheckClosed(); m_padding = value; RedrawSurface(); }
        Windows::UI::Color ForegroundColor() { CheckClosed(); return m_foregroundColor; }
        void ForegroundColor(Windows::UI::Color const& value) { CheckClosed(); m_foregroundColor = value; RedrawSurface(); }
        Windows::UI::Color BackgroundColor() { CheckClosed(); return m_backgroundColor; }
        void BackgroundColor(Windows::UI::Color const& value) { CheckClosed(); m_backgroundColor = value; RedrawSurface(); }
        void RedrawSurface();
        winrt::event_token SurfaceRedrawn(Windows::Foundation::EventHandler<Robmikh::CompositionSurfaceFactory::TextSurfaceRedrawnEventArgs> const& handler) { return m_surfaceRedrawnEvent.add(handler); }
        void SurfaceRedrawn(winrt::event_token const& token) noexcept { m_surfaceRedrawnEvent.remove(token); }
        void Close();

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

        void UpdateTextLayout();
        void OnDeviceReplaced(winrt::Windows::Foundation::IInspectable const& sender, Windows::UI::Composition::RenderingDeviceReplacedEventArgs const& args);

    private:
        API::SurfaceFactory m_surfaceFactory{ nullptr };
        Windows::UI::Composition::CompositionDrawingSurface m_surface{ nullptr };
        hstring m_text;
        float m_width;
        float m_height;
        Microsoft::Graphics::Canvas::Text::CanvasTextFormat m_textFormat;
        API::Padding m_padding;
        Windows::UI::Color m_foregroundColor;
        Windows::UI::Color m_backgroundColor;
        Microsoft::Graphics::Canvas::Text::CanvasTextLayout m_textLayout{ nullptr };

        winrt::event<Windows::Foundation::EventHandler<Robmikh::CompositionSurfaceFactory::TextSurfaceRedrawnEventArgs>> m_surfaceRedrawnEvent;
        API::SurfaceFactory::DeviceReplaced_revoker m_deviceReplaced;
        std::atomic<bool> m_closed = false;
    };
}
