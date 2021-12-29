/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Collections.Generic;
using System.Windows.Forms;

using Asphalt.Controls;
using Asphalt.Controls.Theme;

namespace GenesisVFX
{
    public partial class ExplorerForm
    {
        private Control[] _DiscreteControls;
        private Control[] _DistortControls;
        private Control[] _EffectCenterControls;
        private Control[] _MaskClipControls;

        private void UXInit()
        {
            _DiscreteControls = new Control[]
            {
                hwVaryRandomLabel,
                hwVaryRotationLabel,
                hwVaryScaleLabel,
                hwTVaryTime,
                hwTVaryRotation,
                hwTVaryScale
            };

            _DistortControls = new Control[]
            {
                hwDistX,
                hwDistY,
                hwXYLock,
                hwDistZ,
                hwDistIntensity,
                hwDistZLabel,
                hwDistOffset,
                hwDistOffsetLabel,
                hwDistPeturb,
                hwDistPeturbLabel
            };

            _EffectCenterControls = new Control[]
            {
                hwPickX,
                hwPickXLabel,
                hwPickY,
                hwPickYLabel,
                hwCenterLabel
            };

            _MaskClipControls = new Control[]
            {
                hwOutClippingLabel,
                hwOutputClipNone,
                hwOutputClipInner,
                hwOutputClipOuter
            };

            var _Tooltips = new Dictionary<Control, string>()
            {
                {               hwLoadPacks, "Choose a directory to search for effect files"                                                                         },

                {          hwBackgroundPull, "Load the background image from Paint.NET"                                                                              },
                {         hwBackgroundClear, "Switch to using a solid clear colour as a background"                                                                  },
                {        hwClearColourLabel, "The colour to use as a background, if solid clear mode is enabled"                                                     },
                {    hwBackgroundAlphaLabel, "The alpha value to use for the background, if solid clear mode is enabled"                                             },

                {          hwRendScaleLabel, "Global effect scale"                                                                                                   },
                {         hwRendRotateLabel, "Global effect rotation"                                                                                                },
                {        hwRendSpatialLabel, "Global effect time"                                                                                                    },

                {             hwCenterLabel, "Relative position in the image to render the effect\nIgnored if a mask is in use"                                      },
                {           hwVarySeedLabel, "Random seed used by various subsystems"                                                                                },
                {           hwRandomizeSeed, "Choose a new random seed value"                                                                                        },
                {           hwRendTintLabel, "Colour tint applied during effect rendering"                                                                           },

                {       hwRendDisplaceLabel, "When an effect uses displacement, these options control the quality of the sampling used"                              },
                {            hwDisplaceFast, "Default displacement sampling of 1-sample per pixel\nOn large displacements, this will lead to aliasing"               },
                {              hwDisplaceMS, "Multisampled displacement, with controllable sample distance\nThe larger the distance, the more samples will be taken" },
                {                 hwTMSDist, "Distance to sample when using multisampled displacement"                                                               },

                {               hwCNoiseAlt, "Use an alternative, intricate noise override for effects that use noise"                                               },
                {        hwTNoiseScaleLabel, "Global scale for noise"                                                                                                },
                {             hwCNoiseBoost, "Increase the noise variation across the image"                                                                         },

                {               hwDynResMed, "Slowest preview mode;\nHalf-resolution interactive preview, full resolution resolve"                                   },
                {              hwDynResHigh, "Quarter-resolution interactive preview, full resolution resolve"                                                       },
                {             hwDynResUltra, "Fastest preview mode;\nRender interactively at 1/8th resolution, half-resolution resolve"                              },

                {           hwSettingsReset, "Reset all settings to their defaults"                                                                                  },
                {               hwMutateAll, "Randomise settings"                                                                                                    },

                {                hwMaskPull, "Load a mask from Paint.NET"                                                                                            },
                {                hwMaskLoad, "Load a mask from an image on disk"                                                                                     },
                {                hwMaskClip, "Set a mask from the current Paint.NET selection"                                                                       },
                {              hwMaskInvert, "Invert the current mask"                                                                                               },
                {               hwMaskClear, "Clear the mask"                                                                                                        },

                {              hwStatusMask, "Once a high-resolution render has completed, this will report how many separate mask elements were found and used"     },
                {         hwMaskThreshLabel, "Masks are black & white, so we turn input images into on/off masks by thresholding on luminance\nThis value controls the clipping point - lower values means a larger luminance range is considered" },
                {           hwMaskSizeLabel, "When luminance-clipping a complex image, you may end up with single isolated pixels being considered a separate element\nMerging will glom elements together if they are small enough; the higher this value, the more elements may be considered too small and be combined\nNOTE this is very resolution dependant and may not preview correctly" },

                {         hwMaskSmoothLabel, "Controls how much to 'blur' the envelope formed around the mask input; higher values mean softer, rounder effect edges" },
                {           hwMaskPushLabel, "Controls the magnitude of the effect while wrapped around the mask; higher values pushes the effect out from the edges of the mask elements" },
                {        hwMaskTightenLabel, "Think of this as the 'contrast' of the mask envelope; similar to Push, but applied after any distortion\nAt higher values, this can make effects look crisper; at lower values, softer" },
                {     hwMaskDistortionLabel, "Optionally apply noise distortion to the mask envelope"                                                                },

                {                  hwXYLock, "How much distortion to apply in the X/Y axes; click to toggle locking the values"                                      },
                {              hwDistZLabel, "Distortion to apply 'through' the mask envelope"                                                                       },
                {         hwDistOffsetLabel, "Shift the noise distortion to make it more or less severe to the original mask envelope values"                        },
                {         hwDistPeturbLabel, "How much secondary curl displacement to apply, giving more of a smoke-like look at smaller values and a kind of dense-whorl effect at higher ones" },
                {          hwDiscreteRender, "If your mask has multiple elements, enable this to have them rendered as individual layers (potentially greatly increasing render time)\nThis lets you also vary scale, rotation and spatial values per layer to make more varied results" },

                {                  hwRender, "Close the plug-in, use the current settings and render out the full-resolution result"                                 },
                {                  hwCancel, "Close the plug-in, do not apply any changes"                                                                           },
            };

            foreach ( var tip in _Tooltips )
                hwToolTips.SetToolTip( tip.Key, tip.Value );
        }
        private bool IsMaskLoaded => (hwMaskImage.Image != null);

        private void UpdateTrackEnableStates()
        {
            bool enableDiscreteControls = hwDiscreteRender.Pressed && IsMaskLoaded;
            foreach ( Control c in _DiscreteControls )
                c.Enabled = enableDiscreteControls;

            bool enableDistortControls = !hwMDOff.Pressed && IsMaskLoaded;
            foreach ( Control c in _DistortControls )
                c.Enabled = enableDistortControls;

            hwTNoiseBoost.Enabled   = hwCNoiseBoost.Pressed;
            hwTMSDist.Enabled       = hwDisplaceMS.Pressed;
        }

        private void UpdateMaskEnableStates()
        {
            bool hasMask = IsMaskLoaded;

            // toggle all controls in the mask control container
            foreach ( Control ctl in hwMaskControlsCt.Controls )
                ctl.Enabled = hasMask;

            foreach ( Control ctl in _MaskClipControls )
                ctl.Enabled = hasMask;

            // .. and invert-toggle the effect center, as it is ignored if we use a mask
            foreach ( Control c in _EffectCenterControls )
                c.Enabled = !hasMask;

            hwStatusMask.Enabled = true;

            UpdateTrackEnableStates();
        }
    }
}
