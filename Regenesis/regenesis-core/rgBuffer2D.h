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


// ---------------------------------------------------------------------------------------------
//
// A flexible 2D buffer for storing single or multi-channel images; plenty of options for accessing
// at 2D addresses and for converting between storage choices - ie. turning an RGBA colour buffer into
// a uint8 mono mask by sampling intensity
//
//
template< typename _Type >
class Buffer2D
{
using selftype = Buffer2D< _Type >;
public:

    Buffer2D() = delete;
    Buffer2D( const Buffer2D& rhs ) = delete;

    rginline Buffer2D(
        const int32_t width,
        const int32_t height )
        : m_width( width )
        , m_height( height )
    {
        m_buffer = (_Type*)calloc( sizeof( _Type ), m_width * m_height );
    }

    rginline Buffer2D( Buffer2D&& other )
        : m_width( other.getWidth() )
        , m_height( other.getHeight() )
    {
        m_buffer = other.m_buffer;
        other.m_buffer = nullptr;
    }

    template< typename _Other >
    rginline Buffer2D(
        const Buffer2D<_Other>& other )
        : m_width( other.getWidth() )
        , m_height( other.getHeight() )
    {
        m_buffer = (_Type*)calloc( sizeof( _Type ), m_width * m_height );
        convertFrom( other );
    }


    ~Buffer2D()
    {
        free( m_buffer );
    }

    rginline void bufferCopyFrom( const selftype& other )
    {
        ensure( other.getWidth() == m_width );
        ensure( other.getHeight() == m_height );

        memcpy( m_buffer, other.m_buffer, sizeof( _Type ) * m_width * m_height );
    }

    rginline void resampleFrom( const selftype& other )
    {
        float targetWidthF  = (float)other.getWidth();
        float targetHeightF = (float)other.getHeight();

        float widthFrac  = 1.0f / (float)m_width;
        float heightFrac = 1.0f / (float)m_height;

        for ( int32_t y = 0; y < m_height; y++ )
        {
            float fracY = (float)y * heightFrac;

            for ( int32_t x = 0; x < m_width; x++ )
            {
                float fracX = (float)x * widthFrac;

                operator()(x, y) = bilinearSample(other, fracX * targetWidthF, fracY * targetHeightF);
            }
        }
    }

    rginline int32_t getWidth() const
    {
        return m_width;
    }

    rginline int32_t getHeight() const
    {
        return m_height;
    }


    rginline void clear( const _Type& value )
    {
        for ( size_t i = 0; i < m_width * m_height; i++ )
        {
            m_buffer[i] = value;
        }
    }


    // direct (x,y) accessor does no error checking
    rginline const _Type& operator () ( const int32_t &x, const int32_t &y ) const
    {
        dbg_ensure( x >= 0 && x < m_width );
        dbg_ensure( y >= 0 && y < m_height );

        return m_buffer[( y * m_width ) + x];
    }

    // direct (x,y) accessor does no error checking
    rginline _Type& operator () ( const int32_t &x, const int32_t &y )
    {
        dbg_ensure( x >= 0 && x < m_width );
        dbg_ensure( y >= 0 && y < m_height );

        return m_buffer[( y * m_width ) + x];
    }

    // access the data buffer linearly with []
    rginline const _Type& operator [] ( const size_t offset ) const
    {
        dbg_ensure( offset < ( m_width * m_height ) );

        return m_buffer[offset];
    }

    // access the data buffer linearly with []
    rginline _Type& operator [] ( const size_t offset )
    {
        dbg_ensure( offset < ( m_width * m_height ) );

        return m_buffer[offset];
    }


    // set a pixel, safely ignoring out-of-bounds XY coordinates
    rginline void poke(
        const int32_t x,
        const int32_t y,
        const _Type& colour )
    {
        if ( x < 0          ||
             x >= m_width   ||
             y < 0          ||
             y >= m_height )
        {
            return;
        }

        m_buffer[( y * m_width ) + x] = colour;
    }

