#pragma once

#include "TextSurfaceRedrawnEventArgs.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct TextSurfaceRedrawnEventArgs : TextSurfaceRedrawnEventArgsT<TextSurfaceRedrawnEventArgs>
    {
        TextSurfaceRedrawnEventArgs(
            API::TextSurface const& surface,
            API::SurfaceFactory const& surfaceFactory);

        Robmikh::CompositionSurfaceFactory::TextSurface Surface() { return m_surface; }
        Robmikh::CompositionSurfaceFactory::SurfaceFactory SurfaceFactory() { return m_surfaceFactory; }
    private:
        API::TextSurface m_surface{ nullptr };
        API::SurfaceFactory m_surfaceFactory{ nullptr };
    };
}
