/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Linq;
using System.Windows.Forms;

using ICSharpCode.SharpZipLib.Zip;

using Asphalt.Controls;
using Asphalt.Controls.Theme;

namespace GenesisVFX
{
    public partial class ExplorerForm
    {
        private readonly Size          _thumbnailSize = new Size( 220, 118 );
        private readonly int           _thumbnailGap  = 10;

        // scale thumbnail size to the current DPI
        private Size GetScaledThumbnailSize()
        {
            var sWidth  = PaintDotNet.UIScaleFactor.ConvertToPixelsInt( _thumbnailSize.Width,  PaintDotNet.UIScaleFactor.Minimum, PaintDotNet.UIScaleFactor.Current );
            var sHeight = PaintDotNet.UIScaleFactor.ConvertToPixelsInt( _thumbnailSize.Height, PaintDotNet.UIScaleFactor.Minimum, PaintDotNet.UIScaleFactor.Current );
            return new Size( sWidth, sHeight );
        }

        SortedDictionary<string, List<(string, string)>> gfxFileCatalog = new SortedDictionary<string, List<(string, string)>>();
        Dictionary<string, byte[]> gfxDataLookup = new Dictionary<string, byte[]>();

        // control replacement of the active GFX file, terminating any active rendering first
        private string _currentGfxFile;
        private string CurrentGfxFile
        {
            get { return _currentGfxFile; }
            set {
                hwTriggerRender.Stop();
                hwTriggerRenderFull.Stop();

                changeID++;

                _currentGfxFile = value;

                DoRender();
            }
        }
        private string _currentGfxLibrary;

        // produce a panel to help explain what to do when loading with no gfx library
        private Control CreateGfxMessagePanel()
        {
            var messagePanel = new AsphaltPanel()
            {
                Text     = "To begin, load a library of Genesis Effects files using the folder browser to your left",
                TextSize = FontSize.Smaller,
                TextFont = FontChoice.LightItalic,
                Edges    = AnchorStyles.None,
                Dock     = DockStyle.Fill
            };

            messagePanel.Canvas.Background = Canvas.Mode.Auto;

            hwPreviewScroller.Controls.Add( messagePanel );

            return messagePanel;
        }

        private void HwLoadPacks_PanelClicked( object sender, MouseEventArgs e )
        {
            if ( hwLoadFXLib.ShowDialog() == DialogResult.OK )
            {
                SearchForGfx( hwLoadFXLib.FileName );
            }
        }

        private void HwPackChoice_PanelClicked( object sender, MouseEventArgs e )
        {
            var SenderPanel = (sender as AsphaltPanel);

            var catalogNames = gfxFileCatalog.Keys.Select(f => (f, f as object)).ToList();

            DropDownHelper.DoPopUp(
                SenderPanel,
                catalogNames,
                ( text, tag ) =>
                {
                    LoadCatalog( text );
                } );
        }

        private byte[] CurrentGfxDataOrNull()
        {
            if ( string.IsNullOrEmpty( CurrentGfxFile ) )
                return null;

            return gfxDataLookup[CurrentGfxFile];
        }

        private void SearchForGfx( string zipFile )
        {
            gfxFileCatalog.Clear();
            gfxDataLookup.Clear();

            hwPackChoice.Text = "No Packs Loaded";

            // try loading presets from the provided zip file
            if ( !string.IsNullOrEmpty( zipFile ) && File.Exists( zipFile ) )
            {
                foreach ( var zipState in GenesisEffect.IterateZipContents( zipFile ) )
                {
                    if ( !zipState.entry.IsFile )
                        continue;

                    if ( Path.GetExtension( zipState.entry.Name ).ToLower() != ".gfx" )
                        continue;

                    var justName = Path.GetFileNameWithoutExtension(zipState.entry.Name);
                    var justPath = Path.GetDirectoryName(zipState.entry.Name);

                    if ( !gfxFileCatalog.ContainsKey( justPath ) )
                    {
                        gfxFileCatalog.Add( justPath, new List<(string, string)>() );
                    }

                    byte[] buffer = new byte[zipState.stream.Length];
                    zipState.stream.Read( buffer, 0, (int)zipState.stream.Length );

                    var fullLookup = $"{zipFile}{GfxConstants.MagicSepInZip}{zipState.entry.Name}";

                    gfxFileCatalog[justPath].Add( (justName, fullLookup) );
                    gfxDataLookup.Add( fullLookup, buffer );
                }
            }

            // go see if we have some local files to load too, outside of the main zip preset library
            if ( Directory.Exists( userFilePathPDN ) )
            {
                string localFileCategory = "User";

                foreach ( var localGfx in Directory.EnumerateFiles( userFilePathPDN, "*.gfx", SearchOption.AllDirectories ) )
                {
                    var justName = Path.GetFileNameWithoutExtension(localGfx);

                    if ( !gfxFileCatalog.ContainsKey( localFileCategory ) )
                    {
                        gfxFileCatalog.Add( localFileCategory, new List<(string, string)>() );
                    }

                    var buffer = File.ReadAllBytes(localGfx);

                    var fullLookup = $"{localGfx}{GfxConstants.MagicSepInFile}{justName}";

                    gfxFileCatalog[localFileCategory].Add( (justName, fullLookup) );
                    gfxDataLookup.Add( fullLookup, buffer );
                }
            }

            if ( gfxFileCatalog.Count > 0 )
            {
                LoadCatalog( gfxFileCatalog.First().Key );

                ConfigRegistry.PathToContentLibrary.Value = zipFile;
            }
            else
            {
                MessageBox.Show( "Sorry, could not find any Genesis effects files in that zip file!", hwAppTitle.Text, MessageBoxButtons.OK, MessageBoxIcon.Warning );
            }
        }

