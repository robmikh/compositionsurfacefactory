#include "pch.h"
#include "SurfaceFactory.h"
#include "Lock.h"
#include "UriSurface.h"
#include "TextSurface.h"
#include "SurfaceFactoryOptions.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;
using namespace Windows::Graphics::Display;

namespace CSF = Robmikh::CompositionSurfaceFactory;

IVector<SurfaceFactory^>^ CSF::SurfaceFactory::s_surfaceFactories = ref new Vector<CSF::SurfaceFactory^>();
Lock^ CSF::SurfaceFactory::s_listLock = ref new Lock();

SurfaceFactory^ SurfaceFactory::GetSharedSurfaceFactoryForCompositor(WUC::Compositor^ compositor)
{
	SurfaceFactory^ result = nullptr;

	// We do this under a lock so that it is safe for multiple callers on different threads. The
	// SurfaceFactory itself may also be changing the list when it is being destroyed.
	{
		auto lockSession = s_listLock->GetLockSession();

		// Look to see if one of the surface factories is associated with the compositor
		for (unsigned int i = 0; i < s_surfaceFactories->Size; i++)
		{
			auto surfaceFactory = s_surfaceFactories->GetAt(i);

			if (surfaceFactory->Compositor == compositor)
			{
				result = surfaceFactory;
				break;
			}
		}

		// If we didn't find one, then make one and add it to the list
		if (result == nullptr)
		{
			result = SurfaceFactory::CreateFromCompositor(compositor);
			s_surfaceFactories->Append(result);
		}
	}

	return result;
}

void SurfaceFactory::ClearSharedSurfaceFactories()
{
	auto copiedList = ref new Vector<SurfaceFactory^>();

	// We do this under a lock so that it is safe for multiple callers on different threads. The
	// SurfaceFactory itself may also be changing the list when it is being destroyed.
	{
		auto lockSession = s_listLock->GetLockSession();

		// Copy the list so that when we are disposing the SurfaceFactory objects they don't
		// screw with the list as we are iterating through it.
		for (unsigned int i = 0; i < s_surfaceFactories->Size; i++)
		{
			auto surfaceFactory = s_surfaceFactories->GetAt(i);

			copiedList->Append(surfaceFactory);
		}
	}


	// Dispose all the SurfaceFactories. This has to be done outside of the lock because the
	// SurfaceFactory will request the lock when disposing, and we would dead lock;
	for (unsigned int i = 0; i < copiedList->Size; i++)
	{
		auto surfaceFactory = copiedList->GetAt(i);

		surfaceFactory->~SurfaceFactory();
	}

	{
		auto lockSession = s_listLock->GetLockSession();
		s_surfaceFactories->Clear();
	}

	copiedList->Clear();
}

SurfaceFactory^ SurfaceFactory::CreateFromCompositor(WUC::Compositor^ compositor)
{
    auto options = SurfaceFactoryOptions();
    options.UseSoftwareRenderer = false;

    auto surfaceFactory = ref new SurfaceFactory(compositor, options);
    return surfaceFactory;
}

SurfaceFactory^ SurfaceFactory::CreateFromCompositor(WUC::Compositor^ compositor, SurfaceFactoryOptions options)
{
    auto surfaceFactory = ref new SurfaceFactory(compositor, options);
    return surfaceFactory;
}

SurfaceFactory^ SurfaceFactory::CreateFromGraphicsDevice(CompositionGraphicsDevice^ graphicsDevice)
{
    auto surfaceFactory = ref new SurfaceFactory(graphicsDevice, nullptr);
    return surfaceFactory;
}

SurfaceFactory^ SurfaceFactory::CreateFromGraphicsDevice(CompositionGraphicsDevice^ graphicsDevice, Lock^ lock)
{
    auto surfaceFactory = ref new SurfaceFactory(graphicsDevice, lock);
    return surfaceFactory;
}

SurfaceFactory::SurfaceFactory(WUC::Compositor^ compositor, SurfaceFactoryOptions options)
{
    m_compositor = compositor;
    m_drawingLock = ref new Lock();
    m_isGraphicsDeviceCreator = true;
	m_deviceLostHelper = ref new DeviceLostHelper();
	OnDeviceLostHandler = m_deviceLostHelper->DeviceLost
		+= ref new EventHandler<DeviceLostEventArgs^>(this, &SurfaceFactory::OnDeviceLost);

    CreateDevice(options);
}

SurfaceFactory::SurfaceFactory(CompositionGraphicsDevice^ graphicsDevice, Lock^ lock)
{
    m_compositor = graphicsDevice->Compositor;
    m_graphicsDevice = graphicsDevice;
    m_isGraphicsDeviceCreator = false;
    if (lock == nullptr)
    {
        m_drawingLock = ref new Lock();
    }
    else
    {
        m_drawingLock = lock;
    }

    OnRenderingDeviceReplacedHandler = m_graphicsDevice->RenderingDeviceReplaced 
        += ref new TypedEventHandler<CompositionGraphicsDevice ^, RenderingDeviceReplacedEventArgs ^>(this, &SurfaceFactory::OnRenderingDeviceReplaced);
}

SurfaceFactory::~SurfaceFactory()
{
    Uninitialize();
}

void SurfaceFactory::OnRenderingDeviceReplaced(CompositionGraphicsDevice ^sender, RenderingDeviceReplacedEventArgs ^args)
{
    OutputDebugString(L"CompositionSurfaceFactory - Rendering Device Replaced");
    concurrency::create_async([this, args]
    {
        this->RaiseDeviceReplacedEvent(args);
    });
}

void SurfaceFactory::RaiseDeviceReplacedEvent(RenderingDeviceReplacedEventArgs ^args)
{
    DeviceReplaced(this, args);
}

