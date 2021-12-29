/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#pragma once

#include "rgBuffer2D.h"
#include "rgColour.h"
#include "rgIO.h"
#include "rgTweaks.h"


//----------------------------------------------------------------------------------------------------------------------
struct RenderOutput
{
    RenderOutput()
        : m_time( 0.0f )
        , m_globalScale( 1.0f )
        , m_flareSizeEdge( 512.0f )     // set to the largest edge of the target canvas, controls a few internal scaling calls
        , m_colourise( ColourF::s_white )
        , m_hueShift( 0 )
        , m_saturationShift( 0 )
        , m_valueShift( 0 )
        , m_result( nullptr )
    {
    }

    float           m_time;
    float           m_globalScale;
    float           m_flareSizeEdge;
    
    ColourF         m_colourise;
    
    float           m_hueShift;
    float           m_saturationShift;
    float           m_valueShift;

    ColourFBufferUP m_result;
};


//----------------------------------------------------------------------------------------------------------------------
struct RenderSamplePoint
{
    Point2f         m_original;     // the original pixel coordinates
    Point2f         m_displaced;
    float           m_angValue;
    float           m_distValue;
    uint32_t        m_sampleHash;

    static rginline void outputsFromPoint( const Point2f& inp, float& angle, float &distance )
    {
        angle = atan2f( inp.y, inp.x ) + c_two_pi;
        distance = sqrtf(
            ( inp.x * inp.x ) +
            ( inp.y * inp.y ) );
    }

    rginline void calculateDerived( const Point2f& displaced )
    {
        m_displaced = displaced;
        outputsFromPoint( m_displaced, m_angValue, m_distValue );
    }
};


//----------------------------------------------------------------------------------------------------------------------
struct RenderCoordinate
{
    float       m_screenCX;
    float       m_screenCY;

    float       m_renderCX;
    float       m_renderCY;

    float       m_angleAdd;
};


//----------------------------------------------------------------------------------------------------------------------
class FlareRenderer
{
public:

    FlareRenderer(
        const RenderTweaks& tweaks );

    ~FlareRenderer();

    void Prepare(
        const RenderOutput&         routput,
        const GfxFile::FlareBlob&   blob );

    void ComputeForCenter(
        const Point2f&              centerPt,
        RenderCoordinate&           coordinate ) const;

    Point2f Displace(
        const Point2f&              in,
        const RenderCoordinate&     coordinate ) const;

    void Render(
        const RenderSamplePoint&    samplePoint,
        const RenderCoordinate&     coordinate,
        const ColourFBuffer&        sourceImage,
        ColourF&                    sourcePixel ) const;

protected:

    bool Sample(
        const float     sampleAngValue,
        const float     sampleDistValue,
        ColourF&        outputColour,
        ColourF&        outputStreak ) const;


    struct LayerContext;
    std::unique_ptr<LayerContext>   context;

    const RenderTweaks&             m_tweaks;
};
