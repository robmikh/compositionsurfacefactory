#pragma once
#include "SurfaceFactory.h"

namespace Robmikh
{
namespace CompositionSurfaceFactory
{
	[Windows::Foundation::Metadata::WebHostHiddenAttribute]
	public ref class SurfaceUtilities sealed
	{
	public:
		static void FillSurfaceWithColor(
			SurfaceFactory^ surfaceFactory, 
			CompositionDrawingSurface^ surface, 
			Windows::UI::Color color, 
			Size size);

		static void FillSurfaceWithDirect3DSurface(
			SurfaceFactory^ surfaceFactory, 
			CompositionDrawingSurface^ surface, 
			IDirect3DSurface^ direct3DSurface, 
			Size size, 
			InterpolationMode interpolation);

		static IAsyncAction^ FillSurfaceWithUriAsync(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			Uri^ uri,
			Size size,
			InterpolationMode interpolation);

		static void FillSurfaceWithBytes(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			const Platform::Array<byte>^ bytes,
			int widthInPixels,
			int heightInPixels,
			Size size,
			InterpolationMode interpolation);

		//
		// Overloads
		//

		// FillSurfaceWithColor
		static void FillSurfaceWithColor(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			Windows::UI::Color color)
		{
			FillSurfaceWithColor(surfaceFactory, surface, color, Size::Empty);
		}

		// FillSurfaceWithDirect3DSurface
		static void FillSurfaceWithDirect3DSurface(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			IDirect3DSurface^ direct3DSurface,
			Size size)
		{
			FillSurfaceWithDirect3DSurface(surfaceFactory, surface, direct3DSurface, size, InterpolationMode::Linear);
		}

		static void FillSurfaceWithDirect3DSurface(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			IDirect3DSurface^ direct3DSurface)
		{
			FillSurfaceWithDirect3DSurface(surfaceFactory, surface, direct3DSurface, Size::Empty);
		}

		// FillSurfaceWithUriAsync
		static IAsyncAction^ FillSurfaceWithUriAsync(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			Uri^ uri,
			Size size)
		{
			return FillSurfaceWithUriAsync(surfaceFactory, surface, uri, size, InterpolationMode::Linear);
		}

		static IAsyncAction^ FillSurfaceWithUriAsync(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			Uri^ uri)
		{
			return FillSurfaceWithUriAsync(surfaceFactory, surface, uri, Size::Empty);
		}

		// FillSurfaceWithBytes
		static void FillSurfaceWithBytes(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			const Platform::Array<byte>^ bytes,
			int widthInPixels,
			int heightInPixels,
			Size size)
		{
			FillSurfaceWithBytes(surfaceFactory, surface, bytes, widthInPixels, heightInPixels, size, InterpolationMode::Linear);
		}

		static void FillSurfaceWithBytes(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			const Platform::Array<byte>^ bytes,
			int widthInPixels,
			int heightInPixels)
		{
			FillSurfaceWithBytes(surfaceFactory, surface, bytes, widthInPixels, heightInPixels, Size::Empty);
		}

	internal:
		static void FillSurfaceWithCanvasBitmap(
			SurfaceFactory^ surfaceFactory,
			CompositionDrawingSurface^ surface,
			CanvasBitmap^ bitmap,
			Size size,
			InterpolationMode interpolation);

	private:
		SurfaceUtilities();
	};
}
}