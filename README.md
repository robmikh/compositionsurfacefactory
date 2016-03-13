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

### TextSurface
TextSurface is very similar to ManagedSurfae in that they are both wrappers to the CompositionDrawingSurface class which handle device lost events raised by the ImageLoader. However, TextSurface doesn’t get its contents from an image, but rather draws its own contents based on a given String. TextSurface also allows you to specify font, font size, font style, wrapping, and color by leveraging Win2D’s TextLayout APIs. Using TextSurface is easy:

```
var textSurface = _imageLoader.CreateTextSurface("Hello, World!");
//
// Consuming the real surface
//
visual.Brush = _compositor.CreateSurfaceBrush(textSurface.Surface);
```

It's important to note that changing properties telsl the TextSurface to redraw, so if you're going to do a lot of customization, it might be wise to do most of it at creation time:

```
var textSurface= _imageLoader.CreateTextSurface(reallyLongString,       // Text
                                                250.0f,                 // Desired Width
                                                0.0f,                   // Desired Height
                                                "Times New Roman",      // Font Family
                                                14.0f,                  // Font Size
                                                FontStyle.Normal,       // Font Style
                                                WordWrapping.WholeWord, // Wrapping
                                                new Padding(),          // Padding
                                                Colors.DarkRed,         // Foreground Color
                                                Colors,Transparent);    // Background Color
```

Finally, changing properties on the fly after creation could impact the new size of the surface, so a SurfaceRedrawn event has been exposed. This way you can always make sure your text is displayed correctly:

```
textSurface.SurfaceRedrawn += (s, a) =>
{
    visual.Size = textSurface.Size.ToVector2();
};
textSurface.FontSize = 30.0f;
```

Just like with ManagedSurface, don't forget to dispose of TextSurface when you're done!

### Device Lost and Device Lifetime
A device lost event is exposed to the caller through the ImageLoader. When notified of a device lost, the caller should ask the ImageLoader to redraw any needed surfaces. MangedSurface will do this for you if used.

When the ImageLoader is created with only a Compositor, it will create the devices itself. As such, the ImageLoader will be responsible for device lifetime, which includes creating and disposing of the device. However, if a CompositoinGraphicsDevice is given to the ImageLoader instead, then the caller is responsible for the device's lifetime.
