#include "pch.h"
#include "SurfaceFactory.h"
#include "TextSurface.h"
#include "SharedLock.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

namespace CSF = Robmikh::CompositionSurfaceFactory;

TextSurfaceRedrawnEventArgs^ TextSurfaceRedrawnEventArgs::Create(TextSurface^ surface, CSF::SurfaceFactory^ surfaceFactory)
{
    return ref new TextSurfaceRedrawnEventArgs(surface, surfaceFactory);
}

TextSurfaceRedrawnEventArgs::TextSurfaceRedrawnEventArgs(
    TextSurface^ surface,
    CSF::SurfaceFactory^ surfaceFactory) :
    m_surface(surface),
    m_surfaceFactory(surfaceFactory)
{
}

TextSurface^ TextSurface::Create(CSF::SurfaceFactory^ surfaceFactory,
                                 Platform::String^ text)
{
    return ref new TextSurface(surfaceFactory, text);
}

TextSurface^ TextSurface::Create(CSF::SurfaceFactory^ surfaceFactory,
                                 Platform::String^ text,
                                 float width,
                                 float height,
                                 Platform::String^ fontFamily,
                                 float fontSize,
                                 WUT::FontStyle fontStyle,
                                 TextHorizontalAlignment horizontalAlignment,
                                 TextVerticalAlignment verticalAlignment,
                                 CSF::WordWrapping wordWrapping,
                                 CSF::Padding padding,
                                 Windows::UI::Color foregroundColor,
                                 Windows::UI::Color backgroundColor)
{
    return ref new TextSurface(surfaceFactory, 
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
}

CanvasHorizontalAlignment TextSurface::GetCanvasHorizontalAlignment(TextHorizontalAlignment value)
{
    switch (value)
    {
    case TextHorizontalAlignment::Left:
        return CanvasHorizontalAlignment::Left;
    case TextHorizontalAlignment::Right:
        return CanvasHorizontalAlignment::Right;
    case TextHorizontalAlignment::Center:
        return CanvasHorizontalAlignment::Center;
    case TextHorizontalAlignment::Justified:
        return CanvasHorizontalAlignment::Justified;
    default:
        return CanvasHorizontalAlignment::Left;
    }
}

TextHorizontalAlignment TextSurface::GetTextHorizontalAlignment(CanvasHorizontalAlignment value)
{
    switch (value)
    {
    case CanvasHorizontalAlignment::Left:
        return TextHorizontalAlignment::Left;
    case CanvasHorizontalAlignment::Right:
        return TextHorizontalAlignment::Right;
    case CanvasHorizontalAlignment::Center:
        return TextHorizontalAlignment::Center;
    case CanvasHorizontalAlignment::Justified:
        return TextHorizontalAlignment::Justified;
    default:
        return TextHorizontalAlignment::Left;
    }
}

CanvasVerticalAlignment TextSurface::GetCanvasVerticalAlignment(TextVerticalAlignment value)
{
    switch (value)
    {
    case TextVerticalAlignment::Top:
        return CanvasVerticalAlignment::Top;
    case TextVerticalAlignment::Center:
        return CanvasVerticalAlignment::Center;
    case TextVerticalAlignment::Bottom:
        return CanvasVerticalAlignment::Bottom;
    default:
        return CanvasVerticalAlignment::Top;
    }
}

TextVerticalAlignment TextSurface::GetTextVerticalAlignment(CanvasVerticalAlignment value)
{
    switch (value)
    {
    case CanvasVerticalAlignment::Top:
        return TextVerticalAlignment::Top;
    case CanvasVerticalAlignment::Center:
        return TextVerticalAlignment::Center;
    case CanvasVerticalAlignment::Bottom:
        return TextVerticalAlignment::Bottom;
    default:
        return TextVerticalAlignment::Top;
    }
}

CanvasWordWrapping TextSurface::GetCanvasWordWrapping(CSF::WordWrapping value)
{
    switch (value)
    {
    case CSF::WordWrapping::Character:
        return CanvasWordWrapping::Character;
    case CSF::WordWrapping::EmergencyBreak:
        return CanvasWordWrapping::EmergencyBreak;
    case CSF::WordWrapping::NoWrap:
        return CanvasWordWrapping::NoWrap;
    case CSF::WordWrapping::WholeWord:
        return CanvasWordWrapping::WholeWord;
    case CSF::WordWrapping::Wrap:
        return CanvasWordWrapping::Wrap;
    default:
        return CanvasWordWrapping::NoWrap;
    }
}

CSF::WordWrapping TextSurface::GetWordWrapping(CanvasWordWrapping value)
{
    switch (value)
    {
    case CanvasWordWrapping::Character:
        return CSF::WordWrapping::Character;
    case CanvasWordWrapping::EmergencyBreak:
        return CSF::WordWrapping::EmergencyBreak;
    case CanvasWordWrapping::NoWrap:
        return CSF::WordWrapping::NoWrap;
    case CanvasWordWrapping::WholeWord:
        return CSF::WordWrapping::WholeWord;
    case CanvasWordWrapping::Wrap:
        return CSF::WordWrapping::Wrap;
    default:
        return CSF::WordWrapping::NoWrap;
    }
}

TextSurface::TextSurface(CSF::SurfaceFactory^ surfaceFactory,
                         Platform::String^ text) :
                        m_surfaceFactory(surfaceFactory),
                        m_text(text)
{
    Initialize(0, 
               0, 
               L"Segoe UI", 
               14.0f, 
               WUT::FontStyle::Normal, 
               TextHorizontalAlignment::Left, 
               TextVerticalAlignment::Top, 
               CSF::WordWrapping::NoWrap, 
               { 0, 0, 0, 0 }, 
               Windows::UI::Colors::Black, 
               Windows::UI::Colors::Transparent);
}

TextSurface::TextSurface(CSF::SurfaceFactory^ surfaceFactory,
                         Platform::String^ text,
                         float width,
                         float height,
                         Platform::String^ fontFamily,
                         float fontSize,
                         WUT::FontStyle fontStyle,
                         TextHorizontalAlignment horizontalAlignment,
                         TextVerticalAlignment verticalAlignment,
                         CSF::WordWrapping wordWrapping,
                         CSF::Padding padding,
                         Windows::UI::Color foregroundColor,
                         Windows::UI::Color backgroundColor) :
                        m_surfaceFactory(surfaceFactory),
                        m_text(text)
{
    Initialize(width,
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
}

TextSurface::~TextSurface()
{
    Uninitialize();
}

void TextSurface::Initialize(float width,
                             float height,
                             Platform::String^ fontFamily,
                             float fontSize,
                             WUT::FontStyle fontStyle,
                             TextHorizontalAlignment horizontalAlignment,
                             TextVerticalAlignment verticalAlignment,
                             CSF::WordWrapping wordWrapping,
                             CSF::Padding padding,
                             Windows::UI::Color foregroundColor,
                             Windows::UI::Color backgroundColor)
{
    // We initialize the surface to the size of 1x1 becuase
    // we can't create a drawing session from an empty surface
    // and we need the session to create our first TextLayout.
    m_surface = m_surfaceFactory->CreateSurface({ 1, 1 });
    m_textFormat = ref new CanvasTextFormat();

    m_width = width;
    m_height = height;
    m_padding = padding;
    m_foregroundColor = foregroundColor;
    m_backgroundColor = backgroundColor;

    m_textFormat->FontFamily = fontFamily;
    m_textFormat->FontSize = fontSize;
    m_textFormat->FontStyle = fontStyle;
    m_textFormat->HorizontalAlignment = GetCanvasHorizontalAlignment(horizontalAlignment);
    m_textFormat->VerticalAlignment = GetCanvasVerticalAlignment(verticalAlignment);
    m_textFormat->WordWrapping = GetCanvasWordWrapping(wordWrapping);

    UpdateTextLayout();

    OnDeviceReplacedHandler = m_surfaceFactory->DeviceReplaced 
        += ref new EventHandler<RenderingDeviceReplacedEventArgs ^>(this, &TextSurface::OnDeviceReplacedEvent);
}

void TextSurface::OnDeviceReplacedEvent(Object ^sender, RenderingDeviceReplacedEventArgs ^args)
{
    OutputDebugString(L"CompositionSurfaceFactory - Redrawing TextSurface from Device Replaced");
    RedrawSurface();
}

void TextSurface::UpdateTextLayout()
{
    auto lock = m_surfaceFactory->DrawingLock;

    lock->Lock(ref new SharedLockWork([=]() mutable
    {
        auto session = CanvasComposition::CreateDrawingSession(m_surface);
        m_textLayout = ref new CanvasTextLayout(session, m_text, m_textFormat, m_width, m_height);
    }));
}

void TextSurface::RedrawSurface()
{
    auto lock = m_surfaceFactory->DrawingLock;

    lock->Lock(ref new SharedLockWork([=]() mutable
    {
        float width = (float)(m_textLayout->DrawBounds.Left + m_textLayout->DrawBounds.Right) + m_padding.Left + m_padding.Right;
        float height = (float)(m_textLayout->DrawBounds.Top + m_textLayout->DrawBounds.Bottom) + m_padding.Top + m_padding.Bottom;
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
        session->Clear(m_backgroundColor);
        session->DrawTextLayout(m_textLayout, m_padding.Left, m_padding.Right, m_foregroundColor);
    }));

    auto args = TextSurfaceRedrawnEventArgs::Create(this, m_surfaceFactory);
    concurrency::create_async([this, args]
    {
        SurfaceRedrawn(this, args);
    });
}

void TextSurface::Uninitialize()
{
    m_surfaceFactory->DeviceReplaced -= OnDeviceReplacedHandler;
    m_surface->~CompositionDrawingSurface();
    m_surface = nullptr;
    m_surfaceFactory = nullptr;
    m_textFormat = nullptr;
    m_textLayout = nullptr;
}