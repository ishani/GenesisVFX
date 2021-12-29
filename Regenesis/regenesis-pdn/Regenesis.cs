/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Runtime.InteropServices;

using Asphalt.Controls;

namespace regenesis
{
    public enum MaskDistortion
    {
        None    = 0,
        Noise   = 1,
        Celluar = 2,
        Lump    = 3
    };

    public enum MaskPunchOut
    {
        None    = 0,
        Outer   = 1,
        Inner   = 2
    };

    [StructLayout( LayoutKind.Sequential, Pack = 1 )]
    public struct PixelBits
    {
        public int      Width;
        public int      Height;
        public int      Stride;
        public IntPtr   Scan0;


        public PixelBits( BitmapData bd )
        {
            Width = bd.Width;
            Height = bd.Height;
            Stride = bd.Stride;
            Scan0 = bd.Scan0;
        }
    }

    [StructLayout( LayoutKind.Sequential, Pack = 1 )]
    public struct IntColour8
    {
        public byte     R;
        public byte     G;
        public byte     B;
        public byte     A;

        public void SetFrom(Color c)
        {
            R = c.R;
            G = c.G;
            B = c.B;
            A = c.A;
        }
    }

    [StructLayout( LayoutKind.Sequential, Pack = 1 )]
    public struct Images
    {
        public PixelBits   Background;
        public PixelBits   Mask;

        public PixelBits   Output;

        public float       LongestEdge;
    }

