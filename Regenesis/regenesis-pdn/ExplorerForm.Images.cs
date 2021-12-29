/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading.Tasks;
using System.Windows.Forms;

using Asphalt.Controls;
using PaintDotNet;

namespace GenesisVFX
{
    public partial class ExplorerForm
    {
        private readonly ColorMatrix colorInversionMatrix = new ColorMatrix(
           new float[][]
           {
              new float[] {-1, 0, 0, 0, 0},
              new float[] {0, -1, 0, 0, 0},
              new float[] {0, 0, -1, 0, 0},
              new float[] {0, 0, 0, 1, 0},
              new float[] {1, 1, 1, 0, 1}
           });

        private Bitmap bgSourceImage;
        private Bitmap bgBlankImage;

        private void CreateBackgroundImages()
        {
            bgSourceImage = EnvironmentParameters.SourceSurface.CreateAliasedBitmap();

            bgBlankImage = new Bitmap( EnvironmentParameters.SourceSurface.Width, EnvironmentParameters.SourceSurface.Height );
            UpdateBackgroundImages();
        }

        private void UpdateBackgroundImages()
        {
            using (var g = Graphics.FromImage(bgBlankImage))
            {
                var clearColour = Color.FromArgb(
                    (int)Math.Round(hwBackgroundClearAlpha.CurrentValue),
                    hwBackgroundClearColour.BackColor);

                g.Clear(clearColour);
            }
        }

        private void RenderFitImageToPanel(in Bitmap image, AsphaltPanel panel)
        {
            Rectangle fitDim = regenesis.Utils.AspectFitImage(image.Width, image.Height, panel.Width, panel.Height);

            var bgImage = new Bitmap(panel.Width, panel.Height);
            using ( var g = Graphics.FromImage(bgImage) )
            {
                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                g.SmoothingMode      = System.Drawing.Drawing2D.SmoothingMode.HighQuality;

                g.DrawImage(image, fitDim);
            }

            panel.Image = bgImage;
        }

        private void SetBackground( in Bitmap image )
        {
            if ( image != null )
            {
                foreach (var runner in AllRenderRunners() )
                    runner.SetBackgroundImage( image );
            }
            else
            {
                foreach ( var runner in AllRenderRunners() )
                    runner.ClearBackgroundImage();
            }

            DoRender();
        }

        private void SetMask(in Bitmap image)
        {
            if ( image != null )
            {
                foreach ( var runner in AllRenderRunners() )
                    runner.SetMaskImage( image );

                hwMaskImage.Tag = image;
                RenderFitImageToPanel( image, hwMaskImage );
            }
            else
            {
                foreach ( var runner in AllRenderRunners() )
                    runner.ClearMaskImage();

                hwMaskImage.Tag = null;
                hwMaskImage.Image = null;
            }

            UpdateMaskEnableStates();
            DoRender();
        }

        private void UpdateBackgroundChoices()
        {
            var bgColourControls = new Control[]
            {
                hwBackgroundClearColour,
                hwClearColourLabel,
                hwBackgroundClearAlpha,
                hwBackgroundAlphaLabel
            };

            foreach (Control c in bgColourControls)
                c.Enabled = hwBackgroundClear.Pressed;

            if ( hwBackgroundClear.Pressed )
            {
                UpdateBackgroundImages();
                SetBackground( bgBlankImage );
            }
            else
            {
                SetBackground( bgSourceImage );
            }
        }

        private void BackgroundModeChanged( object sender, bool previous, bool current )
        {
            UpdateBackgroundChoices();
        }

        private void HwBackgroundClearColour_Click(object sender, EventArgs e)
        {
            var SenderPanel = (sender as PictureBox);
            var localPt = new Point(SenderPanel.Location.X, SenderPanel.Location.Y + SenderPanel.Height);

            AsphaltColourWheel.PopupAt(SenderPanel.Parent.PointToScreen(localPt), SenderPanel.BackColor, (o, rgb) =>
            {
                SenderPanel.BackColor = rgb.ToColor();
                UpdateBackgroundChoices();
            });
        }

