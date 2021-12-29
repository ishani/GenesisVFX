/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

// ------------------------------------------------------------------------------------------------
// DLL
//      This project exposes the Regenesis core to a .NET-compatible DLL; specifically, it can 
//      consume and produce .NET bitmap data.
//      It produces the DLL used by the Paint.NET plugin.
// ------------------------------------------------------------------------------------------------



#include "rgCommon.h"
#include "rgAPI.h"
#include "rgColour.h"
#include "rgIO.h"
#include "rgNoise.h"
#include "rgBuffer2D.h"
#include "rgDeepMask.h"
#include "rgRender.h"
#include "rgMath.h"


#include <stdarg.h>
#include <iostream>

#ifdef _MSC_VER
#include <windows.h>
#define RGDLL       __declspec( dllexport )
#else
#define RGDLL       __attribute__ ((visibility ("default")))
#define CALLBACK
#endif

// ------------------------------------------------------------------------------------------------

// the following are shared data structures; if you change these, you'll need to update the P/Invoke-side
// in .NET code

extern "C" {
#pragma pack(push, 1)

    enum class MaskDistortion : int32_t
    {
        None    = 0,
        Noise   = 1,
        Celluar = 2,
        Lump    = 3
    };

    enum class MaskPunchOut : int32_t
    {
        None    = 0,
        Outer   = 1,
        Inner   = 2
    };

    struct IntColour8
    {
        int8_t          R;
        int8_t          G;
        int8_t          B;
        int8_t          A;

        inline ColourF toColourF() const
        {
            ColourI8 bgi8( R, G, B, A );
            return ColourF( bgi8 );
        }
    };

    struct PixelData
    {
        int32_t         Width;
        int32_t         Height;
        int32_t         Stride;
        void*           Scan0;
    };

    struct Images
    {
        PixelData       Background;
        PixelData       Mask;

        PixelData       Output;

        float           LongestEdge;
    };

    struct Globals
    {
        float           CenterX;
        float           CenterY;

        IntColour8      BackgroundColour;
        IntColour8      ColouriseColour;

        float           Time;

        float           GlobalScale;
        float           GlobalRotate;

        // tweaks

        float           GlobalHueShift;
        float           GlobalSaturationShift;
        float           GlobalValueShift;

        bool            ProtectAlpha;

        bool            DisplacementSmoothing;
        float           DisplacementSmoothingDistance;
        int32_t         DisplacementSmoothingSampleGrid;

        bool            BoostNoiseVariance;
        float           BoostNoiseScale;

        bool            UseHadesNoise;
        float           GlobalNoiseScale;

        // mask stuff

        bool            RenderMaskElementsSeparately;          // instead of producing a single SDF for the mask, render each separate component and then blend them
        uint32_t        PerElementRandomisationSeed;
        float           PerElementRandomisationTime;
        float           PerElementRandomisationRotation;
        float           PerElementRandomisationScaleMin;
        float           PerElementRandomisationScaleMax;

        float           ShapeMaskExtractionThreshold;
        uint32_t        ShapeMaskExtractionMinElementSizeInPixels;
        float           ShapeMaskBlurSize;
        float           ShapeMaskDistanceMultiplier;
        float           ShapeMaskTighten;

        uint32_t        ShapeMaskRandomSeed;
        MaskDistortion  ShapeMaskDistortionMode;
        float           ShapeMaskDistortionPerturbAmount;
        float           ShapeMaskDistortionX;
        float           ShapeMaskDistortionY;
        float           ShapeMaskDistortionZ;
        float           ShapeMaskDistortionOffset;
        float           ShapeMaskDistortionI;
        bool            ShapeMaskAltZAxis;

        MaskPunchOut    ShapeMaskPunchOut;

        int32_t         AllElementOverload;
    };

    struct ResultMeta
    {
        float           RenderTime;
        uint32_t        MaskElements;
        char            EffectName[256];
    };

#pragma pack(pop)

    // ------------------------------------------------------------------------------------------------
    // our .NET public API

    #define API_VERSION 300

    RGDLL uint32_t RgAPI()
    {
        return API_VERSION;
    }

    // string to throw on the UI just to track which build you're running
    RGDLL const char* RgVersion()
    {
        return rg_stringify(API_VERSION) " : " OPT_VERSION;
    }

    typedef bool ( CALLBACK *LogFeedback )( int32_t level, const char* str );
    LogFeedback g_logFunction = nullptr;

    RGDLL void SetLoggingFn( LogFeedback logFn )
    {
        g_logFunction = logFn;
        if ( g_logFunction != nullptr )
            g_logFunction( (int32_t) LogLevel::Default, "Logging callback set" );
    }


    // the main event
    RGDLL bool RenderFile(
        const char*     arg_gfxFile, 
        const Globals*  arg_globals, 
        const Images*   arg_images, 
        ResultMeta*     out_meta );

    RGDLL bool RenderMemory(
        const uint8_t*  arg_gfxInMemory,
        const int       arg_gfxInMemorySize,
        const Globals*  arg_globals, 
        const Images*   arg_images, 
        ResultMeta*     out_meta );

} // extern "C"



