#include "pch.h"
#include "DeviceLostEventArgs.h"

using namespace winrt;
using namespace Windows::Graphics::DirectX::Direct3D11;

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    DeviceLostEventArgs::DeviceLostEventArgs(IDirect3DDevice device)
    {
        m_device = device;
    }

    IDirect3DDevice DeviceLostEventArgs::Device()
    {
        return m_device;
    }
}
