#pragma once

#include "DeviceLostHelper.g.h"

namespace winrt::Robmikh::CompositionSurfaceFactory::implementation
{
    struct DeviceLostHelper : DeviceLostHelperT<DeviceLostHelper>
    {
        DeviceLostHelper() = default;
        ~DeviceLostHelper() { StopWatchingCurrentDevice(); OnDeviceLostHandler = NULL; }

        Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice CurrentlyWatchedDevice() { return m_device; }
        void WatchDevice(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device);
        void StopWatchingCurrentDevice();
        winrt::event_token DeviceLost(Windows::Foundation::EventHandler<Robmikh::CompositionSurfaceFactory::DeviceLostEventArgs> const& handler) { return m_deviceLostEvent.add(handler); }
        void DeviceLost(winrt::event_token const& token) noexcept { m_deviceLostEvent.remove(token); }

    private:
        void RaiseDeviceLostEvent(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& oldDevice);

        static void CALLBACK OnDeviceLost(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WAIT wait, TP_WAIT_RESULT waitResult);
    private:
        Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device{ nullptr };
        winrt::event<Windows::Foundation::EventHandler<Robmikh::CompositionSurfaceFactory::DeviceLostEventArgs>> m_deviceLostEvent;

        PTP_WAIT OnDeviceLostHandler = NULL;
        HANDLE m_eventHandle = NULL;
        DWORD m_cookie = NULL;
    };
}

namespace winrt::Robmikh::CompositionSurfaceFactory::factory_implementation
{
    struct DeviceLostHelper : DeviceLostHelperT<DeviceLostHelper, implementation::DeviceLostHelper>
    {
    };
}
