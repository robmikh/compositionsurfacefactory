using Microsoft.Graphics.Canvas.Text;
using Microsoft.Graphics.Canvas.UI.Composition;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.UI;
using Windows.UI.Composition;
using Windows.UI.Text;

namespace Robmikh.Util.CompositionImageLoader
{
    public interface ITextSurface : IDisposable
    {
        //
        // Read-only Properties
        //
        IImageLoader ImageLoader { get; }
        ICompositionSurface Surface { get; }
        Size Size { get; }

        //
        // Editable Properties
        //
        String Text { get; set; }
        float Width { get; set; }
        float Height { get; set; }
        String FontFamily { get; set; }
        float FontSize { get; set; }
        FontStyle FontStyle { get; set; }
        WordWrapping WordWrapping { get; set; }
        Padding Padding { get; set; }
        Color Foreground { get; set; }
        Color Background { get; set; }

        //
        // Functions
        //
        void RedrawSurface();

        //
        // Events
        //
        event EventHandler<Object> SurfaceRedrawn;
    }

    //
    // We use our own enum since it seems that we
    // can't expose types from Win2D as a runtime
    // component.
    //
    public enum WordWrapping
    {
        Wrap = 0,
        NoWrap = 1,
        EmergencyBreak = 2,
        WholeWord = 3,
        Character = 4
    }

    public struct Padding
    {
        public float Left;
        public float Right;
        public float Top;
        public float Bottom;
    }

    //
    // We use an extension instead of casting the enum to
    // ensure that we are compatible with future versions
    // of Win2D just incase the enum values change.
    //
    static class WrappingExtensions
    {
        public static CanvasWordWrapping ToCanvasWordWrapping(this WordWrapping wrap)
        {
            switch(wrap)
            {
                case WordWrapping.Character:
                    return CanvasWordWrapping.Character;
                case WordWrapping.EmergencyBreak:
                    return CanvasWordWrapping.EmergencyBreak;
                case WordWrapping.NoWrap:
                    return CanvasWordWrapping.NoWrap;
                case WordWrapping.WholeWord:
                    return CanvasWordWrapping.WholeWord;
                case WordWrapping.Wrap:
                    return CanvasWordWrapping.Wrap;
                default:
                    return CanvasWordWrapping.NoWrap;
            }
        }

        public static WordWrapping ToWordWrapping(this CanvasWordWrapping wrap)
        {
            switch (wrap)
            {
                case CanvasWordWrapping.Character:
                    return WordWrapping.Character;
                case CanvasWordWrapping.EmergencyBreak:
                    return WordWrapping.EmergencyBreak;
                case CanvasWordWrapping.NoWrap:
                    return WordWrapping.NoWrap;
                case CanvasWordWrapping.WholeWord:
                    return WordWrapping.WholeWord;
                case CanvasWordWrapping.Wrap:
                    return WordWrapping.Wrap;
                default:
                    return WordWrapping.NoWrap;
            }
        }
    }

    class TextSurface : ITextSurface
    {
        public event EventHandler<Object> SurfaceRedrawn;

        private IImageLoaderInternal _imageLoader;
        private CompositionDrawingSurface _surface;

        private String _text;
        private float _width;
        private float _height;
        private Padding _padding;
        private Color _foreground;
        private Color _background;

        private CanvasTextFormat _textFormat;
        private CanvasTextLayout _textLayout;

        #region Read-only Properties
        public IImageLoader ImageLoader
        {
            get
            {
                return _imageLoader;
            }
        }

        public ICompositionSurface Surface
        {
            get
            {
                return _surface;
            }
        }

        public Size Size
        {
            get
            {
                if (_surface != null)
                {
                    return _surface.Size;
                }
                else
                {
                    return Size.Empty;
                }
            }
        }
        #endregion

