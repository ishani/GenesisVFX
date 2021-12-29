/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Diagnostics;
using System.Drawing;

using PaintDotNet;
using PaintDotNet.Effects;

namespace GenesisVFX
{
    public partial class ExplorerForm
    {
        protected override void InitDialogFromToken(EffectConfigToken effectTokenCopy)
        {
            CreateBackgroundImages();

            renderRunnerFull     = new regenesis.RenderRunner( hwIm.Size, EnvironmentParameters.SourceSurface.Size, 1 );
            renderRunnerHalf     = new regenesis.RenderRunner( hwIm.Size, EnvironmentParameters.SourceSurface.Size, 2 );
            renderRunnerQuarter  = new regenesis.RenderRunner( hwIm.Size, EnvironmentParameters.SourceSurface.Size, 4 );
            renderRunnerEighth   = new regenesis.RenderRunner( hwIm.Size, EnvironmentParameters.SourceSurface.Size, 8 );

            var dynResMode = ConfigRegistry.DynamicResolutionMode.Value;
            switch ( dynResMode )
            {
                default:
                case 1: hwDynResMed.Pressed   = true; break;
                case 2: hwDynResHigh.Pressed  = true; break;
                case 3: hwDynResUltra.Pressed = true; break;
            }

            var gect = (effectTokenCopy as GenesisEffectConfigToken);

            if ( !gect.PickedPoint.HasValue )
                relativePickCenter              = new PointF( 0.4f, 0.4f );
            else
                relativePickCenter              = gect.PickedPoint.Value;

            if (gect.UseBackgroundSourceImage)
                hwBackgroundPull.Pressed = true;
            else
                hwBackgroundClear.Pressed = true;

            hwBackgroundClearColour.BackColor   = gect.BackgroundClearColour;
            hwBackgroundClearAlpha.CurrentValue = (float)gect.BackgroundClearAlpha;
            hwTintColour.BackColor              = gect.TintColour;

            hwCProtectAlpha.Pressed             = gect.ProtectAlpha;

            hwTScale.CurrentValue               = gect.GlobalScale;
            hwTRotate.CurrentValue              = gect.GlobalRotate;
            hwTTime.CurrentValue                = gect.Time;

            hwTHueShift.CurrentValue            = gect.GlobalHueShift;
            hwTSatShift.CurrentValue            = gect.GlobalSaturationShift;
            hwTValueShift.CurrentValue          = gect.GlobalValueShift;

            if ( gect.Displace == GenesisEffectConfigToken.DisplaceMode.Default )
                hwDisplaceFast.Pressed          = true;
            else
                hwDisplaceMS.Pressed            = true;

            hwTMSDist.CurrentValue              = gect.DisplaceMS;

            hwTNoiseScale.CurrentValue          = gect.NoiseScale;
            hwCNoiseBoost.Pressed               = gect.NoiseBoost;
            hwTNoiseBoost.CurrentValue          = gect.NoiseBoostAmount;
            hwCNoiseAlt.Pressed                 = gect.HadesNoise;

            hwDiscreteSeed.Text                 = gect.RandomSeed;

            hwMaskThreshold.CurrentValue        = gect.MaskThreshold;
            hwMaskSizeClip.CurrentValue         = gect.MaskSizeClip;
            hwMaskSmooth.CurrentValue           = gect.MaskSmoothing;
            hwMaskPush.CurrentValue             = gect.MaskPush;
            hwMaskTighten.CurrentValue          = gect.MaskTighten;

            hwDiscreteRender.Pressed            = gect.MaskDiscreteRender;
            hwTVaryTime.CurrentValue            = gect.MaskVaryTime;
            hwTVaryRotation.CurrentValue        = gect.MaskVaryRotation;
            hwTVaryScale.CurrentValue           = gect.MaskVaryScale;

            switch ( gect.MaskDistortionMode )
            {
                case GenesisEffectConfigToken.MaskDistort.Blob:
                    hwMDBlob.Pressed = true;
                    break;
                case GenesisEffectConfigToken.MaskDistort.Web:
                    hwMDWeb.Pressed = true;
                    break;
                case GenesisEffectConfigToken.MaskDistort.Lump:
                    hwMDLump.Pressed = true;
                    break;

                default:
                    hwMDOff.Pressed = true;
                    break;
            }

            hwDistPeturb.CurrentValue           = gect.MaskDistortionPerturb;
            hwDistOffset.CurrentValue           = gect.MaskDistortionOffset;

            hwDistX.CurrentValue                = gect.MaskDistortionX;
            hwDistY.CurrentValue                = gect.MaskDistortionY;
            hwDistZ.CurrentValue                = gect.MaskDistortionZ;
            hwDistIntensity.CurrentValue        = gect.MaskDistortionI;

            hwDistZLabel.Pressed                = !gect.MaskDistortionAltZAxis;

            switch ( gect.MaskClipping )
            {
                default:
                case GenesisEffectConfigToken.MaskClip.None:
                    hwOutputClipNone.Pressed = true;
                    break;
                case GenesisEffectConfigToken.MaskClip.Inner:
                    hwOutputClipInner.Pressed = true;
                    break;
                case GenesisEffectConfigToken.MaskClip.Outer:
                    hwOutputClipOuter.Pressed = true;
                    break;
            }

            if ( !string.IsNullOrEmpty( gect.EffectLibrary ) )
            {
                LoadCatalog( gect.EffectLibrary );
            }
            if ( gect.EffectFile != null )
            {
                CurrentGfxFile = gect.EffectFile;
            }

            // will also trigger disable-state update
            SetMask( gect.MaskImage );

            hwOverload.Tag = gect.AllElementOverload;
            UpdateOverrideText();
        }