// ------------------------------------------------------------------------------------------------
// implementation of the required core library functions

void rgLogOutput( const LogLevel ll, const char* fmt, ... )
{
    va_list args;

    char buf[1000];

    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );

    if ( g_logFunction != nullptr )
        g_logFunction( ( int32_t)ll, buf );
}

void rgAssertHandler( const char* func, const uint64_t line, const char* failed_expr )
{
    rgLogOutput( LogLevel::Error, "[Assert Failed]\n%s (%" PRIu64 ") - %s\n\n", func, line, failed_expr );
}

void rgProgressHandler( const char* task, const float current, const float total )
{
}



// ------------------------------------------------------------------------------------------------
// turn a .NET pixel buffer into an I8 buffer for use with Regenesis
//
ColourI8Buffer imageFromPixelBits( const PixelData* incoming )
{
    ColourI8Buffer image( incoming->Width, incoming->Height );

    const char* top = (const char*)incoming->Scan0;

    AUTO_PARALLELIZE_LOOP
    for ( int32_t y = 0; y < incoming->Height; y++ )
    {
        const char* scanline = &top[ y * incoming->Stride ];
        for ( int32_t x = 0; x < incoming->Width; x++ )
        {
            const char* block = scanline + ( x * 4 );

            // .NET = B G R A
            image( x, y ).Set(
                block[2],
                block[1],
                block[0],
                block[3]
            );
        }
    }

    return std::move( image );
}

// ------------------------------------------------------------------------------------------------
// ... and vice versa, turn an I8 buffer into a .NET pixel buffer
//
void pixelBitsFromImage( const ColourI8Buffer& image, const PixelData* outgoing )
{
    char* top = (char*)outgoing->Scan0;

    AUTO_PARALLELIZE_LOOP
    for ( int32_t y = 0; y < outgoing->Height; y++ )
    {
        char* scanline = &top[y * outgoing->Stride];
        for ( int32_t x = 0; x < outgoing->Width; x++ )
        {
            char* block = scanline + ( x * 4 );

            ColourI8 pxc = image( x, y );
            
            // .NET = B G R A
            block[0] = pxc.b;
            block[1] = pxc.g;
            block[2] = pxc.r;
            block[3] = pxc.a;
        }
    }

}