    // get a pointer back to a pixel, or null if XY is out-of bounds
    rginline _Type* peek(
        const int32_t x,
        const int32_t y )
    {
        if ( x < 0          ||
            x >= m_width    ||
            y < 0           ||
            y >= m_height )
        {
            return nullptr;
        }

        return &m_buffer[( y * m_width ) + x];
    }


    // conversion template that can move a buffer to a different storage type
    template< typename _Other >
    rginline void convertFrom( const Buffer2D<_Other>& other, const bool yFlip = false )
    {
        ensure( m_width == other.getWidth() );
        ensure( m_height == other.getHeight() );

        AUTO_PARALLELIZE_LOOP
        for ( int32_t y = 0; y < m_height; y++ )
        {
            const int32_t _y = yFlip ? ( m_height - 1 - y ) : y;

            for ( int32_t x = 0; x < m_width; x++ )
            {
                pixelTypeConversion( other( x, y ), operator()( x, _y ) );
            }
        }
    }

    // a convenience (ie. not fast) function to run a lambda on each value in the buffer
    // don't use for anything speed critical!
    rginline void forEach( const std::function<void( const int32_t index, _Type& value )>& callback )
    {
        for ( int32_t i = 0; i < m_width* m_height; i++ )
        {
            callback( i, operator[]( i ) );
        }
    }

    // direct buffer access
    rginline const _Type* data() const
    {
        return m_buffer;
    }


private:

    _Type      *m_buffer;

    int32_t     m_width;
    int32_t     m_height;
};


// ---------------------------------------------------------------------------------------------

rginline void pixelTypeConversion( const float inV, float& outV )
{
    outV = inV;
}

rginline void pixelTypeConversion( const uint8_t inV, uint8_t& outV )
{
    outV = inV;
}

rginline void pixelTypeConversion( const ColourF& inV, ColourF& outV )
{
    outV = inV;
}

rginline void pixelTypeConversion( const ColourI8& inV, ColourI8& outV )
{
    outV = inV;
}


rginline void pixelTypeConversion( const float inV, uint8_t& outV )
{
    outV = (uint8_t)clamp( inV * 255.0f, 0.0f, 255.0f );
}

rginline void pixelTypeConversion( const ColourF& inV, uint8_t& outV )
{
    outV = (uint8_t)clamp( inV.ComputeIntensity() * 255.0f, 0.0f, 255.0f );
}

rginline void pixelTypeConversion( const ColourI8& inV, uint8_t& outV )
{
    outV = (uint8_t)inV.ComputeIntensity();
}


rginline void pixelTypeConversion( const uint8_t inV, float& outV )
{
    outV = ( (float)inV ) / 255.0f;
}

rginline void pixelTypeConversion( const ColourF inV, float& outV )
{
    outV = inV.ComputeIntensity();
}

rginline void pixelTypeConversion( const ColourI8 inV, float& outV )
{
    outV = ( (float)inV.ComputeIntensity() ) / 255.0f;
}


rginline void pixelTypeConversion( const uint8_t inV, ColourF& outV )
{
    const float cI = ( (float)inV ) / 255.0f;
    outV = ColourF( cI, cI, cI );
}

rginline void pixelTypeConversion( const float inV, ColourF& outV )
{
    outV = ColourF( inV, inV, inV );
}

rginline void pixelTypeConversion( const float inV, ColourI8& outV )
{
    const uint8_t cV = (uint8_t)clamp( inV * 255.0f, 0.0f, 255.0f );
    outV = ColourI8( cV );
}


rginline void pixelTypeConversion( const ColourI8 inV, ColourF& outV )
{
    outV = ColourF( inV );
}

rginline void pixelTypeConversion( const ColourF inV, ColourI8& outV )
{
    outV = ColourI8( inV );
}



// ---------------------------------------------------------------------------------------------