        protected GenesisEffectConfigToken CreateDefaultToken()
        {
            var defaultToken = new GenesisEffectConfigToken()
            {
                TriggerFinalRender          = false,

                PickedPoint                 = null,

                UseBackgroundSourceImage    = true,
                BackgroundClearColour       = Color.Black,
                BackgroundClearAlpha        = 255,
                TintColour                  = Color.White,

                ProtectAlpha                = true,

                GlobalScale                 = 1.0f,
                GlobalRotate                = 0.0f,
                Time                        = 0.0f,

                GlobalHueShift              = 0.0f,
                GlobalSaturationShift       = 0.0f,
                GlobalValueShift            = 0.0f,

                Displace                    = GenesisEffectConfigToken.DisplaceMode.Default,

                DisplaceMS                  = 0.1f,

                NoiseScale                  = 1.0f,
                NoiseBoost                  = false,
                NoiseBoostAmount            = 16.0f,
                HadesNoise                  = false,

                RandomSeed                  = "------------",

                MaskThreshold               = 0.5f,
                MaskSizeClip                = 0.0f,
                MaskSmoothing               = 0.1f,
                MaskPush                    = 1.0f,
                MaskTighten                 = 0.0f,

                MaskDiscreteRender          = false,
                MaskVaryTime                = 0.0f,
                MaskVaryRotation            = 0.0f,
                MaskVaryScale               = 0.0f,

                MaskDistortionMode          = GenesisEffectConfigToken.MaskDistort.None,
                MaskDistortionPerturb       = 0.1f,
                MaskDistortionOffset        = 0.5f,
                MaskDistortionX             = 0.0f,
                MaskDistortionY             = 0.0f,
                MaskDistortionZ             = 0.0f,
                MaskDistortionI             = 1.0f,
                MaskDistortionAltZAxis      = false,

                MaskClipping                = GenesisEffectConfigToken.MaskClip.None,

                AllElementOverload          = -1
            };
            return defaultToken;
        }

        protected override void InitialInitToken()
        {
            theEffectToken = CreateDefaultToken();
        }