        private void HwBackgroundClearAlpha_ValueChanged(object sender, TrackEventArgs e)
        {
            UpdateBackgroundChoices();
        }



        private void HwMaskPull_PanelClicked(object sender, MouseEventArgs e)
        {
            hwDiscreteRender.Pressed = false;

            Bitmap image = EnvironmentParameters.SourceSurface.CreateAliasedBitmap();
            SetMask( image );
        }

        private void HwMaskLoad_PanelClicked(object sender, MouseEventArgs e)
        {
            previewClickIgnore = 2;
            if (hwPickImageDlg.ShowDialog() == DialogResult.OK)
            {
                hwDiscreteRender.Pressed = false;

                try
                {
                    Bitmap image = Bitmap.FromFile( hwPickImageDlg.FileName ) as Bitmap;
                    SetMask( image );
                }
                catch ( Exception ex )
                {
                    MessageBox.Show( $"Unable to load image file [{hwPickImageDlg.FileName}]\n\n{ex.Message}", hwAppTitle.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation );
                }
            }
        }

        private void HwMaskPaste_PanelClicked( object sender, MouseEventArgs e )
        {
            var clipImage = ClipboardUtil.GetImage();
            if ( clipImage != null )
            {
                SetMask( clipImage );
            }
            else
            {
                MessageBox.Show( $"The clipboard did not contain an image", hwAppTitle.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation );
            }
        }

        private unsafe void HwMaskClip_PanelClicked( object sender, MouseEventArgs e )
        {
            hwDiscreteRender.Pressed = false;

            // nothing selected, or everything selected - neither of which interest us
            var selectionArea = EnvironmentParameters.GetSelectionAsPdnRegion().GetArea64();
            if ( selectionArea == 0 ||
                 selectionArea == EnvironmentParameters.SourceSurface.Width * EnvironmentParameters.SourceSurface.Height )
                return;

            Bitmap clipImage = new Bitmap(
                EnvironmentParameters.SourceSurface.Width,
                EnvironmentParameters.SourceSurface.Height,
                PixelFormat.Format32bppArgb );

            try
            {
                using ( var gfx = Graphics.FromImage(clipImage) )
                    gfx.Clear(Color.Black);
            
                Rectangle selection = EnvironmentParameters.GetSelectionAsPdnRegion().GetBoundsInt();

                var bitmapData = clipImage.LockBits(selection, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
                {
                    int bmWidth = bitmapData.Width;
                    int bmHeight = bitmapData.Height;

                    byte* ptr = (byte*)(void*)bitmapData.Scan0;
                    int stride = bitmapData.Stride;

                    Parallel.For( 0, bmHeight, delegate (int y)
                    {
                        ColorBgra* ptr2 = (ColorBgra*)(ptr + (long)y * (long)stride);

                        for ( int x = 0; x < bmWidth; x++ )
                        {
                            if ( EnvironmentParameters.GetSelectionAsPdnRegion().IsVisible(selection.Left + x, selection.Top + y) )
                                *ptr2 = Color.White;

                            ptr2++;
                        }
                    });
                }

                clipImage.UnlockBits(bitmapData);
            }
            catch ( Exception ex )
            {
                MessageBox.Show("Could not extract selection to new mask:\n\n" + ex.Message, hwAppTitle.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            SetMask( clipImage );
        }

        private void HwMaskInvert_PanelClicked( object sender, MouseEventArgs e )
        {
            if ( hwMaskImage.Tag is Bitmap maskImage )
            {
                Bitmap maskInverted = new Bitmap( maskImage );

                using ( var g = Graphics.FromImage( maskInverted ) )
                {
                    var attributes = new ImageAttributes();
                    attributes.SetColorMatrix( colorInversionMatrix );

                    g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                    g.DrawImage( maskInverted, new Rectangle( 0, 0, maskImage.Width, maskImage.Height ), 0, 0, maskImage.Width, maskImage.Height, GraphicsUnit.Pixel, attributes );
                }

                SetMask( maskInverted );
            }
        }

        private void HwMaskClear_PanelClicked(object sender, MouseEventArgs e)
        {
            SetMask( null );
        }
    }
}
