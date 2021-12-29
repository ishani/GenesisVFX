/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using System.Windows.Forms;

using Asphalt.Controls;
using Asphalt.Controls.Theme;

namespace GenesisVFX
{
    public partial class ExplorerForm : 
         // AsphaltForm 
          AsphaltEffectConfigDialog
    {
        private static void LogF( int level, string msg )
        {
            if ( level == 3 )
            {
                MessageBox.Show( msg, "Regenesis Engine Error", MessageBoxButtons.OK, MessageBoxIcon.Error );
            }
            Console.WriteLine(msg);
        }
        private static readonly regenesis.FeedbackFn logFunction = new regenesis.FeedbackFn(LogF);

        private readonly string userFilePathPDN;            // path given by PDN for 'user files' with a GVFX suffix, eg. "C:\Users\(Name)\Documents\paint.net User Files\Genesis"

        private regenesis.RenderRunner renderRunnerFull;
        private regenesis.RenderRunner renderRunnerHalf;
        private regenesis.RenderRunner renderRunnerQuarter;
        private regenesis.RenderRunner renderRunnerEighth;

        private regenesis.RenderRunner GetInteractiveRunner()
        {
            if ( hwDynResMed.Pressed )
                return renderRunnerHalf;
            else if ( hwDynResHigh.Pressed )
                return renderRunnerQuarter;
            else // if ( hwDynResUltra.Pressed )
                return renderRunnerEighth;
        }

        private regenesis.RenderRunner GetFullPreviewRunner()
        {
            if ( hwDynResUltra.Pressed )
                return renderRunnerHalf;
            else
                return renderRunnerFull;
        }

        private IEnumerable<regenesis.RenderRunner> AllRenderRunners()
        {
            yield return renderRunnerFull;
            yield return renderRunnerHalf;
            yield return renderRunnerQuarter;
            yield return renderRunnerEighth;
        }


        private GenesisEffectConfigToken internalConfigToken = new GenesisEffectConfigToken();

        private Control gfxBrowserMessage;

        private PointF _relativePickCenter;
        private PointF  relativePickCenter
        {
            get { return _relativePickCenter; }
            set {
                _relativePickCenter = value;

                hwPickX.Text = (_relativePickCenter.X * 100.0f).ToString( "0.0" );
                hwPickY.Text = (_relativePickCenter.Y * 100.0f).ToString( "0.0" );
            }
        }

        private Int32  previewClickIgnore      = 0;
        private UInt32 changeID                = 1;
        private UInt32 renderingFullChangeID   = 0;

        const UInt32 rgExpectedAPI = 300;

        public ExplorerForm( string paintDotNetUserFilePath )
        {
            var launchLocation = Assembly.GetExecutingAssembly().Location;
            var runtimePath = Path.Combine( Path.GetDirectoryName(launchLocation), "GenesisVFX.Runtime" );

            if ( !Directory.Exists( runtimePath ) ||
                 !File.Exists( Path.Combine(runtimePath, regenesis.Imports.Regenesis64Lib) ) )
            {
                MessageBox.Show( $"Could not find the Regenesis engine or support runtimes in {runtimePath}", "Cannot boot GenesisVFX", MessageBoxButtons.OK, MessageBoxIcon.Error );
                throw new Exception( "Runtimes not found" );
            }
            regenesis.Native.SetDllDirectory( runtimePath );


            userFilePathPDN = paintDotNetUserFilePath;

            var rgAPI = regenesis.Imports.RgAPI();
            if ( rgAPI != rgExpectedAPI )
            {
                MessageBox.Show( $"Invalid Regenesis64 API version; got {rgAPI}, wanted {rgExpectedAPI}\nCheck the 'regenesis64.dll' in your PDN Effects directory and ensure it's up to date with the Effect plugin", "Cannot boot GenesisVFX", MessageBoxButtons.OK, MessageBoxIcon.Error );
                throw new Exception( "API Version Mismatch" );
            }

            regenesis.Imports.SetLoggingFn( logFunction );

            ConfigRegistry.VersionStamp.Value = regenesis.Imports.GetRgVersion();

            InitializeComponent();

            UXInit();

            hwStatus.Text      = string.Empty;
            hwStatus.IconID    = IconID.None;

            UpdateMaskStatus( 0 );
            UpdateMaskEnableStates();

            gfxBrowserMessage  = CreateGfxMessagePanel();

            SearchForGfx( ConfigRegistry.PathToContentLibrary );
        }

        private void ExplorerForm_Load( object sender, EventArgs e )
        {
            UpdateOverrideText();
        }

        private void UpdateMaskStatus( uint knownElementCount )
        {
            if ( CurrentGfxFile == null )
            {
                hwStatusMask.Text = "M A S K I N G";
                return;
            }

            if ( IsMaskLoaded )
            {
                var elementText = knownElementCount == 1 ? "E L E M E N T" : "E L E M E N T S";

                hwStatusMask.Text                            = $"M A S K I N G   ( {knownElementCount}  {elementText} )";
            }
            else
            {
                hwStatusMask.Text                            = "M A S K I N G   ( N O T  S E T )";
            }
        }

        private void UpdateRenderRunner( regenesis.RenderRunner rr )
        {
            PopulateTokenFromForm( internalConfigToken );
            TokenToRenderRunner( internalConfigToken, EnvironmentParameters.SourceSurface.Size, rr );
        }

        private void DoRenderFull()
        {
            var currentFullRunner = GetFullPreviewRunner();

            if ( renderingFullChangeID != 0 || currentFullRunner == null )
                return;

            hwCancel.Enabled = false;
            hwPreview.Enabled = false;
            hwRender.Enabled = false;

            renderingFullChangeID = changeID;

            hwFullRenderBusy.Activated = true;
            hwStatus.Text              = "Rendering higher resolution preview...";
            hwStatus.IconID            = IconID.Sync;
            hwStatusMask.Text          = "U P D A T I N G . . .";

            UpdateRenderRunner( currentFullRunner );

            Task.Factory.StartNew(() =>
            {
                var meta = new regenesis.ResultMeta();
                currentFullRunner.DoRender( CurrentGfxDataOrNull(), ref meta );

                return meta;

            }).ContinueWith(meta =>
            {
                var lastChangeID      = renderingFullChangeID;
                renderingFullChangeID = 0;

                if ( changeID != lastChangeID )
                {
                    BeginInvoke((Action)(() => DoRenderFull() ));
                    return;
                }

                BeginInvoke((Action)(() =>
                {
                    hwIm.Image      = currentFullRunner.CanvasBuffer;


                    hwFullRenderBusy.Activated = false;
                    hwStatus.IconID            = IconID.None;
                    hwStatus.Text              = $"v.{regenesis.Imports.GetRgVersion()} // took {meta.Result.RenderTime:0.00}s";

                    UpdateMaskStatus( meta.Result.MaskElements );

                    hwCancel.Enabled = true;
                    hwPreview.Enabled = true;
                    hwRender.Enabled = true;
                }));
            });
        }

        private void DoRender()
        {
            var currentRapidRunner = GetInteractiveRunner();

            if ( currentRapidRunner == null )
                return;

            UpdateRenderRunner( currentRapidRunner );

            changeID ++;

            var meta = new regenesis.ResultMeta();
            currentRapidRunner.DoRender( CurrentGfxDataOrNull(), ref meta );

            hwTriggerRenderFull.Stop();
            hwTriggerRenderFull.Start();

            // update to show we will kick a full-res render off - assuming one isn't already in progress
            if ( renderingFullChangeID == 0 )
            {
                hwStatus.Text       = "Waiting to update higher resolution preview...";
                hwStatus.IconID     = IconID.ExclamationTriangle;

                hwStatusMask.Text   = "U P D A T E   P E N D I N G";
            }

            hwIm.Image = currentRapidRunner.CanvasBuffer;
            hwIm.Refresh();
        }

        internal void TriggerDelayedRender()
        {
            hwTriggerRenderFull.Stop();
            hwTriggerRender.Stop();

            hwTriggerRender.Start();
        }

        private void HwTriggerRender_Tick(object sender, EventArgs e)
        {
            hwTriggerRender.Stop();
            DoRender();
        }

        private void HwTriggerRenderFull_Tick(object sender, EventArgs e)
        {
            hwTriggerRenderFull.Stop();
            DoRenderFull();
        }

        internal void SetNewCenterFromMouse(MouseEventArgs e)
        {
            var remap = new regenesis.RenderRunner.HostRemapping( EnvironmentParameters.SourceSurface.Size, hwIm.Size );
            relativePickCenter = remap.RemapHostPointRelative( new Point( e.X, e.Y ) );

            DoRender();
        }

        private void hwIm_MouseDownAndMove(object sender, MouseEventArgs e)
        {
            // don't react if we are in masked mode
            if ( hwMaskImage.Image != null )
                return;

            // when dialog boxes are used, clicks can filter through (ie double clicking on a file)
            // so we have a little hack to discard a number of potentially errant events in that case
            if ( previewClickIgnore > 0 )
            {
                previewClickIgnore--;
                return;
            }

            if ( e.Button == MouseButtons.Left )
                SetNewCenterFromMouse(e);
        }

        private void HwIm_MouseUp( object sender, MouseEventArgs e )
        {
            HwTriggerRenderFull_Tick( sender, null );
        }

        private void GenericTrackChangeHandler(object sender, TrackEventArgs e)
        {
            TriggerDelayedRender();
        }

        private void GenericClickChangeHandler(object sender, MouseEventArgs e)
        {
            TriggerDelayedRender();
            UpdateTrackEnableStates();
        }

        private void GenericPressStateChange( object sender, bool previous, bool current )
        {
            TriggerDelayedRender();
            UpdateTrackEnableStates();
        }

        private void DynamicResolutionChange( object sender, bool previous, bool current )
        {
            if ( hwDynResMed.Pressed )
            {
                ConfigRegistry.DynamicResolutionMode.Value = 1;
            }
            else if ( hwDynResHigh.Pressed )
            {
                ConfigRegistry.DynamicResolutionMode.Value = 2;
            }
            else if ( hwDynResUltra.Pressed )
            {
                ConfigRegistry.DynamicResolutionMode.Value = 3;
            }
            DoRender();
        }

        private void HwRender_PanelClicked(object sender, MouseEventArgs e)
        {
            DialogResult = DialogResult.OK;
            Close();

            // write the current config out to the config token, mark it as ready to render and tell PDN that it's ready to update
            {
                var gect = (theEffectToken as GenesisEffectConfigToken);
                PopulateTokenFromForm( gect );
                gect.TriggerFinalRender = true;
                (Effect as GenesisEffect).triggerFinalRender = true;

                FinishTokenUpdate();
            }
        }

        private void HwCancel_PanelClicked(object sender, MouseEventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();

            // mark the config as having been cancelled 
            {
                var gect = (theEffectToken as GenesisEffectConfigToken);
                gect.TriggerFinalRender = false;
                (Effect as GenesisEffect).triggerFinalRender = false;

                FinishTokenUpdate();
            }
        }

        private void HwPreview_PanelClicked( object sender, MouseEventArgs e )
        {
            var gect = (theEffectToken as GenesisEffectConfigToken);
            PopulateTokenFromForm( gect );
            gect.TriggerFinalRender = true;
            (Effect as GenesisEffect).triggerFinalRender = true;

            FinishTokenUpdate();
        }

        private void HwDiscreteRender_PanelClicked(object sender, MouseEventArgs e)
        {
            TriggerDelayedRender();
        }

        private void HwDiscreteSeed_TextChanged(object sender, EventArgs e)
        {
            TriggerDelayedRender();
        }

        private void RandomizeSeed()
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

            var random = new Random();
            var seed   = new string(Enumerable.Repeat( chars, 12 )
                                              .Select( s => s[random.Next(s.Length)])
                                              .ToArray()
                                              );

            hwDiscreteSeed.Text = seed;
        }