        protected void PopulateTokenFromForm(GenesisEffectConfigToken gect)
        {
            gect.EffectFile                 = _currentGfxFile;
            gect.EffectLibrary              = _currentGfxLibrary;

            gect.PickedPoint                = relativePickCenter;

            gect.UseBackgroundSourceImage   = hwBackgroundPull.Pressed;
            gect.BackgroundClearColour      = hwBackgroundClearColour.BackColor;
            gect.BackgroundClearAlpha       = (int)Math.Round(hwBackgroundClearAlpha.CurrentValue);
            gect.TintColour                 = hwTintColour.BackColor;

            gect.ProtectAlpha               = hwCProtectAlpha.Pressed;

            gect.GlobalScale                = hwTScale.CurrentValue;
            gect.GlobalRotate               = hwTRotate.CurrentValue;
            gect.Time                       = hwTTime.CurrentValue;

            gect.GlobalHueShift             = hwTHueShift.CurrentValue;
            gect.GlobalSaturationShift      = hwTSatShift.CurrentValue;
            gect.GlobalValueShift           = hwTValueShift.CurrentValue;

            if ( hwDisplaceFast.Pressed )
                gect.Displace               = GenesisEffectConfigToken.DisplaceMode.Default;
            else
                gect.Displace               = GenesisEffectConfigToken.DisplaceMode.Multisampled;

            gect.DisplaceMS                 = hwTMSDist.CurrentValue;

            gect.NoiseScale                 = hwTNoiseScale.CurrentValue;
            gect.NoiseBoost                 = hwCNoiseBoost.Pressed;
            gect.NoiseBoostAmount           = hwTNoiseBoost.CurrentValue;
            gect.HadesNoise                 = hwCNoiseAlt.Pressed;

            gect.RandomSeed                 = hwDiscreteSeed.Text;

            gect.MaskImage                  = hwMaskImage.Tag as Bitmap;

            gect.MaskThreshold              = hwMaskThreshold.CurrentValue;
            gect.MaskSizeClip               = hwMaskSizeClip.CurrentValue;
            gect.MaskSmoothing              = hwMaskSmooth.CurrentValue;
            gect.MaskPush                   = hwMaskPush.CurrentValue;
            gect.MaskTighten                = hwMaskTighten.CurrentValue;

            gect.MaskDiscreteRender         = hwDiscreteRender.Pressed;
            gect.MaskVaryTime               = hwTVaryTime.CurrentValue;
            gect.MaskVaryRotation           = hwTVaryRotation.CurrentValue;
            gect.MaskVaryScale              = hwTVaryScale.CurrentValue;

            gect.MaskDistortionMode         = GenesisEffectConfigToken.MaskDistort.None;
            if ( hwMDBlob.Pressed )
                gect.MaskDistortionMode     = GenesisEffectConfigToken.MaskDistort.Blob;
            else if ( hwMDWeb.Pressed )
                gect.MaskDistortionMode     = GenesisEffectConfigToken.MaskDistort.Web;
            else if ( hwMDLump.Pressed )
                gect.MaskDistortionMode     = GenesisEffectConfigToken.MaskDistort.Lump;

            gect.MaskDistortionPerturb      = hwDistPeturb.CurrentValue;
            gect.MaskDistortionOffset       = hwDistOffset.CurrentValue;

            gect.MaskDistortionX            = hwDistX.CurrentValue;
            gect.MaskDistortionY            = hwDistY.CurrentValue;
            gect.MaskDistortionZ            = hwDistZ.CurrentValue;
            gect.MaskDistortionI            = hwDistIntensity.CurrentValue;

            gect.MaskDistortionAltZAxis     = !hwDistZLabel.Pressed;
            
            gect.MaskClipping               = GenesisEffectConfigToken.MaskClip.None;
            if ( hwOutputClipInner.Pressed )
                gect.MaskClipping           = GenesisEffectConfigToken.MaskClip.Inner;
            else if ( hwOutputClipOuter.Pressed )
                gect.MaskClipping           = GenesisEffectConfigToken.MaskClip.Outer;

            if ( hwOverload.Tag != null )
            {
                int overrideId = (int)hwOverload.Tag;
                gect.AllElementOverload = overrideId;
            }
        }

