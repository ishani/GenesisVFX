/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#include "rgCommon.h"
#include "rgColour.h"



#if 0
void fColour::FilterOver( float & or , float &og, float &ob, float &oa, fColour &C )
{
    or = C.r*( 1 - a ) + r * a; 
    og = C.g*( 1 - a ) + g * a;
    ob = C.b*( 1 - a ) + b * a; 
    oa = 1.0f - ( 1.0f - C.a )*( 1.0f - a );
}

void fColour::FilterOver( fColour &o, fColour &C )
{
    o.r = C.r*( 1 - a ) + r * a; 
    o.g = C.g*( 1 - a ) + g * a;
    o.b = C.b*( 1 - a ) + b * a; 
    o.a = 1.0f - ( 1.0f - C.a )*( 1.0f - a );
}

void fColour::FilterOver( float & or , float &og, float &ob, float &oa, float &Cr, float &Cg, float &Cb, float &Ca )
{
    or = Cr * ( 1 - a ) + r * a; 
    og = Cg * ( 1 - a ) + g * a;
    ob = Cb * ( 1 - a ) + b * a; 
    oa = 1.0f - ( 1.0f - Ca )*( 1.0f - a );
}

void fColour::FilterOver( float &Cr, float &Cg, float &Cb, float &Ca )
{
    Cr = Cr * ( 1 - a ) + r * a; 
    Cg = Cg * ( 1 - a ) + g * a;
    Cb = Cb * ( 1 - a ) + b * a; 
    Ca = 1.0f - ( 1.0f - Ca )*( 1.0f - a );
}
#endif


const ColourF ColourF::s_transparent( 0.0f, 0.0f, 0.0f, 0.0f );
const ColourF ColourF::s_black      ( 0.0f, 0.0f, 0.0f, 1.0f );
const ColourF ColourF::s_white      ( 1.0f, 1.0f, 1.0f, 1.0f );
const ColourF ColourF::s_red        ( 1.0f, 0.0f, 0.0f, 1.0f );

const ColourI8 ColourI8::s_transparent(   0,   0,   0,   0 );
const ColourI8 ColourI8::s_black      (   0,   0,   0, 255 );
const ColourI8 ColourI8::s_white      ( 255, 255, 255, 255 );
const ColourI8 ColourI8::s_red        ( 255,   0,   0, 255 );


// ------------------------------------------------------------------------------------------------
ColourF::ColourF( const ColourHSV& in_hsv )
{
    a = in_hsv.a;
    HSV_to_RGB( in_hsv.h, in_hsv.s, in_hsv.v, r, g, b );
}

// ------------------------------------------------------------------------------------------------
ColourI8::ColourI8( const ColourF& in_f )
{
    r = (uint8_t)( std::max( 0.0f, std::min( in_f.r, 1.0f ) ) * 255.0f );
    g = (uint8_t)( std::max( 0.0f, std::min( in_f.g, 1.0f ) ) * 255.0f );
    b = (uint8_t)( std::max( 0.0f, std::min( in_f.b, 1.0f ) ) * 255.0f );
    a = (uint8_t)( std::max( 0.0f, std::min( in_f.a, 1.0f ) ) * 255.0f );
}

// ------------------------------------------------------------------------------------------------
ColourI8::ColourI8( const ColourHSV& in_hsv ) : ColourI8( ColourF(in_hsv) )
{
}

// ------------------------------------------------------------------------------------------------
ColourHSV::ColourHSV( const ColourF& in_f )
{
    a = in_f.a;
    RGB_to_HSV( in_f.r, in_f.g, in_f.b, h, s, v );
}

// ------------------------------------------------------------------------------------------------
ColourHSV::ColourHSV( const ColourI8& in_i8 ) : ColourHSV( ColourF( in_i8 ) )
{
}