// the known, usable set of buffer configurations with convertFrom implementations - ie. those that
// have a natural conversion that doesn't require configuration
using MonoBufferF      = Buffer2D<float>;
using MonoBufferU8     = Buffer2D<uint8_t>;
using MonoBufferU16    = Buffer2D<uint16_t>;
using ColourFBuffer    = Buffer2D<ColourF>;
using ColourI8Buffer   = Buffer2D<ColourI8>;

// others that are useful but not necessarily defined to cleanly move between others
using BitBuffer        = Buffer2D<bool>;
using DistanceBuffer   = Buffer2D<Point2f>;

// unique-ptr buffer types
using MonoBufferFUP    = std::unique_ptr< MonoBufferF >;
using MonoBufferI8UP   = std::unique_ptr< MonoBufferU8 >;
using ColourFBufferUP  = std::unique_ptr< ColourFBuffer >;
using ColourI8BufferUP = std::unique_ptr< ColourI8Buffer >;



// ---------------------------------------------------------------------------------------------
//
// load / save via stb
//

namespace _internal
{
    bool image_file_encode_png( const std::string& filename, const std::vector<uint8_t>& in, int32_t w, int32_t h );
    bool image_file_encode_jpeg( const std::string& filename, const std::vector<uint8_t>& in, int32_t w, int32_t h );

    bool image_file_decode( std::vector<uint8_t>& out, int32_t& w, int32_t& h, const std::string& filename );

    const char* image_error_text();
}

enum class OutputFormat
{
    AsJPEG = 1,
    AsPNG = 2
};


// ------------------------------------------------------------------------------------------------
// write out a buffer to a LDR format (JPG or PNG, based on filename); as long it has a [] accessor whose value can conver to a ColourI8
//
template< typename _BufferStorage >
inline bool writeOutBufferAsImage( const Buffer2D<_BufferStorage>& buf, const char* filename, const OutputFormat format, bool ignoreAlpha = false )
{
    const int32_t width = buf.getWidth();
    const int32_t height = buf.getHeight();

    std::vector<uint8_t> image;
    image.resize( width * height * 4 );

    rgLogOutput( LogLevel::Verbose, "[%s] writing [%i x %i] image to '%s'", __FUNCTION__, width, height, filename );

    AUTO_PARALLELIZE_LOOP
    for ( int32_t i = 0; i < width * height; i++ )
    {
        ColourI8 colI8;
        pixelTypeConversion( buf[i], colI8 );

        uint8_t* colByte = &image[i * 4];

        colByte[0] = colI8.r;
        colByte[1] = colI8.g;
        colByte[2] = colI8.b;
        colByte[3] = ignoreAlpha ? 255 : colI8.a;
    }

    bool ok = false;

    if ( format == OutputFormat::AsJPEG )
    {
        ok = _internal::image_file_encode_jpeg( filename, image, width, height );
    }
    else
    {
        ok = _internal::image_file_encode_png( filename, image, width, height );
    }

    if ( !ok )
    {
        rgLogOutput( LogLevel::Error, "[%s] encode failed - %s", __FUNCTION__, _internal::image_error_text() );
        return false;
    }

    return true;
}


// ------------------------------------------------------------------------------------------------
// load a low dynamic range image - from PNG, JPG, BMP etc
//
template< typename _BufferStorage >
inline std::unique_ptr< Buffer2D<_BufferStorage> > bufferFromImage( const char* filename )
{
    std::vector<uint8_t> image;
    int32_t width, height;

    rgLogOutput( LogLevel::Verbose, "[%s] decoding image '%s'", __FUNCTION__, filename );

    //decode
    bool ok = _internal::image_file_decode( image, width, height, filename );
    if ( !ok )
    {
        rgLogOutput( LogLevel::Error, "[%s] decode failed - %s", __FUNCTION__, _internal::image_error_text() );
        return nullptr;
    }

    // create a new buffer object and feed it from the raw pixels we loaded
    auto instance = std::make_unique< Buffer2D<_BufferStorage> >( width, height );

    AUTO_PARALLELIZE_LOOP
    for ( int32_t i = 0; i < width * height; i++ )
    {
        // create an I8 colour from the pixel stream
        uint8_t* colByte = &image[i * 4];
        ColourI8 colI8( colByte[0], colByte[1], colByte[2], colByte[3] );

        // push directly into the buffer
        ( *instance )[i] = _BufferStorage( colI8 );
    }

    return std::move( instance );
}