// ------------------------------------------------------------------------------------------------
bool Render(
    const GfxFile&  arg_gfxFile,
    const Globals*  arg_globals,
    const Images*   arg_images,
    ResultMeta*     out_meta )
{
    Timer blockTimer;

    strcpy( out_meta->EffectName, arg_gfxFile.header.m_name );

    if ( arg_images->Output.Scan0 == nullptr )
    {
        rgLogOutput( LogLevel::Error, "Output image buffer undefined" );
        return false;
    }

    const int32_t pipelineWidth  = arg_images->Output.Width;
    const int32_t pipelineHeight = arg_images->Output.Height;

    ColourFBuffer previousLayer( pipelineWidth, pipelineHeight );

    if ( arg_images->Background.Scan0 != nullptr )
    {
        ColourI8Buffer backgroundI8 = imageFromPixelBits( &arg_images->Background );
        previousLayer.convertFrom( backgroundI8 );
    }
    else
    {
        ColourF bgF = arg_globals->BackgroundColour.toColourF();
        previousLayer.clear( bgF );
    }


    DeepMask* deepMask = nullptr;
    ColourFBuffer* maskBuffer = nullptr;
    ColourFBuffer* maskOrigin = nullptr;

    if ( arg_images->Mask.Scan0 != nullptr )
    {
        ColourI8Buffer maskI8 = imageFromPixelBits( &arg_images->Mask );

        maskBuffer = new ColourFBuffer( pipelineWidth, pipelineHeight );
        maskBuffer->convertFrom( maskI8 );

        deepMask = new DeepMask( pipelineWidth, pipelineHeight );
        deepMask->preprocess( *maskBuffer, arg_globals->ShapeMaskExtractionThreshold, arg_globals->ShapeMaskExtractionMinElementSizeInPixels );

        if ( arg_globals->ShapeMaskPunchOut != MaskPunchOut::None )
        {
            maskOrigin = new ColourFBuffer(pipelineWidth, pipelineHeight);
            maskOrigin->bufferCopyFrom(previousLayer);
        }
        else
        {
            delete maskBuffer;
            maskBuffer = nullptr;
        }
    }

    RenderGlobals renderGlobals;
    renderGlobals.m_time                                = arg_globals->Time;
    renderGlobals.m_scale                               = arg_globals->GlobalScale;
    renderGlobals.m_rotation                            = arg_globals->GlobalRotate;
    renderGlobals.m_center                              = Point2f( arg_globals->CenterX, arg_globals->CenterY );

    RenderOutput renderOut;
    renderOut.m_time                                    = arg_globals->Time;
    renderOut.m_globalScale                             = arg_globals->GlobalScale;
    renderOut.m_flareSizeEdge                           = arg_images->LongestEdge;
    renderOut.m_hueShift                                = arg_globals->GlobalHueShift;
    renderOut.m_saturationShift                         = arg_globals->GlobalSaturationShift;
    renderOut.m_valueShift                              = arg_globals->GlobalValueShift;
    renderOut.m_colourise                               = arg_globals->ColouriseColour.toColourF();

    RenderTweaks tweaks;
    tweaks.m_allElementOverload                         = arg_globals->AllElementOverload;
    tweaks.m_protectAlpha                               = arg_globals->ProtectAlpha;

    tweaks.masking.m_renderMaskElementsSeparately       = arg_globals->RenderMaskElementsSeparately;
    tweaks.masking.m_perElementRandomisationSeed        = arg_globals->PerElementRandomisationSeed;
    tweaks.masking.m_perElementRandomisationTime        = arg_globals->PerElementRandomisationTime;
    tweaks.masking.m_perElementRandomisationRotation    = arg_globals->PerElementRandomisationRotation;
    tweaks.masking.m_perElementRandomisationScaleMin    = arg_globals->PerElementRandomisationScaleMin;
    tweaks.masking.m_perElementRandomisationScaleMax    = arg_globals->PerElementRandomisationScaleMax;

    tweaks.displacement.m_smoothing                     = arg_globals->DisplacementSmoothing;
    tweaks.displacement.m_smoothingDistance             = arg_globals->DisplacementSmoothingDistance;
    tweaks.displacement.m_smoothingSampleGrid           = arg_globals->DisplacementSmoothingSampleGrid;

    tweaks.noise.m_boostVariance                        = arg_globals->BoostNoiseVariance;
    tweaks.noise.m_boostVarianceScale                   = arg_globals->BoostNoiseScale;
    tweaks.noise.m_useHadesNoise                        = arg_globals->UseHadesNoise;
    tweaks.noise.m_globalNoiseScale                     = arg_globals->GlobalNoiseScale;

    MaskProcessSettings maskSettings;
    maskSettings.m_blurSize                             = arg_globals->ShapeMaskBlurSize;
    maskSettings.m_distanceMultiplier                   = arg_globals->ShapeMaskDistanceMultiplier;
    maskSettings.m_distanceTighten                      = arg_globals->ShapeMaskTighten;

    maskSettings.m_randomSeed                           = arg_globals->ShapeMaskRandomSeed;

    switch ( arg_globals->ShapeMaskDistortionMode )
    {
        case MaskDistortion::None: maskSettings.m_distortionMode        = MaskProcessSettings::DistortionMode::None;    break;
        case MaskDistortion::Noise: maskSettings.m_distortionMode       = MaskProcessSettings::DistortionMode::Noise;   break;
        case MaskDistortion::Celluar: maskSettings.m_distortionMode     = MaskProcessSettings::DistortionMode::Celluar; break;
        case MaskDistortion::Lump: maskSettings.m_distortionMode        = MaskProcessSettings::DistortionMode::Lump;    break;
        default:
        {
            rgLogOutput( LogLevel::Error, "Unknown distortion mode [%i]", arg_globals->ShapeMaskDistortionMode );
            return false;
        }
    }

    maskSettings.m_distortPerturb                       = arg_globals->ShapeMaskDistortionPerturbAmount;
    maskSettings.m_distortOffset                        = arg_globals->ShapeMaskDistortionOffset;
    maskSettings.m_distortMult.x                        = arg_globals->ShapeMaskDistortionX;
    maskSettings.m_distortMult.y                        = arg_globals->ShapeMaskDistortionY;
    maskSettings.m_distortMult.z                        = arg_globals->ShapeMaskDistortionZ;
    maskSettings.m_distortIntensity                     = arg_globals->ShapeMaskDistortionI;
    maskSettings.m_distortAltZAxis                      = arg_globals->ShapeMaskAltZAxis;

    regenesis::render(
        arg_gfxFile,
        renderGlobals,
        previousLayer,
        tweaks,
        renderOut,
        maskSettings,
        deepMask );

    if ( deepMask != nullptr && arg_globals->ShapeMaskPunchOut != MaskPunchOut::None )
    {
        if ( maskBuffer != nullptr &&
             maskOrigin != nullptr )
        {
            const bool punchInner = ( arg_globals->ShapeMaskPunchOut == MaskPunchOut::Inner );

            const float threshold            = arg_globals->ShapeMaskExtractionThreshold;
            const float thresholdLerpRescale = punchInner ? ( 1.0f / ( 1.0f - threshold) ) : ( 1.0f / threshold );

            AUTO_PARALLELIZE_LOOP
            for ( int32_t y = 0; y < pipelineHeight; y++ )
            {
                for ( int32_t x = 0; x < pipelineWidth; x++ )
                {
                    const float maskValue = (*maskBuffer)(x, y).ComputeIntensity();

                    if ( punchInner )
                    {
                        if ( maskValue > threshold)
                        {
                            const float remappedValue = ( maskValue - threshold ) * thresholdLerpRescale;
                            (*renderOut.m_result)(x, y).LerpTo( (*maskOrigin)(x, y), remappedValue );
                        }
                    }
                    else
                    {
                        if ( maskValue < threshold)
                        {
                            const float remappedValue = 1.0f - ( maskValue * thresholdLerpRescale );
                            (*renderOut.m_result)(x, y).LerpTo( (*maskOrigin)(x, y), remappedValue );
                        }
                    }
                }
            }
        }
        else
        {
            rgLogOutput( LogLevel::Error, "Could not perform mask punch-out, missing buffers" );
        }
    }

    ColourI8Buffer outputImageI8( *renderOut.m_result );
    pixelBitsFromImage( outputImageI8, &arg_images->Output );

    out_meta->MaskElements = ( deepMask == nullptr ) ? 0U : (uint32_t)deepMask->getElements().size();
    out_meta->RenderTime = (float)blockTimer.secondsElapsed();

    delete deepMask;
    delete maskBuffer;
    delete maskOrigin;

    return true;
}


