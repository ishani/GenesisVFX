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

// use a Sobel operator when building SGF, as opposed to a simple dx/dy calculation
#define MASK_USE_SOBEL_FILTER   1

// ---------------------------------------------------------------------------------------------
//
struct MaskProcessSettings
{
    enum class DistortionMode
    {
        None,
        Noise,
        Celluar,
        Lump,
    };

    MaskProcessSettings()
        : m_randomSeed( 12345 )
        , m_isolateElement( -1 )
        , m_blurSize( 40.0f )
        , m_distanceMultiplier( 1.0f )
        , m_distanceTighten( 1.0f )
        , m_distortMult( 0.0f, 0.0f, 0.0f )
        , m_distortOffset( 0.5f )
        , m_distortPerturb( 0.0f )
        , m_distortionMode( DistortionMode::None )
        , m_distortAltZAxis( false )
    {}

    uint32_t        m_randomSeed;
    int32_t         m_isolateElement;

    float           m_blurSize;
    float           m_distanceMultiplier;
    float           m_distanceTighten;

    Point3f         m_distortMult;
    float           m_distortOffset;
    float           m_distortPerturb;
    float           m_distortIntensity;
    DistortionMode  m_distortionMode;
    bool            m_distortAltZAxis;
};



// ---------------------------------------------------------------------------------------------
//
struct MaskElement
{
    rginline MaskElement(
        const uint32_t numPixels,
        const uint32_t numPasses,
        const Point2d& centroid )
        : m_numPixels( numPixels )
        , m_numPasses( numPasses )
        , m_centroid( centroid )
    {
    }

    uint32_t    m_numPixels;
    uint32_t    m_numPasses;
    Point2d     m_centroid;
};

using MaskElements = std::vector<MaskElement>;



// ---------------------------------------------------------------------------------------------
//

class DeepMask
{
public:

    DeepMask() = delete;
    DeepMask( const DeepMask& ) = delete;

    // set all buffers up, matching the input image size
    DeepMask( 
            const int32_t _width, 
            const int32_t _height )
        : m_valid( false )
        , m_maskedPixels( 0 )
        , m_maskedCentroid( 0, 0 )
        , m_bitMask(        _width, _height )
        , m_elementBuffer(  _width, _height )
        , m_udf(            _width, _height )
        , m_sgf(            _width, _height )
    {
        m_elementData.reserve( 64 );
    }
    
    
    void preprocess( 
        const ColourFBuffer&        imageInput, 
        const float                 threshold,
        const uint32_t              minimumElementSizeInPixels );

    void postprocess(
        const MaskProcessSettings&  settings );


    rginline bool isValid() const
    {
        return m_valid;
    }

    rginline int32_t getWidth() const
    {
        return m_bitMask.getWidth();
    }

    rginline int32_t getHeight() const
    {
        return m_bitMask.getHeight();
    }

    rginline uint32_t getMaskedPixels() const
    {
        return m_maskedPixels;
    }

    rginline Point2f getMaskedCentroid() const
    {
        return Point2f( (float)m_maskedCentroid.x, (float)m_maskedCentroid.y );
    }

    rginline const MaskElements& getElements() const
    {
        return m_elementData;
    }

    rginline const DistanceBuffer& getSGF() const
    {
        return m_sgf;
    }

    rginline const DistanceBuffer* getSGFIfProcessed() const
    {
        if ( !m_valid || m_elementData.empty() )
            return nullptr;

        return &m_sgf;
    }

protected:


    std::atomic_bool        m_valid;

    uint32_t                m_maskedPixels;
    Point2d                 m_maskedCentroid;

    BitBuffer               m_bitMask;

    MonoBufferU8            m_elementBuffer;
    MaskElements            m_elementData;

    MonoBufferF             m_udf;
    DistanceBuffer          m_sgf;
};