// ------------------------------------------------------------------------------------------------
// sample a colour buffer at a fractional XY position, with smooth blending
//
inline ColourF bilinearSample(
    const ColourFBuffer& image,
    const float x,
    const float y )
{
    // reflect if position is negative
    const float absX = /*fabsf*/( x );
    const float absY = /*fabsf*/( y );

    // find the candidate X/Y cells at either edge of the float positions; clamp in the progress
    const int32_t floorX = clamp( (int32_t)( floorf( absX ) ), 0, image.getWidth() - 1 );
    const int32_t ceilX  = clamp( (int32_t)( ceilf( absX ) ), 0, image.getWidth() - 1 );
    const int32_t floorY = clamp( (int32_t)( floorf( absY ) ), 0, image.getHeight() - 1 );
    const int32_t ceilY  = clamp( (int32_t)( ceilf( absY ) ), 0, image.getHeight() - 1 );

    // compute relative weights for the ceil/floor values on each axis
    const float CX_weight = fractf( x );
    const float FX_weight = 1.0f - CX_weight;
    const float CY_weight = fractf( y );
    const float FY_weight = 1.0f - CY_weight;

    // bilinear blend
    const ColourF sample_FX_FY = ( image( floorX, floorY ) * FY_weight ) * FX_weight;
    const ColourF sample_CX_FY = ( image(  ceilX, floorY ) * FY_weight ) * CX_weight;
    const ColourF sample_FX_CY = ( image( floorX, ceilY )  * CY_weight ) * FX_weight;
    const ColourF sample_CX_CY = ( image(  ceilX, ceilY )  * CY_weight ) * CX_weight;

    return sample_FX_FY +
           sample_CX_FY +
           sample_FX_CY +
           sample_CX_CY;
}

// ------------------------------------------------------------------------------------------------
// sample a colour buffer at a fractional XY position, with smooth blending - but per channel
//
inline float bilinearSampleChannel(
    const ColourFBuffer& image,
    const float x,
    const float y,
    const size_t channelIndex )
{
    // reflect if position is negative
    const float absX = /*fabsf*/( x );
    const float absY = /*fabsf*/( y );

    // find the candidate X/Y cells at either edge of the float positions; clamp in the progress
    const int32_t floorX = clamp( (int32_t)( floorf( absX ) ), 0, image.getWidth() - 1 );
    const int32_t ceilX  = clamp( (int32_t)( ceilf( absX ) ), 0, image.getWidth() - 1 );
    const int32_t floorY = clamp( (int32_t)( floorf( absY ) ), 0, image.getHeight() - 1 );
    const int32_t ceilY  = clamp( (int32_t)( ceilf( absY ) ), 0, image.getHeight() - 1 );

    // compute relative weights for the ceil/floor values on each axis
    const float CX_weight = fractf( x );
    const float FX_weight = 1.0f - CX_weight;
    const float CY_weight = fractf( y );
    const float FY_weight = 1.0f - CY_weight;

    // bilinear blend
    const float sample_FX_FY = ( image( floorX, floorY )[channelIndex] * FY_weight ) * FX_weight;
    const float sample_CX_FY = ( image(  ceilX, floorY )[channelIndex] * FY_weight ) * CX_weight;
    const float sample_FX_CY = ( image( floorX, ceilY )[channelIndex]  * CY_weight ) * FX_weight;
    const float sample_CX_CY = ( image(  ceilX, ceilY )[channelIndex]  * CY_weight ) * CX_weight;

    return sample_FX_FY +
           sample_CX_FY +
           sample_FX_CY +
           sample_CX_CY;
}
