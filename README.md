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
var surface1 = imageLoader.LoadImageFromUri(new Uri("ms-appx:///Assets/Images/testimage.png"));
var surface2 = await imageLoader.LoadImageFromUriAsync(new Uri("ms-appx:///Assets/Images/testimage.png"));
```
You can also pass in a CompositionGraphicsDevice instead of a Compositor if you already have a device you would like the ImageLoader to use.
```
var imageLoader = ImageLoaderFactory.CreateImageLoader(_compositionGraphicsDevice);
var surface1 = imageLoader.LoadImageFromUri(new Uri("ms-appx:///Assets/Images/testimage.png"));
var surface2 = await imageLoader.LoadImageFromUriAsync(new Uri("ms-appx:///Assets/Images/testimage.png"));
```

### LoadImageFromUri vs LoadImageFromUriAsync
LoadImageFromUri will immediately return a surface that is initially be empty. Later on a thread pool thread, the surface will be resized and filled in with the decoded image. This means that the call is synchronous but the image won't actually be loaded until a later time.

LoadImageFromUriAsync will asynchronously create a surface and decode the image at the same time. This means that when you get the surface back it is guaranteed to be filled in.

### Device Lost and Device Lifetime
A device lost event is exposed to the caller through the ImageLoader. When notified of a device lost, the caller should ask the ImageLoader to redraw any needed surfaces.

When the ImageLoader is created with only a Compositor, it will create the devices itself. As such, the ImageLoader will be responsible for device lifetime, which includes creating and disposing of the device. However, if a CompositoinGraphicsDevice is given to the ImageLoader instead, then the caller is responsible for the device's lifetime.
