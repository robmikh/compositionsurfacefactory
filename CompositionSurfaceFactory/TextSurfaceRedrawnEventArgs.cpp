#include "pch.h"
#include "TextSurfaceRedrawnEventArgs.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    TextSurfaceRedrawnEventArgs::TextSurfaceRedrawnEventArgs(
        API::TextSurface const& surface,
        API::SurfaceFactory const& surfaceFactory)
    {
        m_surface = surface;
        m_surfaceFactory = surfaceFactory;
    }
}