        private void HwRandomizeSeed_PanelClicked(object sender, MouseEventArgs e)
        {
            RandomizeSeed();
        }

        private void HwTintColour_Click(object sender, EventArgs e)
        {
            var SenderPanel = (sender as PictureBox);
            var localPt = new Point( SenderPanel.Location.X, SenderPanel.Location.Y + SenderPanel.Height );

            AsphaltColourWheel.PopupAt( SenderPanel.Parent.PointToScreen(localPt), SenderPanel.BackColor, (o, rgb) =>
            {
                SenderPanel.BackColor = rgb.ToColor();
                TriggerDelayedRender();
            });
        }

        private bool XY_ValueIsChanging = false;
        private void XY_ValueChanged(object sender, TrackEventArgs e)
        {
            if ( XY_ValueIsChanging )
                return;

            if ( !hwXYLock.Pressed )
            {
                XY_ValueIsChanging = true;
                {
                    if (sender == hwDistX)
                        hwDistY.CurrentValue = hwDistX.CurrentValue;
                    else
                        hwDistX.CurrentValue = hwDistY.CurrentValue;
                }
                XY_ValueIsChanging = false;
            }

            TriggerDelayedRender();
        }

        private void HwXYLock_PanelClicked(object sender, MouseEventArgs e)
        {
            if ( !hwXYLock.Pressed )
            {
                XY_ValueIsChanging = true;
                {
                    hwDistY.CurrentValue = hwDistX.CurrentValue;
                }
                XY_ValueIsChanging = false;

                TriggerDelayedRender();
            }
        }