        #region Editable Properties
        public String Text
        {
            get { return _text; }
            set
            {
                _text = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        public float Width
        {
            get { return _width; }
            set
            {
                _width = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        public float Height
        {
            get { return _height; }
            set
            {
                _height = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        public String FontFamily
        {
            get { return _textFormat.FontFamily; }
            set
            {
                _textFormat.FontFamily = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        public float FontSize
        {
            get { return _textFormat.FontSize; }
            set
            {
                _textFormat.FontSize = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        public FontStyle FontStyle
        {
            get { return _textFormat.FontStyle; }
            set
            {
                _textFormat.FontStyle = value;
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        public WordWrapping WordWrapping
        {
            get { return _textFormat.WordWrapping.ToWordWrapping(); }
            set
            {
                _textFormat.WordWrapping = value.ToCanvasWordWrapping();
                UpdateTextLayout();
                RedrawSurface();
            }
        }

        public Padding Padding
        {
            get { return _padding; }
            set
            {
                _padding = value;
                RedrawSurface();
            }
        }

        public Color Foreground
        {
            get { return _foreground; }
            set
            {
                _foreground = value;
                RedrawSurface();
            }
        }

        public Color Background
        {
            get { return _background; }
            set
            {
                _background = value;
                RedrawSurface();
            }
        }
        #endregion

        #region Constructors
        public TextSurface(IImageLoaderInternal imageLoader,
                           String text)
        {
            Initialize(imageLoader,
                       text,
                       0,
                       0,
                       "Segoe UI",
                       14.0f,
                       FontStyle.Normal,
                       WordWrapping.NoWrap,
                       new Padding(),
                       Colors.Black,
                       Colors.Transparent);
        }

        public TextSurface(IImageLoaderInternal imageLoader,
                           String text,
                           float width,
                           float height,
                           String fontFamily,
                           float fontSize,
                           FontStyle fontStyle,
                           WordWrapping wordWrapping,
                           Padding padding,
                           Color foreground,
                           Color background)
        {
            Initialize(imageLoader,
                       text,
                       width,
                       height,
                       fontFamily,
                       fontSize,
                       fontStyle,
                       wordWrapping,
                       padding,
                       foreground,
                       background);
        }
        #endregion

        #region Public Methods
        public void RedrawSurface()
        {
            _imageLoader.DoWorkUnderLock(() =>
            {
                float width = (float)(_textLayout.DrawBounds.Left + _textLayout.DrawBounds.Right) + _padding.Left + _padding.Right;
                float height = (float)(_textLayout.DrawBounds.Top + _textLayout.DrawBounds.Bottom) + _padding.Top + _padding.Bottom;
                CanvasComposition.Resize(_surface, new Size(width, height));

                using (var session = CanvasComposition.CreateDrawingSession(_surface))
                {
                    session.Clear(_background);
                    session.DrawTextLayout(_textLayout, _padding.Left, _padding.Right, _foreground);
                }
            });

            Task.Run(() =>
            {
                if (SurfaceRedrawn != null)
                {
                    RaiseSurfaceRedrawnEvent();
                }
            });
        }

        public void Dispose()
        {
            SurfaceRedrawn = null;
            _surface.Dispose();
            _imageLoader.DeviceReplacedEvent -= OnDeviceReplaced;
            _surface = null;
            _imageLoader = null;
        }
        #endregion

        #region Private Methods
        private void Initialize(IImageLoaderInternal imageLoader,
                                String text,
                                float width,
                                float height,
                                String fontFamily,
                                float fontSize,
                                FontStyle fontStyle,
                                WordWrapping wordWrapping,
                                Padding padding,
                                Color foreground,
                                Color background)
        {
            _imageLoader = imageLoader;
            //
            // We initialize the surface to the size of 1x1 becuase
            // we can't create a drawing session from an empty surface
            // and we need the session to create our first TextLayout.
            //
            _surface = _imageLoader.CreateSurface(new Size(1, 1));
            _textFormat = new CanvasTextFormat();

            _text = text;
            _width = width;
            _height = height;
            _padding = padding;
            _foreground = foreground;
            _background = background;

            _textFormat.FontFamily = fontFamily;
            _textFormat.FontSize = fontSize;
            _textFormat.FontStyle = fontStyle;
            _textFormat.WordWrapping = wordWrapping.ToCanvasWordWrapping();

            UpdateTextLayout();

            _imageLoader.DeviceReplacedEvent += OnDeviceReplaced;
        }

        private void OnDeviceReplaced(object sender, object e)
        {
            Debug.WriteLine("CompositionImageLoader - Redrawing TextSurface from Device Replaced");
            RedrawSurface();
        }

        private void UpdateTextLayout()
        {
            _imageLoader.DoWorkUnderLock(() =>
            {
                using (var session = CanvasComposition.CreateDrawingSession(_surface))
                {
                    _textLayout = new CanvasTextLayout(session, _text, _textFormat, _width, _height);
                }
            });
        }

        private void RaiseSurfaceRedrawnEvent()
        {
            var surfaceEvent = SurfaceRedrawn;
            if (surfaceEvent != null)
            {
                surfaceEvent(this, new EventArgs());
            }
        }
        #endregion
    }
}
