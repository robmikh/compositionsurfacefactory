#include "pch.h"
#include "TextSurface.h"
#include "TextSurfaceRedrawnEventArgs.h"

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
    TextSurface::TextSurface(
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
        Color const& foregroundColor, 
        Color const& backgroundColor)
    {
        m_surfaceFactory = surfaceFactory;
        m_text = text;

        // We initialize the surface to the size of 1x1 becuase
        // we can't create a drawing session from an empty surface
        // and we need the session to create our first TextLayout.
        m_surface = m_surfaceFactory.CreateSurface({ 1, 1 });
        m_textFormat = CanvasTextFormat();

        m_width = width;
        m_height = height;
        m_padding = padding;
        m_foregroundColor = foregroundColor;
        m_backgroundColor = backgroundColor;

        m_textFormat.FontFamily(fontFamily);
        m_textFormat.FontSize(fontSize);
        m_textFormat.FontStyle(fontStyle);
        m_textFormat.HorizontalAlignment(static_cast<CanvasHorizontalAlignment>(horizontalAlignment));
        m_textFormat.VerticalAlignment(static_cast<CanvasVerticalAlignment>(verticalAlignment));
        m_textFormat.WordWrapping(static_cast<CanvasWordWrapping>(wordWrapping));

        UpdateTextLayout();

        m_deviceReplaced = m_surfaceFactory.DeviceReplaced(auto_revoke, { this, &TextSurface::OnDeviceReplaced });
    }

    void TextSurface::RedrawSurface()
    {
        CheckClosed();

        auto lock = m_surfaceFactory.DrawingLock();

        {
            auto lockSession = lock.GetLockSession();
            
            // I thought it should be:
            //float width = m_textLayout.DrawBounds().Width + m_padding.Left + m_padding.Right;
            //float height = m_textLayout.DrawBounds().Height + m_padding.Top + m_padding.Bottom;
            // But it needs to be:
            float width = m_textLayout.DrawBounds().X + m_textLayout.DrawBounds().Width + m_padding.Left + m_padding.Right;
            float height = m_textLayout.DrawBounds().Y + m_textLayout.DrawBounds().Height + m_padding.Top + m_padding.Bottom;
            // The surface can't have any dimmension be 0, so make it at least 1.0f
            if (width < 1.0f)
            {
                width = 1.0f;
            }
            if (height < 1.0f)
            {
                height = 1.0f;
            }
            CanvasComposition::Resize(m_surface, { width, height });
            auto session = CanvasComposition::CreateDrawingSession(m_surface);
            session.Clear(m_backgroundColor);
            session.DrawTextLayout(m_textLayout, m_padding.Left, m_padding.Right, m_foregroundColor);
        }

        if (m_surfaceRedrawnEvent)
        {
            m_surfaceRedrawnEvent(*this, make<TextSurfaceRedrawnEventArgs>(*this, m_surfaceFactory));
        }
    }

    void TextSurface::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_deviceReplaced.revoke();
            m_surface.Close();
            m_surface = nullptr;
            m_surfaceFactory = nullptr;
            m_text.clear();
            m_textLayout = nullptr;
            m_textFormat = nullptr;
        }
    }

    void TextSurface::UpdateTextLayout()
    {
        auto lock = m_surfaceFactory.DrawingLock();

        {
            auto lockSession = lock.GetLockSession();
            auto session = CanvasComposition::CreateDrawingSession(m_surface);
            m_textLayout = CanvasTextLayout(session, m_text, m_textFormat, m_width, m_height);
        }
    }

    void TextSurface::OnDeviceReplaced(
        winrt::Windows::Foundation::IInspectable const&, 
        RenderingDeviceReplacedEventArgs const&)
    {
        RedrawSurface();
    }
}
