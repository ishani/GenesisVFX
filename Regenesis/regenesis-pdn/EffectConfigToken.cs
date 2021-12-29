/*
    GenesisVFX for Paint.NET, part of the regenesis project
    Harry Denholm, ishani.org 2018-2021
*/

using System;
using System.Drawing;

using PaintDotNet.Effects;

namespace GenesisVFX
{
    [Serializable]
    public class GenesisEffectConfigToken : EffectConfigToken
    {
        public enum DisplaceMode
        {
            Default,
            Multisampled
        }

        public enum MaskClip
        {
            None,
            Inner,
            Outer
        }

        public enum MaskDistort
        {
            None,
            Blob,
            Web,
            Lump
        }

        public bool                     TriggerFinalRender;

        public string                   EffectFile;
        public string                   EffectLibrary;

        public PointF?                  PickedPoint;

        public bool                     UseBackgroundSourceImage;
        public Color                    BackgroundClearColour;
        public int                      BackgroundClearAlpha;
        public Color                    TintColour;

        public bool                     ProtectAlpha;

        public float                    GlobalScale;
        public float                    GlobalRotate;
        public float                    Time;

        public float                    GlobalHueShift;
        public float                    GlobalSaturationShift;
        public float                    GlobalValueShift;

        public DisplaceMode             Displace;
        public float                    DisplaceMS;

        public float                    NoiseScale;
        public bool                     NoiseBoost;
        public float                    NoiseBoostAmount;
        public bool                     HadesNoise;

        public string                   RandomSeed;

        public Bitmap                   MaskImage;

        public float                    MaskThreshold;
        public float                    MaskSizeClip;
        public float                    MaskSmoothing;
        public float                    MaskPush;
        public float                    MaskTighten;

        public bool                     MaskDiscreteRender;
        public float                    MaskVaryTime;
        public float                    MaskVaryRotation;
        public float                    MaskVaryScale;

        public MaskDistort              MaskDistortionMode;
        public float                    MaskDistortionPerturb;
        public float                    MaskDistortionOffset;

        public float                    MaskDistortionX;
        public float                    MaskDistortionY;
        public float                    MaskDistortionZ;
        public float                    MaskDistortionI;
        public bool                     MaskDistortionAltZAxis;

        public MaskClip                 MaskClipping;

        public Int32                    AllElementOverload;

        public override object Clone()
        {
            var cloned        = this.MemberwiseClone() as GenesisEffectConfigToken;

            if ( EffectFile != null )
                cloned.EffectFile = string.Copy( EffectFile );

            if ( EffectLibrary != null )
                cloned.EffectLibrary = string.Copy( EffectLibrary );

            if ( RandomSeed != null )
                cloned.RandomSeed = string.Copy( RandomSeed );

            if ( MaskImage != null )
                cloned.MaskImage = new Bitmap( MaskImage );

            return cloned;
        }
    }
}