        private void UpdatePickPointsFromTextInput()
        {
            float newPickX;
            if ( !float.TryParse(hwPickX.Text, out newPickX) )
            {
                hwPickX.PanelShading.PanelShader.Scheme = Scheme.KeyA;
                return;
            }
            else
                hwPickX.PanelShading.PanelShader.Scheme = Scheme.Panel;

            float newPickY;
            if ( !float.TryParse(hwPickY.Text, out newPickY) )
            {
                hwPickY.PanelShading.PanelShader.Scheme = Scheme.KeyA;
                return;
            }
            else
                hwPickY.PanelShading.PanelShader.Scheme = Scheme.Panel;

            relativePickCenter = new PointF( newPickX * 0.01f, newPickY * 0.01f );
            TriggerDelayedRender();
        }

        private void HwPickCenterXY_LoseFocus(object sender, EventArgs e)
        {
            UpdatePickPointsFromTextInput();
        }

        private void HwPickCenterXY_KeyUp(object sender, KeyEventArgs e)
        {
            if ( e.KeyCode == Keys.Enter )
            {
                UpdatePickPointsFromTextInput();
            }
        }

        private void HwSettingsReset_PanelClicked(object sender, MouseEventArgs e)
        {
            if ( MessageBox.Show("Reset all configuration values - including mask - to defaults?", hwAppTitle.Text + " - Factory Reset", MessageBoxButtons.YesNo, MessageBoxIcon.Question ) == DialogResult.Yes )
            {
                var defaultToken = CreateDefaultToken();
                InitDialogFromToken(defaultToken);
            }
        }