void SurfaceFactory::CreateDevice(SurfaceFactoryOptions options)
{
    if (m_compositor != nullptr && m_isGraphicsDeviceCreator)
    {
        if (m_canvasDevice == nullptr)
        {
			m_canvasDevice = ref new CanvasDevice(options.UseSoftwareRenderer);

			m_deviceLostHelper->WatchDevice(m_canvasDevice);
        }

        if (m_graphicsDevice == nullptr)
        {
            m_graphicsDevice = CanvasComposition::CreateCompositionGraphicsDevice(m_compositor, m_canvasDevice);
            OnRenderingDeviceReplacedHandler = m_graphicsDevice->RenderingDeviceReplaced
                += ref new TypedEventHandler<CompositionGraphicsDevice ^, RenderingDeviceReplacedEventArgs ^>(this, &SurfaceFactory::OnRenderingDeviceReplaced);
        }
    }
}

void SurfaceFactory::OnDeviceLost(Platform::Object^ sender, DeviceLostEventArgs^ args)
{
    OutputDebugString(L"CompositionSurfaceFactory - Canvas Device Lost");

	bool softwareRenderer = m_canvasDevice->ForceSoftwareRenderer;

	m_canvasDevice = ref new CanvasDevice(softwareRenderer);
	m_deviceLostHelper->WatchDevice(m_canvasDevice);

    CanvasComposition::SetCanvasDevice(m_graphicsDevice, m_canvasDevice);
}

CompositionDrawingSurface^ SurfaceFactory::CreateSurface(Size size)
{
    Size surfaceSize = size;
    if (surfaceSize.IsEmpty)
    {
        // We start out with a size of 0,0 for the surface, because we don't know
        // the size of the image at this time. We resize the surface later.
        surfaceSize = { 0, 0 };
    }

    CompositionGraphicsDevice^ graphicsDevice = m_graphicsDevice;
    CompositionDrawingSurface^ surface = nullptr;

	{
		auto lockSession = m_drawingLock->GetLockSession();
		surface = graphicsDevice->CreateDrawingSurface(
			surfaceSize,
			Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
			Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
	}

    return surface;
}

void SurfaceFactory::ResizeSurface(CompositionDrawingSurface^ surface, Size size)
{
	{
		auto lockSession = m_drawingLock->GetLockSession();
		CanvasComposition::Resize(surface, size);
	}
}

UriSurface^ SurfaceFactory::CreateUriSurface()
{
	return CreateUriSurface(nullptr);
}

UriSurface^ SurfaceFactory::CreateUriSurface(Uri^ uri)
{
    return CreateUriSurface(uri, Size::Empty);
}

UriSurface^ SurfaceFactory::CreateUriSurface(Uri^ uri, Size size)
{
    return CreateUriSurface(uri, size, InterpolationMode::Linear);
}

UriSurface^ SurfaceFactory::CreateUriSurface(Uri^ uri, Size size, InterpolationMode interpolation)
{
    auto uriSurface = UriSurface::Create(this, uri, size, interpolation);
    auto ignored = uriSurface->RedrawSurfaceAsync();

    return uriSurface;
}

IAsyncOperation<UriSurface^>^ SurfaceFactory::CreateUriSurfaceAsync(Uri^ uri)
{
    return CreateUriSurfaceAsync(uri, Size::Empty);
}

IAsyncOperation<UriSurface^>^ SurfaceFactory::CreateUriSurfaceAsync(Uri^ uri, Size size)
{
    return CreateUriSurfaceAsync(uri, size, InterpolationMode::Linear);
}

IAsyncOperation<UriSurface^>^ SurfaceFactory::CreateUriSurfaceAsync(Uri^ uri, Size size, InterpolationMode interpolation)
{
    return concurrency::create_async([=]() -> UriSurface^
    {
        auto uriSurface = UriSurface::Create(this, uri, size, interpolation);

        auto drawTask = concurrency::create_task(uriSurface->RedrawSurfaceAsync()).then([uriSurface]() -> UriSurface^
        {
            return uriSurface;
        });

        return drawTask.get();
    });
}

TextSurface^ SurfaceFactory::CreateTextSurface()
{
	return CreateTextSurface("");
}

TextSurface^ SurfaceFactory::CreateTextSurface(Platform::String^ text)
{
    auto textSurface = TextSurface::Create(this, text);
    textSurface->RedrawSurface();

    return textSurface;
}

TextSurface^ SurfaceFactory::CreateTextSurface(Platform::String^ text,
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
    auto textSurface = TextSurface::Create(this, 
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
    textSurface->RedrawSurface();

    return textSurface;
}

void SurfaceFactory::Uninitialize()
{
	{
		auto lockSession = m_drawingLock->GetLockSession();

		m_compositor = nullptr;
		if (m_canvasDevice != nullptr)
		{
			m_deviceLostHelper->StopWatchingCurrentDevice();
			m_deviceLostHelper = nullptr;

			m_canvasDevice->~CanvasDevice();
			m_canvasDevice = nullptr;
		}

		if (m_graphicsDevice != nullptr)
		{
			m_graphicsDevice->RenderingDeviceReplaced -= OnRenderingDeviceReplacedHandler;
			// Only dispose the composition graphics device if we own the device.
			if (m_isGraphicsDeviceCreator)
			{
				m_graphicsDevice->~CompositionGraphicsDevice();
			}
			m_graphicsDevice = nullptr;
		}

		{
			auto listLockSession = s_listLock->GetLockSession();
			unsigned int index = 0;
			if (s_surfaceFactories->IndexOf(this, &index))
			{
				s_surfaceFactories->RemoveAt(index);
			}
		}
	}
	m_drawingLock = nullptr;
}