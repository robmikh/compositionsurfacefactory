#pragma once

#include "DeviceLostEventArgs.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct DeviceLostEventArgs : DeviceLostEventArgsT<DeviceLostEventArgs>
    {
        DeviceLostEventArgs(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device);

        Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice Device();

    private:
        Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device{ nullptr };
    };
}
