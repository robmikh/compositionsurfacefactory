# CompositionImageLoader
CompositionImageLoader is an image loader to use with the Windows.UI.Composition api based on Win2D and written with C#.

## Nuget Package
You can download the latest nuget package for CompositionImageLoader using this command:
```
PM>  Install-Package Robmikh.Util.CompositionImageLoader -Pre 
```
Learn more [here](https://www.nuget.org/packages/Robmikh.Util.CompositionImageLoader).

## Usage
Loading images into a surface is easy using CompositionImageLoader:
```
var imageLoader = ImageLoaderFactory.CreateImageLoader(_compositor);
var surface = imageLoader.LoadImageFromUri(new Uri("ms-appx:///Assets/Images/testimage.png"));
```
You can also pass in a CompositionGraphicsDevice instead of a Compositor if you already have a device you would like the ImageLoader to use.
```
var imageLoader = ImageLoaderFactory.CreateImageLoader(_compositionGraphicsDevice);
var surface = await imageLoader.LoadImageFromUriAsync(new Uri("ms-appx:///Assets/Images/testimage.png"));
```

### LoadImageFromUri vs LoadImageFromUriAsync
LoadImageFromUri will immediately return a surface that is initially be empty. Later on a thread pool thread, the surface will be resized and filled in with the decoded image. This means that the call is synchronous but the image won't actually be loaded until a later time.

LoadImageFromUriAsync will asynchronously create a surface and decode the image at the same time. This means that when you get the surface back it is guaranteed to be filled in.

### ManagedSurface
ManagedSurface is just a wrapper to the CompositionDrawingSurface class which handles device lost events raised by the ImageLoader. Using it is similar to LoadImageFromUri(Async):
```
var managedSurface = _imageLoader.CreateManagedSurfaceFromUri(new Uri("ms-appx:///Assets/Images/testimage.png"));
//
// Consuming the real surface
//
visual.Brush = _compositor.CreateSurfaceBrush(managedSurface.Surface);
```
When you're done with the ManagedSurface, just call Dispose() to let it unregister from ImageLoader's DeviceReplaced event. It is important to note that a ManagedSurface is "tethered" to an ImageLoader, as it is dependent on the ImageLoader to be able to redraw its surface. If you call Dispose() on the ImageLoader, any ManagedSurfaces that were created with that ImageLoader won't be able to redraw and will throw an exception when asked.

You can also ask a ManagedSurface to redraw its contents at any time, and even provide a new Uri for it to use (and hold onto for the purposes of DeviceLost events).

### Device Lost and Device Lifetime
A device lost event is exposed to the caller through the ImageLoader. When notified of a device lost, the caller should ask the ImageLoader to redraw any needed surfaces. MangedSurface will do this for you if used.

When the ImageLoader is created with only a Compositor, it will create the devices itself. As such, the ImageLoader will be responsible for device lifetime, which includes creating and disposing of the device. However, if a CompositoinGraphicsDevice is given to the ImageLoader instead, then the caller is responsible for the device's lifetime.
