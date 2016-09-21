# CompositionSurfaceFactory
CompositionSurfaceFactory (formerly known as CompositionImageLoader) is a surface factory intended for use with the Windows.UI.Composition api based on Win2D and written in C++/CX. Don't worry, image loading still comes out of the box!

## Nuget Package
You can download the latest nuget package for CompositionImageLoader using this command:
```
PM>  Install-Package Robmikh.CompositionSurfaceFactory -Pre
```
Learn more [here](https://www.nuget.org/packages/Robmikh.CompositionSurfaceFactory).

## Usage
Loading images into a surface is easy using CompositionSurfaceFactory:
```
var surfaceFactory = SurfaceFactory.CreateFromCompositor(_compositor);
var surface = surfaceFactory.CreateSurfaceFromUri(new Uri("ms-appx:///Assets/Images/testimage.png"));
```
You can also pass in a CompositionGraphicsDevice instead of a Compositor if you already have a device you would like the SurfaceFactory to use.
```
var surfaceFactory = SurfaceFactory.CreateFromGraphicsDevice(_compositionGraphicsDevice);
var surface = await surfaceFactory.CreateSurfaceFromUriAsync(new Uri("ms-appx:///Assets/Images/testimage.png"));
```

### CreateSurfaceFromUri vs CreateSurfaceFromUriAsync
CreateSurfaceFromUri will immediately return a surface that is initially be empty. Later on a thread pool thread, the surface will be resized and filled in with the decoded image. This means that the call is synchronous but the image won't actually be loaded until a later time.

CreateSurfaceFromUriAsync will asynchronously create a surface and decode the image at the same time. This means that when you get the surface back it is guaranteed to be filled in.

### UriSurface
UriSurface (formerly ManagedSurface) is just a wrapper to the CompositionDrawingSurface class which handles device lost events raised by the SurfaceFactory. Using it is similar to CreateSurfaceFromUri(Async):
```
var uriSurface = _surfaceFactory.CreateUriSurface(new Uri("ms-appx:///Assets/Images/testimage.png"));
//
// Consuming the real surface
//
visual.Brush = _compositor.CreateSurfaceBrush(uriSurface.Surface);
```
When you're done with the UriSurface, just call Dispose() to let it unregister from SurfaceFactory's DeviceReplaced event. It is important to note that a UriSurface is "tethered" to a SurfaceFactory, as it is dependent on the SurfaceFactory to be able to redraw its surface. If you call Dispose() on the SurfaceFactory, any UriSurfaces that were created with that SurfaceFactory won't be able to redraw and will throw an exception when asked.

You can also ask a UriSurface to redraw its contents at any time, and even provide a new Uri for it to use (and hold onto for the purposes of DeviceLost events).

### TextSurface
TextSurface is very similar to UriSurface in that they are both wrappers to the CompositionDrawingSurface class which handle device lost events raised by the SurfaceFactory. However, TextSurface doesn’t get its contents from an image, but rather draws its own contents based on a given String. TextSurface also allows you to specify font, font size, font style, wrapping, and color by leveraging Win2D’s TextLayout APIs. Using TextSurface is easy:

```
var textSurface = _surfaceFactory.CreateTextSurface("Hello, World!");
//
// Consuming the real surface
//
visual.Brush = _compositor.CreateSurfaceBrush(textSurface.Surface);
```

It's important to note that changing properties tells the TextSurface to redraw, so if you're going to do a lot of customization, it might be wise to do most of it at creation time:

```
var textSurface= _surfaceFactory.CreateTextSurface(reallyLongString,            // Text
                                                  250.0f,                       // Desired Width
                                                  0.0f,                         // Desired Height
                                                  "Times New Roman",            // Font Family
                                                  14.0f,                        // Font Size
                                                  FontStyle.Normal,             // Font Style
                                                  TextHorizontalAlignment.Left, // Horizontal Alignment
                                                  TextVerticalAlignment.Top,    // Vertical Alignment
                                                  WordWrapping.WholeWord,       // Wrapping
                                                  new Padding(),                // Padding
                                                  Colors.DarkRed,               // Foreground Color
                                                  Colors.Transparent);          // Background Color
```

Finally, changing properties on the fly after creation could impact the new size of the surface, so a SurfaceRedrawn event has been exposed. This way you can always make sure your text is displayed correctly:

```
textSurface.SurfaceRedrawn += (s, a) =>
{
    visual.Size = textSurface.Size.ToVector2();
};
textSurface.FontSize = 30.0f;
```

Just like with UriSurface, don't forget to dispose of TextSurface when you're done!

### Device Lost and Device Lifetime
A device lost event is exposed to the caller through the SurfaceFactory. When notified of a device lost, the caller should ask the SurfaceFactory to redraw any needed surfaces. UriSurface and TextSurface will do this for you if used.

When the SurfaceFactory is created with only a Compositor, it will create a new `CanvasDevice`. As such, the SurfaceFactory will be responsible for device lifetime, which includes creating and disposing of the device. Alternatively, if a CompositoinGraphicsDevice is given to the SurfaceFactory instead, then the caller is responsible for the device's lifetime.