    [StructLayout( LayoutKind.Sequential, Pack = 1 )]
    public struct ResultMeta
    {
        public float    RenderTime;
        public UInt32   MaskElements;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst=256)]
        public String   EffectName;
    }


    [StructLayout( LayoutKind.Sequential, Pack = 1 )]
    struct Globals
    {
        public float        CenterX;
        public float        CenterY;

        public IntColour8   BackgroundColour;
        public IntColour8   ColouriseColour;

        public float        Time;

        public float        GlobalScale;
        public float        GlobalRotate;

        public float        GlobalHueShift;
        public float        GlobalSaturationShift;
        public float        GlobalValueShift;

        [MarshalAs (UnmanagedType.I1)]
        public bool         ProtectAlpha;

        [MarshalAs (UnmanagedType.I1)]
        public bool         DisplacementSmoothing;
        public float        DisplacementSmoothingDistance;
        public int          DisplacementSmoothingSampleGrid;

        [MarshalAs (UnmanagedType.I1)]
        public bool         BoostNoiseVariance;
        public float        BoostNoiseScale;

        [MarshalAs (UnmanagedType.I1)]
        public bool         UseHadesNoise;
        public float        GlobalNoiseScale;


        [MarshalAs (UnmanagedType.I1)]
        public bool         RenderMaskElementsSeparately;
        public UInt32       PerElementRandomisationSeed;
        public float        PerElementRandomisationTime;
        public float        PerElementRandomisationRotation;
        public float        PerElementRandomisationScaleMin;
        public float        PerElementRandomisationScaleMax;

        public float        ShapeMaskExtractionThreshold;
        public UInt32       ShapeMaskExtractionMinElemSizePixels;
        public float        ShapeMaskBlurSize;
        public float        ShapeMaskDistanceMultiplier;
        public float        ShapeMaskTighten;

        public UInt32       ShapeMaskRandomSeed;
        [MarshalAs(UnmanagedType.I4)]
        public MaskDistortion ShapeMaskDistortionMode;

        public float        ShapeMaskDistortionPerturbAmount;
        public float        ShapeMaskDistortionX;
        public float        ShapeMaskDistortionY;
        public float        ShapeMaskDistortionZ;
        public float        ShapeMaskDistortionOffset;
        public float        ShapeMaskDistortionI;
        [MarshalAs (UnmanagedType.I1)]
        public bool         ShapeMaskAltZAxis;

        public MaskPunchOut ShapeMaskPunchOut;

        public Int32        AllElementOverload;


        public Globals(int rWidth, int rHeight)
        {
            CenterX                                 = (float)rWidth * 0.5f;
            CenterY                                 = (float)rHeight * 0.5f;
            Time                                    = 0.0f;
            GlobalScale                             = 1.0f;
            GlobalRotate                            = 0.0f;

            GlobalHueShift                          = 0.0f;
            GlobalSaturationShift                   = 0.0f;
            GlobalValueShift                        = 0.0f;

            BackgroundColour                        = new IntColour8();
            BackgroundColour.SetFrom( Color.Black );

            ColouriseColour                         = new IntColour8();
            ColouriseColour.SetFrom( Color.White );

            ProtectAlpha                            = true;

            DisplacementSmoothing                   = false;
            DisplacementSmoothingDistance           = 2.5f;
            DisplacementSmoothingSampleGrid         = 5;

            BoostNoiseVariance                      = false;
            BoostNoiseScale                         = 16.0f;

            UseHadesNoise                           = false;
            GlobalNoiseScale                        = 1.0f;

            RenderMaskElementsSeparately            = false;
            PerElementRandomisationSeed             = 1234;
            PerElementRandomisationTime             = 0.0f;
            PerElementRandomisationRotation         = 0.0f;
            PerElementRandomisationScaleMin         = 1.0f;
            PerElementRandomisationScaleMax         = 1.0f;

            ShapeMaskExtractionThreshold            = 0.5f;
            ShapeMaskExtractionMinElemSizePixels    = 1;
            ShapeMaskBlurSize                       = 40.0f;
            ShapeMaskDistanceMultiplier             = 1.0f;
            ShapeMaskTighten                        = 1.0f;

            ShapeMaskRandomSeed                     = 12345;
            ShapeMaskDistortionMode                 = MaskDistortion.None;

            ShapeMaskDistortionPerturbAmount        = 25.0f;
            ShapeMaskDistortionX                    = 0.0f;
            ShapeMaskDistortionY                    = 0.0f;
            ShapeMaskDistortionZ                    = 0.0f;
            ShapeMaskDistortionOffset               = 0.5f;
            ShapeMaskDistortionI                    = 1.0f;
            ShapeMaskAltZAxis                       = false;

            ShapeMaskPunchOut                       = MaskPunchOut.Inner;

            AllElementOverload                      = -1;
        }
    };

    public delegate void FeedbackFn( int logLevel, string message );

    abstract class Native
    {
        [DllImport( "kernel32.dll", SetLastError = true )]
        public static extern bool SetDllDirectory( string lpPathName );

        [DllImport( "kernel32.dll" )]
        public static extern long GetEnabledXStateFeatures();
    }

    // expose API of the Regenesis library
    abstract class Imports
    {
        public const string Regenesis64Lib = @"Regenesis64.dll";

        [DllImport( Regenesis64Lib )]
        public static extern UInt32 RgAPI();

        [DllImport( Regenesis64Lib, CharSet = CharSet.Ansi )]
        public static extern IntPtr RgVersion();

        public static string GetRgVersion()
        {
            IntPtr cstr =regenesis.Imports.RgVersion();
            return Marshal.PtrToStringAnsi(cstr);
        }

        [DllImport( Regenesis64Lib, CharSet = CharSet.Ansi )]
        public static extern void SetLoggingFn( FeedbackFn logFn );


        [DllImport( Regenesis64Lib, CharSet = CharSet.Ansi )]
        public static extern void RenderFile(
            string          arg_gfxFile,
            ref Globals     arg_globals,
            ref Images      arg_images,
            ref ResultMeta  out_meta );

        [DllImport( Regenesis64Lib, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi )]
        public static extern void RenderMemory(
            [In, Out] [MarshalAs( UnmanagedType.LPArray )] byte[] arg_gfxBuffer,
            int             arg_gfxBufferSize,
            ref Globals     arg_globals,
            ref Images      arg_images,
            ref ResultMeta  out_meta );
    }

    abstract class Utils
    {
        public static Rectangle AspectFitImage( int sourceW, int sourceH, int destW, int destH )
        {
            float scale = Math.Min( (float)destW / (float)sourceW, (float)destH / (float)sourceH );

            int drawW = (int)Math.Round((float)sourceW * scale);
            int drawH = (int)Math.Round((float)sourceH * scale);

            int offX = (destW / 2 ) - (drawW / 2);
            int offY = (destH / 2 ) - (drawH / 2);

            return new Rectangle( offX, offY, drawW, drawH );
        }

        public static bool HasAvxSupport()
        {
            try
            {
                return ( Native.GetEnabledXStateFeatures() & 4 ) != 0;
            }
            catch
            {
                return false;
            }
        }
    }

    // utility class that takes care of orchestrating the main viewport rendering via the Regenesis engine
    class RenderRunner : IDisposable
    {
        public readonly int     PipelineWidth;
        public readonly int     PipelineHeight;
        public readonly float   PipelineScale;

        public readonly int     CanvasWidth;
        public readonly int     CanvasHeight;

        public Globals          RenderGlobals;

        public readonly Bitmap  ImageBuffer;
        public readonly Bitmap  CanvasBuffer;

        internal Rectangle      ImageToCanvasFitRect;

        internal Rectangle      ClipRect;
        private Rectangle       CanvasEdge;

        internal Bitmap         Background;
        internal Bitmap         BackgroundLockedCopy;
        internal BitmapData     BackgroundData;
        internal PixelBits      BackgroundPB;

        internal Bitmap         Mask;
        internal BitmapData     MaskData;
        internal PixelBits      MaskPB;

        internal float          LongestEdge;

        internal sealed class HostRemapping
        {
            private readonly int      OffsetX;
            private readonly int      OffsetY;
            private readonly float    RescaleX;
            private readonly float    RescaleY;
            private readonly float    RecpX;
            private readonly float    RecpY;

            public HostRemapping( Size pipeline, Size host )
            {
                Rectangle r = regenesis.Utils.AspectFitImage(
                    pipeline.Width,
                    pipeline.Height,
                    host.Width,
                    host.Height);

                OffsetX  = (host.Width - r.Width) / 2;
                OffsetY  = (host.Height - r.Height) / 2;

                RescaleX = (float)pipeline.Width / (float)r.Width;
                RescaleY = (float)pipeline.Height / (float)r.Height;

                RecpX    = 1.0f / (float)pipeline.Width;
                RecpY    = 1.0f / (float)pipeline.Height;
            }

            public PointF RemapHostPoint( in Point pt )
            {
                return new PointF(
                    (float)(pt.X - OffsetX) * RescaleX,
                    (float)(pt.Y - OffsetY) * RescaleY
                    );
            }

            public PointF RemapHostPointRelative( in Point pt )
            {
                var pixelPoint = RemapHostPoint(pt);
                return new PointF( 
                    pixelPoint.X * RecpX, 
                    pixelPoint.Y * RecpY 
                    );
            }
        };


        public RenderRunner( Size canvasSize, Size sourceSize, int divisor )
        {
            CanvasWidth     = canvasSize.Width;
            CanvasHeight    = canvasSize.Height;

            PipelineWidth   = sourceSize.Width / divisor;
            PipelineHeight  = sourceSize.Height / divisor;
            PipelineScale   = (float)( 1.0 / (double)divisor );

            ClipRect        = new Rectangle( 0, 0, PipelineWidth, PipelineHeight );

            ClearBackgroundImage();
            ClearMaskImage();

            RenderGlobals   = new Globals( PipelineWidth, PipelineHeight );

            ImageBuffer     = new Bitmap( PipelineWidth, PipelineHeight, PixelFormat.Format32bppArgb );
            CanvasBuffer    = new Bitmap( CanvasWidth, CanvasHeight, PixelFormat.Format32bppArgb );

            ImageToCanvasFitRect = Utils.AspectFitImage( ImageBuffer.Width, ImageBuffer.Height, CanvasBuffer.Width, CanvasBuffer.Height );

            if ( canvasSize != sourceSize )
            { 
                CanvasEdge = new Rectangle( 
                    ImageToCanvasFitRect.Left - 1, 
                    ImageToCanvasFitRect.Top - 1, 
                    ImageToCanvasFitRect.Width + 1, 
                    ImageToCanvasFitRect.Height + 1 );
            }

            SetLongestEdge( Math.Max( PipelineWidth, PipelineHeight ) );
        }

        public void SetLongestEdge( int longestEdgePixels )
        {
            LongestEdge = (float)longestEdgePixels * PipelineScale;
        }

        public void SetBackgroundImage( in Bitmap image )
        {
            ClearBackgroundImage();

            Background = new Bitmap( PipelineWidth, PipelineHeight );

            Rectangle fitDim = Utils.AspectFitImage( image.Width, image.Height, Background.Width, Background.Height );

            SetLongestEdge((int)( (float) Math.Max( fitDim.Width, fitDim.Height ) / PipelineScale) );

            using ( Graphics g = Graphics.FromImage( Background ) )
            {
                g.CompositingQuality    = CompositingQuality.HighQuality;
                g.SmoothingMode         = SmoothingMode.HighQuality;

                g.DrawImage( image, fitDim );
            }

            BackgroundLockedCopy = new Bitmap( Background );

            BackgroundData = Background.LockBits( ClipRect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb );
            BackgroundPB = new PixelBits( BackgroundData );
        }

        public void ClearBackgroundImage()
        {
            if ( Background != null )
            {
                Background.UnlockBits( BackgroundData );
                BackgroundData = null;

                BackgroundLockedCopy = null;

                Background.Dispose();
                Background = null;
            }

            BackgroundPB = new PixelBits();
        }

        public void SetMaskImage( in Bitmap image )
        {
            ClearMaskImage();

            Mask = new Bitmap( PipelineWidth, PipelineHeight );

            Rectangle fitDim = Utils.AspectFitImage( image.Width, image.Height, Mask.Width, Mask.Height );

            using ( Graphics g = Graphics.FromImage( Mask ) )
            {
                g.CompositingQuality    = CompositingQuality.HighQuality;
                g.SmoothingMode         = SmoothingMode.HighQuality;

                g.DrawImage( image, fitDim );
            }

            MaskData = Mask.LockBits( ClipRect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb );
            MaskPB = new PixelBits( MaskData );
        }

        public void ClearMaskImage()
        {
            if ( Mask != null )
            {
                Mask.UnlockBits( MaskData );
                MaskData = null;

                Mask.Dispose();
                Mask = null;
            }

            MaskPB = new PixelBits();
        }

        public void DoRender( in Byte[] gfxFile, ref ResultMeta meta )
        {
            if ( gfxFile == null )
            {
                using ( var g = Graphics.FromImage( CanvasBuffer ) )
                {
                    if ( BackgroundLockedCopy != null )
                        g.DrawImage( BackgroundLockedCopy, ImageToCanvasFitRect );
                    else
                        g.Clear( Color.Transparent );

                    var noGfxMessage     = "NO EFFECT FILE LOADED";
                    var noGfxMessageFont = FontLibrary.Instance.Get( FontChoice.Rigid, FontSize.Larger );
                    var noGfxMessageFmt  = new StringFormat() { Alignment = StringAlignment.Center, LineAlignment = StringAlignment.Center };

                    var noGfxSize = g.MeasureString( noGfxMessage, noGfxMessageFont, CanvasBuffer.Size, noGfxMessageFmt );
                    var noGfxRect = new RectangleF( 0, ( CanvasBuffer.Height / 2 ) - ( noGfxSize.Height / 2 ), CanvasBuffer.Width, noGfxSize.Height );

                    if ( CanvasEdge != null )
                        g.DrawRectangle( Pens.White, CanvasEdge );

                    g.FillRectangle( Brushes.Black, noGfxRect );
                    g.DrawString(
                        noGfxMessage,
                        noGfxMessageFont,
                        Brushes.WhiteSmoke,
                        noGfxRect,
                        noGfxMessageFmt
                        );
                }

                return;
            }

            var bmOutData = ImageBuffer.LockBits( ClipRect, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb );

            var RenderImages = new Images
            {
                Background  = BackgroundPB,
                Output      = new PixelBits( bmOutData ),
                Mask        = MaskPB,
                LongestEdge = Math.Max( LongestEdge, 32.0f )
            };

            Imports.RenderMemory( gfxFile, gfxFile.Length, ref RenderGlobals, ref RenderImages, ref meta );

            ImageBuffer.UnlockBits( bmOutData );

            using ( Graphics g = Graphics.FromImage(CanvasBuffer) )
            {
                g.Clear( Color.Transparent );

                if ( CanvasEdge != null )
                    g.DrawRectangle( Pens.White, CanvasEdge );

                g.CompositingQuality = CompositingQuality.HighQuality;
                g.DrawImage( ImageBuffer, ImageToCanvasFitRect );
            }
        }

        public void Dispose()
        {
            Mask?.Dispose();
            Background?.Dispose();
        }
    }
}