        private void LoadCatalog( string name )
        {
            if ( !gfxFileCatalog.ContainsKey( name ) )
            {
                MessageBox.Show( $"Tried to load catalog [{name}] but could not find it", hwAppTitle.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation );
                return;
            }

            // hwToolTips.SetToolTip( hwPackChoice, gfxFileRoots[name] );

            _currentGfxLibrary = name;

            gfxBrowserMessage?.Parent.Controls.Remove( gfxBrowserMessage );
            gfxBrowserMessage = null;

            hwPackChoice.Text = name;

            var gfxFiles = gfxFileCatalog[name];

            var thumbnailSize = GetScaledThumbnailSize();
            var thumbnailer = new regenesis.RenderRunner( thumbnailSize, thumbnailSize, 1 );

            // this is mostly awful...
            if ( name.ToLower().Contains( "displace" ) )
            {
                var thumbnailBG = new Bitmap( thumbnailSize.Width, thumbnailSize.Height );
                using ( var brush = new TextureBrush( Properties.Resources.CheckerDarkTiny, WrapMode.Tile ) )
                {
                    using ( var g = Graphics.FromImage( thumbnailBG ) )
                    {
                        g.FillRectangle( brush, 0, 0, thumbnailSize.Width, thumbnailSize.Height );
                    }
                }
                thumbnailer.SetBackgroundImage( thumbnailBG );
            }

            thumbnailer.RenderGlobals.CenterX = thumbnailSize.Width * 0.3f;
            thumbnailer.RenderGlobals.CenterY = thumbnailSize.Height * 0.3f;

            var meta = new regenesis.ResultMeta();

            var scrollControls = hwPreviewScroller.EditablePanel.Controls;
            int currentOffsetX = 0;

            scrollControls.Clear();

            Cursor.Current = Cursors.WaitCursor;

            foreach ( var gfxFile in gfxFiles )
            {
                thumbnailer.DoRender( gfxDataLookup[gfxFile.Item2], ref meta );

                var thumbnailPanel = new AsphaltPanel()
                {
                    Image           = new Bitmap(thumbnailer.ImageBuffer),
                    Size            = thumbnailSize,
                    Edges           = AnchorStyles.None,
                    Padding         = new Padding(0),
                    Margin          = new Padding(0),
                    Text            = gfxFile.Item1,
                    TextAlign       = ContentAlignment.BottomCenter,
                    TextSize        = FontSize.Tiny,
                    TextOffset      = new SizeF( 0.0f, -4.0f ),
                    TextShadowSize  = 0.9f,
                    Location        = new Point(currentOffsetX, 0),
                    Tag             = gfxFile.Item2
                };

                currentOffsetX += thumbnailPanel.Width + _thumbnailGap;

                thumbnailPanel.Click += OnThumbnailClick;
                scrollControls.Add( thumbnailPanel );
            }
            hwPreviewScroller.EditablePanel.Width = currentOffsetX - _thumbnailGap;

            Cursor.Current = Cursors.Default;
        }

        private void OnThumbnailClick( object sender, EventArgs e )
        {
            if ( sender is AsphaltPanel panel )
            {
                var gfxFileData = panel.Tag as string;

                if ( gfxFileData != null )
                {
                    CurrentGfxFile = gfxFileData;
                }
                else
                {
                    MessageBox.Show( $"Could no longer file GFX file to load", hwAppTitle.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation );
                }
            }
        }
    }
}
