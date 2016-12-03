#include "pch.h"
#include "SurfaceFactory.h"
#include "SurfaceUtilities.h"
#include "Lock.h"

using namespace Robmikh::CompositionSurfaceFactory;
using namespace Platform;

namespace CSF = Robmikh::CompositionSurfaceFactory;

SurfaceUtilities::SurfaceUtilities() { }

void SurfaceUtilities::FillSurfaceWithColor(
	SurfaceFactory^ surfaceFactory, 
	CompositionDrawingSurface^ surface, 
	Windows::UI::Color color, 
	Size size)
{
	auto lock = surfaceFactory->DrawingLock;

	// Make the size at least 1x1
	if (size.IsEmpty)
	{
		size = { 1, 1 };
	}

	// Resize the surface
	surfaceFactory->ResizeSurface(surface, size);

	// Clear the surface with the desired color
	// Because the drawing is done asynchronously and multiple threads could
	// be trying to get access to the device/surface at the same time, we need
	// to do any device/surface work under a lock.
	{
		auto lockSession = lock->GetLockSession();

		auto session = CanvasComposition::CreateDrawingSession(surface);
		session->Clear(color);
	}
}

void SurfaceUtilities::FillSurfaceWithDirect3DSurface(
	SurfaceFactory^ surfaceFactory,
	CompositionDrawingSurface^ surface,
	IDirect3DSurface^ direct3DSurface,
	Size size,
	InterpolationMode interpolation)
{
	auto lock = surfaceFactory->DrawingLock;
	auto canvasDevice = CanvasComposition::GetCanvasDevice(surfaceFactory->GraphicsDevice);

	auto bitmap = CanvasBitmap::CreateFromDirect3D11Surface(canvasDevice, direct3DSurface);

	SurfaceUtilities::FillSurfaceWithCanvasBitmap(surfaceFactory, surface, bitmap, size, interpolation);
}

IAsyncAction^ SurfaceUtilities::FillSurfaceWithUriAsync(
	SurfaceFactory^ surfaceFactory,
	CompositionDrawingSurface^ surface,
	Uri^ uri,
	Size size,
	InterpolationMode interpolation)
{
	auto lock = surfaceFactory->DrawingLock;
	auto canvasDevice = CanvasComposition::GetCanvasDevice(surfaceFactory->GraphicsDevice);

	auto bitmapTask = concurrency::create_task(CanvasBitmap::LoadAsync(canvasDevice, uri));

	auto action = bitmapTask.then([=](CanvasBitmap^ bitmap)
	{
		FillSurfaceWithCanvasBitmap(surfaceFactory, surface, bitmap, size, interpolation);
		return;
	});

	return concurrency::create_async([action]
	{
		return action;
	});
}

void SurfaceUtilities::FillSurfaceWithBytes(
	SurfaceFactory^ surfaceFactory,
	CompositionDrawingSurface^ surface,
	const Platform::Array<byte>^ bytes,
	int widthInPixels,
	int heightInPixels,
	Size size,
	InterpolationMode interpolation)
{
	auto lock = surfaceFactory->DrawingLock;
	auto canvasDevice = CanvasComposition::GetCanvasDevice(surfaceFactory->GraphicsDevice);

	auto bitmap = CanvasBitmap::CreateFromBytes(canvasDevice, bytes, widthInPixels, heightInPixels, Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized);

	SurfaceUtilities::FillSurfaceWithCanvasBitmap(surfaceFactory, surface, bitmap, size, interpolation);
}

void SurfaceUtilities::FillSurfaceWithCanvasBitmap(
	SurfaceFactory^ surfaceFactory,
	CompositionDrawingSurface^ surface,
	CanvasBitmap^ bitmap,
	Size size,
	InterpolationMode interpolation)
{
	auto lock = surfaceFactory->DrawingLock;

	// Determine the size of our bitmap
	Size bitmapSize = bitmap->Size;

	// If the caller did not specify a valid size, then make it the same size as the bitmap
	if (size.IsEmpty)
	{
		size = bitmapSize;
	}

	// Resize the surface
	surfaceFactory->ResizeSurface(surface, size);

	// Draw the bitmap into the surface
	// Because the drawing is done asynchronously and multiple threads could
	// be trying to get access to the device/surface at the same time, we need
	// to do any device/surface work under a lock.
	{
		auto lockSession = lock->GetLockSession();

		{
			auto session = CanvasComposition::CreateDrawingSession(surface);
			Rect surfaceRect = { 0, 0, size.Width, size.Height };
			Rect bitmapRect = { 0, 0, bitmapSize.Width, bitmapSize.Height };
			CanvasImageInterpolation canvasInterpolation = InterpolationModeHelper::GetCanvasImageInterpolation(interpolation);
			session->Clear(Windows::UI::Colors::Transparent);
			session->DrawImage(bitmap, surfaceRect, bitmapRect, 1.0f, canvasInterpolation);
		}
	}
}