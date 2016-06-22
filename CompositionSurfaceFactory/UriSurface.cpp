#include "pch.h"
#include "SurfaceFactory.h"
#include "UriSurface.h"
#include "SharedLock.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

namespace CSF = Robmikh::CompositionSurfaceFactory;

UriSurface^ UriSurface::Create(
    CSF::SurfaceFactory^ surfaceFactory,
    Uri^ uri,
    WF::Size size)
{
    return ref new UriSurface(surfaceFactory, uri, size);
}

UriSurface::UriSurface(
    CSF::SurfaceFactory^ surfaceFactory,
    Uri^ uri, 
    WF::Size size) : 
    m_surfaceFactory(surfaceFactory), 
    m_uri(uri)
{
    m_surface = m_surfaceFactory->CreateSurface(size);

    OnDeviceReplacedHandler = 
        m_surfaceFactory->DeviceReplaced += 
            ref new EventHandler<RenderingDeviceReplacedEventArgs ^>(this, &UriSurface::OnDeviceReplacedEvent);
}

UriSurface::~UriSurface()
{
    Uninitialize();
}

void UriSurface::OnDeviceReplacedEvent(Object ^sender, RenderingDeviceReplacedEventArgs ^args)
{
    OutputDebugString(L"CompositionSurfaceFactory - Redrawing UriSurface from Device Replaced");
    auto ignored = RedrawSurface();
}

IAsyncAction^ UriSurface::RedrawSurface()
{
    return concurrency::create_async([this]() -> concurrency::task<void>
    {
        return RedrawSurfaceWorker(m_uri, WF::Size::Empty);
    });
}

IAsyncAction^ UriSurface::RedrawSurface(Uri^ uri)
{
    return concurrency::create_async([this, uri]() -> concurrency::task<void>
    {
        return RedrawSurfaceWorker(uri, WF::Size::Empty);
    });
}

IAsyncAction^ UriSurface::RedrawSurface(Uri^ uri, WF::Size size)
{
    return concurrency::create_async([this, uri, size]() -> concurrency::task<void>
    {
        return RedrawSurfaceWorker(uri, size);
    });
}

void UriSurface::Resize(WF::Size size)
{
    auto lock = m_surfaceFactory->DrawingLock;

    lock->Lock(ref new SharedLockWork([=]() mutable
    {
        CanvasComposition::Resize(m_surface, size);
    }));
}

concurrency::task<void> UriSurface::RedrawSurfaceWorker(Uri^ uri, WF::Size size) __resumable
{
    m_uri = uri;
    __await m_surfaceFactory->DrawSurface(m_surface, m_uri, size);
}

void UriSurface::Uninitialize()
{
    m_surfaceFactory->DeviceReplaced -= OnDeviceReplacedHandler;
    m_surface->~CompositionDrawingSurface();
    m_surface = nullptr;
    m_surfaceFactory = nullptr;
    m_uri = nullptr;
}