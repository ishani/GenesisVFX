/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#pragma once

#include "rgColour.h"


// ------------------------------------------------------------------------------------------------
// distilled from the various Load(FILE*) functions (flare.h and fractal.h), these structures are what get burnt out to the .gfx files 
//
namespace gfxdata
{

#pragma pack(push, 1)

    constexpr size_t FLARERESOLUTION = 1024;

    enum LightMode  // FlareHeader -> m_light
    {
        /*  0 */ LM_Light,
        /*  1 */ LM_Object,
        /*  2 */ LM_Tint,
        /*  3 */ LM_Add,
        /*  4 */ LM_Subtract,
        /*  5 */ LM_Negative,
        /*  6 */ LM_Blur_Intensity,
        /*  7 */ LM_Blur_RGB,
        /*  8 */ LM_Angular_Displace_Intensity,
        /*  9 */ LM_Angular_Displace_RGB,
        /* 10 */ LM_Radial_Displace_Intensity,
        /* 11 */ LM_Radial_Displace_RGB,
        /* 12 */ LM_Image_Warp_Intensity,
        /* 13 */ LM_Image_Warp_RGB
    };


    // top of the gfx file
    struct Header
    {
        char        m_version[10];          // GFX100 or GFX101, only first 6 bytes are used
        int32_t     m_antiAliasing;
        int32_t     m_single;
        int32_t     m_CSE;                  // !Centroid
        int32_t     m_maskInside;
        float       m_cameraDepth;
        float       m_externalSmoothness;
        float       m_internalSmoothness;
        int32_t     m_colourise;
        float       m_colouriseSmoothness;

        char        m_name[256];            // name of effect

        int32_t     m_numFlares;            // number of FlareHeader + data to load
        int32_t     m_zSort;
    };

    // per flare element data
    struct FlareHeader
    {
        char        m_name[256];
        int32_t      _active;               // overwritten in Flare::Save by the second one below
        int32_t     m_light;
        int32_t     m_reAlign;              // if enabled, this modifies flare angle dependent on where the flare is being rendered in relation to screen center
        int32_t     m_offScreenRender;
        int32_t     m_linked;
        int32_t     m_active;
        float       m_IR2;
        int32_t     m_polar;
    };

    // contents of the [Make] panel, this defines the underlying flare geometry
    struct FlareBuild
    {
        int32_t     m_buildColour;
        int32_t     m_buildShape;
        float       m_buildWidth;
        float       m_buildTransparency;
        int32_t     m_buildNoStreaks;
        int32_t     m_buildNoSides;
        float       m_buildFactor;
        float       m_buildAspect;
        int32_t     m_buildSeed;
        ColourF     m_buildCol;
    };

    // location, rotation, scale and such; any flare has at least 1, more if they are animated
    struct FlarePosAnim
    {
        int32_t     m_time;

        float       m_positionX, 
                    m_positionY;

        float       m_position;
        float       m_angle;
        float       m_flareSize;

        float       m_angleOffSet;
        float       m_colourise;
        float       m_intensity;
        float       m_occlusion;
        float       m_depth;
    };

    struct FlareShpAnim
    {
        int32_t     m_time;

        uint16_t    m_shape[FLARERESOLUTION];
        uint16_t    m_shape2[FLARERESOLUTION];
        ColourI8    m_streaks[FLARERESOLUTION];
        ColourI8    m_col[FLARERESOLUTION];
    };

    // the noise panel
    struct Fractal  // 'Inferno'
    {
        int32_t     m_seed;
        int32_t     m_useSimpleNoise;
        float       m_simpleXScale;         // 'size' in the UI
        float       m_simpleYScale;
        float       m_simpleXSize;          // 'deformation' in the UI
        float       m_simpleYSize;
        float       m_simplePhaseSpeed;
        int32_t     m_simpleLevels;
        int32_t     m_simpleSizeLock;
        int32_t     m_simpleDeformLock;

        int32_t     m_useFracNoise;
        float       m_fracXScale;           // as above
        float       m_fracYScale;
        float       m_fracXSize;            // as above
        float       m_fracYSize;
        float       m_fracPhaseSpeed;
        float       m_fracFreq;
        int32_t     m_fracSizeLock;
        int32_t     m_fracDeformLock;

        void defaults()
        {
            m_seed             = 0;
            m_useSimpleNoise   = 0;
            m_simpleXScale     = 5.0f;
            m_simpleYScale     = 5.0f;
            m_simpleXSize      = 1.0f;
            m_simpleYSize      = 1.0f;
            m_simplePhaseSpeed = 0.05f;
            m_simpleLevels     = 1;
            m_simpleSizeLock   = 1;
            m_simpleDeformLock = 1;

            m_useFracNoise     = 0;
            m_fracXScale       = 5.0f;
            m_fracYScale       = 5.0f;
            m_fracXSize        = 1.0f;
            m_fracYSize        = 1.0f;
            m_fracPhaseSpeed   = 0.05f;
            m_fracFreq         = 1.0f;
            m_fracSizeLock     = 1;
            m_fracDeformLock   = 1;
        }
    };

#pragma pack(pop)

} // namespace gfxdata

