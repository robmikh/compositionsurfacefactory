using Robmikh.Util.CompositionImageLoader;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Composition;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace TestApplication
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private Compositor _compositor;
        private ContainerVisual _rootVisual;
        private SpriteVisual _visual1;
        private SpriteVisual _visual2;

        private IImageLoader _imageLoader;

        public MainPage()
        {
            this.InitializeComponent();

            InitComposition();

            CreateVisual();
            CreateVisualAsync();
        }

        private void InitComposition()
        {
            _compositor = ElementCompositionPreview.GetElementVisual(VisualStandIn).Compositor;
            _imageLoader = ImageLoaderFactory.CreateImageLoader(_compositor);

            _rootVisual = _compositor.CreateContainerVisual();

            ElementCompositionPreview.SetElementChildVisual(VisualStandIn, _rootVisual);
        } 

        private void CreateVisual()
        {
            _visual1 = _compositor.CreateSpriteVisual();

            _visual1.Size = new Vector2(250.0f, 250.0f);
            _visual1.Offset = new Vector3(0.0f, 0.0f, 0.0f);

            _visual1.Brush = _compositor.CreateSurfaceBrush(_imageLoader.LoadImageFromUri(new Uri("ms-appx:///Assets/Images/testimage.png")));

            _rootVisual.Children.InsertAtTop(_visual1);
        }

        private async void CreateVisualAsync()
        {
            _visual2 = _compositor.CreateSpriteVisual();

            _visual2.Size = new Vector2(250.0f, 250.0f);
            _visual2.Offset = new Vector3(300.0f, 0.0f, 0.0f);

            var surface = await _imageLoader.LoadImageFromUriAsync(new Uri("ms-appx:///Assets/Images/testimage.png"));

            _visual2.Brush = _compositor.CreateSurfaceBrush(surface);

            _rootVisual.Children.InsertAtTop(_visual2);
        }
    }
}