// ------------------------------------------------------------------------------------------------
bool RenderFile(
    const char*     arg_gfxFile,
    const Globals*  arg_globals,
    const Images*   arg_images,
    ResultMeta*     out_meta )
{
    GfxFile gfx;
    GfxFile::LoadResult lres = gfx.LoadFile( arg_gfxFile );

    if (lres != GfxFile::LoadResult::Ok)
    {
        rgLogOutput(LogLevel::Error, "Failed to load GFX [%s] (err %i)", arg_gfxFile, lres);
        return false;
    }

    return Render( gfx, arg_globals, arg_images, out_meta );
}

// ------------------------------------------------------------------------------------------------
bool RenderMemory(
    const uint8_t*  arg_gfxInMemory,
    const int       arg_gfxInMemorySize,
    const Globals*  arg_globals,
    const Images*   arg_images,
    ResultMeta*     out_meta )
{
    std::vector<uint8_t> dataBuffer;
    dataBuffer.resize( arg_gfxInMemorySize );
    memcpy( dataBuffer.data(), arg_gfxInMemory, arg_gfxInMemorySize );

    GfxFile gfx;
    GfxFile::LoadResult lres = gfx.LoadMemory( dataBuffer );

    if (lres != GfxFile::LoadResult::Ok)
    {
        rgLogOutput(LogLevel::Error, "Failed to load GFX from memory (err %i)", lres);
        return false;
    }

    return Render( gfx, arg_globals, arg_images, out_meta );
}


#ifdef WIN32


#ifdef __cplusplus  // use C-style linking
extern "C" {
#endif

    // link to Intel libraries
    extern int64_t __intel_cpu_feature_indicator;    // CPU feature bits
    extern int64_t __intel_cpu_feature_indicator_x;  // CPU feature bits
    void __intel_cpu_features_init();                // unfair dispatcher: checks CPU features for Intel CPU's only
    void __intel_cpu_features_init_x();              // fair dispatcher: checks CPU features without discriminating by CPU brand

#ifdef __cplusplus
}  // end of extern "C"
#endif

void intel_cpu_patch() {
    // force a re-evaluation of the CPU features without discriminating by CPU brand
    __intel_cpu_feature_indicator = 0;
    __intel_cpu_feature_indicator_x = 0;
    __intel_cpu_features_init_x();
    __intel_cpu_feature_indicator = __intel_cpu_feature_indicator_x;
}


// ------------------------------------------------------------------------------------------------
//
BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle,
                        IN DWORD     nReason,
                        IN LPVOID    Reserved )
{
    switch ( nReason )
    {
        case DLL_PROCESS_ATTACH:
            intel_cpu_patch();
            DisableThreadLibraryCalls( hDllHandle );
            {
                // do one-off setup tasks
                regenesis::init();
            }
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

#endif // WIN32