constexpr  float c_FlareResF        = (float)gfxdata::FLARERESOLUTION;
constexpr double c_FlareResD        = (double)gfxdata::FLARERESOLUTION;
constexpr  float c_FlareResRadial   = c_FlareResF / c_two_pi;               // aka AngleM

// ------------------------------------------------------------------------------------------------
// given a FlareShpAnim, unpack it from words + i8 colours to float formats ready for use during rendering
//
struct FlareRenderShape
{
    FlareRenderShape( const gfxdata::FlareShpAnim& shp )
        : m_time( shp.m_time )
    {
        for ( size_t i = 0; i < gfxdata::FLARERESOLUTION; i++ )
        {
            m_shape[i]   = UI16ToFloat( shp.m_shape[i] );
            m_shape2[i]  = UI16ToFloat( shp.m_shape2[i] );
            m_streaks[i] = ColourF( shp.m_streaks[i] );
            m_col[i]     = ColourF( shp.m_col[i] );
        }
    }

    using FlareFloats  = std::array< float,   gfxdata::FLARERESOLUTION >;
    using FlareColours = std::array< ColourF, gfxdata::FLARERESOLUTION >;


    //
    // boiled down from various lerp/lerp3/lerpwrap variants in flare.cpp into a generic template function that can sample either float or colour arrays
    //
    // take an input array (ie. m_streaks) and a float index (eg. 22.6), this lerps between (eg. 22 and 23, by 0.6)
    //
    // [wrap] : if index goes out of bounds, and this is true, we wrap around to the start of the array; otherwise takes single colour from start index
    //
    template< typename _Storage, size_t _Resolution >
    rginline _Storage lerp(
        const bool                                  wrap,
        const std::array< _Storage, _Resolution >&  inputArray,
        const float                                 fractionalIndex ) const
    {
        // figure out the two indices to lerp between
        int32_t fromIndex = (int32_t)floor( fractionalIndex );
        int32_t toIndex = fromIndex + 1;

        if ( toIndex >= _Resolution )
        {
            if ( wrap )
            {
                toIndex = 0;
            }
            else
            {
                return inputArray[fromIndex % _Resolution];
            }
        }

        const float fractionOfTo   = fractionalIndex - (float)fromIndex;
        const float fractionOfFrom = 1.0f - fractionOfTo;

        _Storage result;
        do_lerp( result, inputArray[fromIndex], inputArray[toIndex], fractionOfFrom, fractionOfTo );

        return result;
    }


    int32_t         m_time;

    FlareFloats     m_shape;
    FlareFloats     m_shape2;
    FlareColours    m_streaks;
    FlareColours    m_col;

protected:

    rginline static constexpr void do_lerp( ColourF& result, const ColourF& from, const ColourF& to, const float fractionOfFrom, const float fractionOfTo )
    {
        result.r = ( fractionOfTo * to.r ) + ( fractionOfFrom * from.r );
        result.g = ( fractionOfTo * to.g ) + ( fractionOfFrom * from.g );
        result.b = ( fractionOfTo * to.b ) + ( fractionOfFrom * from.b );
        result.a = ( fractionOfTo * to.a ) + ( fractionOfFrom * from.a );
    }

    rginline static constexpr void do_lerp( float& result, const float& from, const float& to, const float fractionOfFrom, const float fractionOfTo )
    {
        result = ( fractionOfTo * to ) + ( fractionOfFrom * from );
    }
};


// ------------------------------------------------------------------------------------------------
// container for loading GFX data off disk
//
class GfxFile
{
public:

    using PosAnimVector = std::vector<gfxdata::FlarePosAnim>;
    using ShapeAnimVector = std::vector<gfxdata::FlareShpAnim>;

    enum class LoadResult
    {
        Ok,
        Genesis110NotSupported,     // saved from VFX 2.0, cannot load (yet)
        InvalidHeader,
        FileNotFound
    };

    struct FlareBlob
    {
        gfxdata::FlareHeader    m_header;

        PosAnimVector           m_pos_keys;
        ShapeAnimVector         m_shape_keys;

        gfxdata::Fractal        m_inferno;
        gfxdata::FlareBuild     m_build;
    };


    LoadResult LoadFile( const char* input_file );
    LoadResult LoadMemory( const std::vector<uint8_t>& memory_stream );

    gfxdata::Header         header;
    std::vector<FlareBlob>  flares;
};