        private void HwMutateAll_PanelClicked(object sender, MouseEventArgs e)
        {
            {
                AsphaltTrack[] listOfTracksToRandomize =
                {
                    hwTScale,
                    hwTRotate,
                    hwTTime,
                    hwTNoiseScale,
                    hwTNoiseBoost,

                    hwMaskSmooth,
                    hwMaskTighten,
                    hwMaskPush,

                    hwDistX,
                    hwDistY,
                    hwDistZ,
                    hwDistIntensity,
                    hwDistOffset,
                    hwDistPeturb,

                    hwTVaryTime,
                    hwTVaryScale,
                    hwTVaryRotation
                };

                var random = new Random();

                foreach ( var track in listOfTracksToRandomize )
                {
                    var rngF     = (float)random.NextDouble();
                    var trackRng = track.MinimumValue + ( (track.MaximumValue - track.MinimumValue) * rngF );

                    track.CurrentValue = trackRng;
                }

                hwCNoiseAlt.Pressed   = ( random.Next() & 1 ) == 1;
                hwCNoiseBoost.Pressed = ( random.Next() & 1 ) == 1;

                switch ( random.Next(0, 3) )
                {
                    default:
                    case 0: hwMDOff.Pressed  = true; break;
                    case 1: hwMDBlob.Pressed = true; break;
                    case 2: hwMDWeb.Pressed  = true; break;
                    case 3: hwMDLump.Pressed = true; break;
                }

                RandomizeSeed();
                UpdateTrackEnableStates();
            }
        }

        private void HwDistZLabel_PanelClicked( object sender, MouseEventArgs e )
        {
            TriggerDelayedRender();
        }

        static List<(string, object)> OverrideOptions = new List<(string, object)>()
        {
            ("No Override",        -1),
            ("Light",               0),
            ("Object",              1),
            ("Add",                 3),
            ("Subtract",            4),
            ("Angular Displace I",  8),
            ("Radial Displace I",   10),
            ("Image Warp I",        12),
            ("Image Warp RGB",      13),
        };

        private void UpdateOverrideText()
        {
            if ( hwOverload.Tag != null )
            {
                int tagId = (int)hwOverload.Tag;

                foreach ( var ovo in OverrideOptions )
                {
                    if ( (int)ovo.Item2 == tagId )
                    {
                        hwOverload.Text = ovo.Item1;
                        return;
                    }
                }
            }
        }

        private void HwOverload_PanelClicked( object sender, MouseEventArgs e )
        {
            var SenderPanel = (sender as AsphaltPanel);

            DropDownHelper.DoPopUp(
                SenderPanel,
                OverrideOptions,
                ( text, id ) =>
                {
                    SenderPanel.Text = text;
                    SenderPanel.Tag = id;
                    DoRender();
                } );
        }
    }
}