        protected override void InitTokenFromDialog()
        {
            var gect = (theEffectToken as GenesisEffectConfigToken);
            PopulateTokenFromForm( gect );
        }

        protected override void OnEffectTokenChanged()
        {

            base.OnEffectTokenChanged();
        }

        internal static void TokenToRenderRunner( in GenesisEffectConfigToken gect, in Size imageSize, regenesis.RenderRunner rr )
        {
            Rectangle fitDim = regenesis.Utils.AspectFitImage( imageSize.Width, imageSize.Height, rr.PipelineWidth, rr.PipelineHeight );

            var minClipSize = (float)( rr.PipelineWidth * rr.PipelineHeight ) * 0.01f;

            var hostRelativePoint = new Point(
                fitDim.X + (int)( Math.Round( gect.PickedPoint.Value.X, 3 ) * (float)fitDim.Width  ),
                fitDim.Y + (int)( Math.Round( gect.PickedPoint.Value.Y, 3 ) * (float)fitDim.Height )
                );

            UInt32 randomSeedFromString = (UInt32)gect.RandomSeed.GetHashCode();

            rr.RenderGlobals.CenterX = hostRelativePoint.X;
            rr.RenderGlobals.CenterY = hostRelativePoint.Y;

            var clearColour = Color.FromArgb( gect.BackgroundClearAlpha, gect.BackgroundClearColour );

            rr.RenderGlobals.BackgroundColour.SetFrom( clearColour );
            rr.RenderGlobals.ColouriseColour.SetFrom( gect.TintColour );

            rr.RenderGlobals.GlobalScale                            = gect.GlobalScale;
            rr.RenderGlobals.GlobalRotate                           = gect.GlobalRotate * 512.0f;
            rr.RenderGlobals.Time                                   = gect.Time;

            rr.RenderGlobals.ProtectAlpha                           = gect.ProtectAlpha;

            rr.RenderGlobals.GlobalHueShift                         = gect.GlobalHueShift;
            rr.RenderGlobals.GlobalSaturationShift                  = gect.GlobalSaturationShift;
            rr.RenderGlobals.GlobalValueShift                       = gect.GlobalValueShift;

            if ( gect.Displace == GenesisEffectConfigToken.DisplaceMode.Default )
            {
                rr.RenderGlobals.DisplacementSmoothing              = false;
            }
            else
            {
                rr.RenderGlobals.DisplacementSmoothing              = true;
                rr.RenderGlobals.DisplacementSmoothingDistance      =      FloatUtil.Lerp( 0.5f, 22.5f, gect.DisplaceMS ) * rr.PipelineScale;
                rr.RenderGlobals.DisplacementSmoothingSampleGrid    = (int)FloatUtil.Lerp( 3.0f, 10.0f, gect.DisplaceMS );
            }

            rr.RenderGlobals.GlobalNoiseScale                       = gect.NoiseScale;

            rr.RenderGlobals.BoostNoiseVariance                     = gect.NoiseBoost;
            rr.RenderGlobals.BoostNoiseScale                        = gect.NoiseBoostAmount;
            rr.RenderGlobals.UseHadesNoise                          = gect.HadesNoise;

            rr.RenderGlobals.ShapeMaskExtractionThreshold           = gect.MaskThreshold;
            rr.RenderGlobals.ShapeMaskExtractionMinElemSizePixels   = (UInt32)Math.Round( FloatUtil.Lerp(1.0f, minClipSize, gect.MaskSizeClip) );
            rr.RenderGlobals.ShapeMaskBlurSize                      = gect.MaskSmoothing * 300.0f * rr.PipelineScale;
            rr.RenderGlobals.ShapeMaskDistanceMultiplier            = gect.MaskPush;

            if ( gect.MaskTighten < 0.0f )
                rr.RenderGlobals.ShapeMaskTighten                   = FloatUtil.Lerp( 1.0f,  2.0f, -gect.MaskTighten );
            else
                rr.RenderGlobals.ShapeMaskTighten                   = FloatUtil.Lerp( 1.0f, 0.01f,  gect.MaskTighten );

            rr.RenderGlobals.RenderMaskElementsSeparately           = gect.MaskDiscreteRender;
            rr.RenderGlobals.PerElementRandomisationSeed            = randomSeedFromString;
            rr.RenderGlobals.PerElementRandomisationTime            = gect.MaskVaryTime * 50.0f;
            rr.RenderGlobals.PerElementRandomisationRotation        = gect.MaskVaryRotation * 512.0f;
            rr.RenderGlobals.PerElementRandomisationScaleMin        = FloatUtil.Lerp( 1.0f, 0.1f, gect.MaskVaryScale );
            rr.RenderGlobals.PerElementRandomisationScaleMax        = FloatUtil.Lerp( 1.0f, 8.0f, gect.MaskVaryScale );

            switch ( gect.MaskDistortionMode )
            {
                case GenesisEffectConfigToken.MaskDistort.None: rr.RenderGlobals.ShapeMaskDistortionMode = regenesis.MaskDistortion.None;    break;
                case GenesisEffectConfigToken.MaskDistort.Blob: rr.RenderGlobals.ShapeMaskDistortionMode = regenesis.MaskDistortion.Noise;   break;
                case GenesisEffectConfigToken.MaskDistort.Web:  rr.RenderGlobals.ShapeMaskDistortionMode = regenesis.MaskDistortion.Celluar; break;
                case GenesisEffectConfigToken.MaskDistort.Lump: rr.RenderGlobals.ShapeMaskDistortionMode = regenesis.MaskDistortion.Lump;    break;
            }

            rr.RenderGlobals.ShapeMaskRandomSeed                    = randomSeedFromString;
            rr.RenderGlobals.ShapeMaskDistortionPerturbAmount       = FloatUtil.Lerp( 0.0f, 300.0f, gect.MaskDistortionPerturb );
            rr.RenderGlobals.ShapeMaskDistortionOffset              = gect.MaskDistortionOffset;

            rr.RenderGlobals.ShapeMaskDistortionX                   = FloatUtil.Lerp( 0.0f, 3200.0f, gect.MaskDistortionX );
            rr.RenderGlobals.ShapeMaskDistortionY                   = FloatUtil.Lerp( 0.0f, 3200.0f, gect.MaskDistortionY );
            rr.RenderGlobals.ShapeMaskDistortionZ                   = FloatUtil.Lerp( 0.0f,   80.0f, gect.MaskDistortionZ );
            rr.RenderGlobals.ShapeMaskDistortionI                   = gect.MaskDistortionI;
            rr.RenderGlobals.ShapeMaskAltZAxis                      = gect.MaskDistortionAltZAxis;

            switch ( gect.MaskClipping )
            {
                case GenesisEffectConfigToken.MaskClip.None:  rr.RenderGlobals.ShapeMaskPunchOut = regenesis.MaskPunchOut.None;     break;
                case GenesisEffectConfigToken.MaskClip.Inner: rr.RenderGlobals.ShapeMaskPunchOut = regenesis.MaskPunchOut.Inner;    break;
                case GenesisEffectConfigToken.MaskClip.Outer: rr.RenderGlobals.ShapeMaskPunchOut = regenesis.MaskPunchOut.Outer;    break;
            }

            rr.RenderGlobals.AllElementOverload                     = gect.AllElementOverload;
        }
    }
}
