#include "pch.h"
#include "SurfaceFactory.h"
#include "UriSurface.h"
#include "Lock.h"
#include "SurfaceUtilities.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

namespace CSF = Robmikh::CompositionSurfaceFactory;

UriSurface^ UriSurface::Create(
    CSF::SurfaceFactory^ surfaceFactory,
    Uri^ uri,
    WF::Size size,
    CSF::InterpolationMode interpolation)
{
    return ref new UriSurface(surfaceFactory, uri, size, interpolation);
}

UriSurface::UriSurface(
    CSF::SurfaceFactory^ surfaceFactory,
    Uri^ uri, 
    WF::Size size,
    CSF::InterpolationMode interpolation) : 
    m_surfaceFactory(surfaceFactory), 
    m_uri(uri),
    m_interpolationMode(interpolation),
    m_desiredSize(size)
{
    m_surface = m_surfaceFactory->CreateSurface(m_desiredSize);

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
    auto ignored = RedrawSurfaceAsync();
}

IAsyncAction^ UriSurface::RedrawSurfaceAsync()
{
    return RedrawSurfaceAsync(m_uri);
}

IAsyncAction^ UriSurface::RedrawSurfaceAsync(Uri^ uri)
{
    return RedrawSurfaceAsync(uri, m_desiredSize);
}

IAsyncAction^ UriSurface::RedrawSurfaceAsync(Uri^ uri, WF::Size size)
{
    return RedrawSurfaceAsync(uri, size, m_interpolationMode);
}

IAsyncAction^ UriSurface::RedrawSurfaceAsync(Uri^ uri, WF::Size size, CSF::InterpolationMode interpolation)
{
	m_uri = uri;
	m_interpolationMode = interpolation;
    m_desiredSize = size;

	if (m_uri != nullptr)
	{
		return SurfaceUtilities::FillSurfaceWithUriAsync(m_surfaceFactory, m_surface, m_uri, m_desiredSize, m_interpolationMode);
	}
	else
	{
		return concurrency::create_async([=]()
		{
			SurfaceUtilities::FillSurfaceWithColor(m_surfaceFactory, m_surface, Windows::UI::Colors::Transparent, m_desiredSize);
		});
	}
}

void UriSurface::Resize(WF::Size size)
{
	m_surfaceFactory->ResizeSurface(m_surface, size);
}

void UriSurface::Uninitialize()
{
    m_surfaceFactory->DeviceReplaced -= OnDeviceReplacedHandler;
    m_surface->~CompositionDrawingSurface();
    m_surface = nullptr;
    m_surfaceFactory = nullptr;
    m_uri = nullptr;
}