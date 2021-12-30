/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#pragma once

class ColourF;
class ColourI8;
class ColourHSV;


// ------------------------------------------------------------------------------------------------
// RGBA, float
//
class ColourF
{
public:
    float       r, g, b, a;

    static const ColourF s_transparent;
    static const ColourF s_black;
    static const ColourF s_white;
    static const ColourF s_red;


    rginline constexpr ColourF(
        const float _r,
        const float _g, 
        const float _b, 
        const float _a = 1.0f )
        : r( _r )
        , g( _g )
        , b( _b )
        , a( _a )
    {}

    rginline constexpr ColourF(
        const float _mono,
        const float _a = 1.0f )
        : r( _mono )
        , g( _mono )
        , b( _mono )
        , a( _a )
    {}

    rginline ColourF() {}

    explicit constexpr ColourF( const ColourI8&   in_i8 );
    explicit ColourF( const ColourHSV&  in_hsv );

    rginline void setChannel(const size_t channel, const float value)
    {
        dbg_ensure(channel <= 3);

        switch (channel)
        {
            case 0: r = value; break;
            case 1: g = value; break;
            case 2: b = value; break;
            case 3: a = value; break;
            default:
                break;
        }
    }

    rginline float operator [] (const size_t channel) const
    {
        dbg_ensure(channel <= 3);

        switch (channel)
        {
            case 0: return r;
            case 1: return g;
            case 2: return b;
            case 3: return a;
            default:
                return 0.0f;
        }
    }

    rginline constexpr void ClearA1()
    {
        r = g = b = 0.0f;
        a = 1.0f;
    }

    rginline constexpr void ClearA0()
    {
        r = g = b = a = 0.0f;
    }

    rginline constexpr void ClampHigh()
    {
        r = std::min( 1.0f, r );
        g = std::min( 1.0f, g );
        b = std::min( 1.0f, b );
        a = std::min( 1.0f, a );
    }

    rginline constexpr void ClampLow()
    {
        r = std::max( 0.0f, r );
        g = std::max( 0.0f, g );
        b = std::max( 0.0f, b );
        a = std::max( 0.0f, a );
    }

    rginline constexpr void Saturate()
    {
        r = saturate( r );
        g = saturate( g );
        b = saturate( b );
        a = saturate( a );
    }

    rginline constexpr float ColourAvg() const
    {
        return 0.333333333333f * (r + g + b);
    }

    rginline constexpr void ObjectBlend( ColourF& rhs )
    {
        r = r * ( 1 - rhs.a ) + rhs.r * rhs.a;
        g = g * ( 1 - rhs.a ) + rhs.g * rhs.a;
        b = b * ( 1 - rhs.a ) + rhs.b * rhs.a;
        a = 1.0f - ( 1.0f - a ) * ( 1.0f - rhs.a );
    }

    rginline constexpr void NegativeBlend( ColourF& rhs )
    {
        r = ( 1.0f - r ) * rhs.r + r * ( 1.0f - rhs.r );
        g = ( 1.0f - g ) * rhs.g + g * ( 1.0f - rhs.g );
        b = ( 1.0f - b ) * rhs.b + b * ( 1.0f - rhs.b );
        a = ( 1.0f - a ) * rhs.a + a * ( 1.0f - rhs.a );
    }

    rginline constexpr void LerpTo( const ColourF& to, const float t )
    {
        r = rgLerp(r, to.r, t);
        g = rgLerp(g, to.g, t);
        b = rgLerp(b, to.b, t);
        a = rgLerp(a, to.a, t);
    }

    rginline bool operator==( const ColourF& rhs )
    {
        return floatAlmostEqualRelative( r, rhs.r ) &&
               floatAlmostEqualRelative( g, rhs.g ) &&
               floatAlmostEqualRelative( b, rhs.b ) &&
               floatAlmostEqualRelative( a, rhs.a );
    }

    rginline constexpr void operator -= ( const ColourF& rhs )
    {
        r -= rhs.r;
        g -= rhs.g;
        b -= rhs.b;
        a -= rhs.a;
    }

    rginline constexpr void operator += ( const ColourF& rhs )
    {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
        a += rhs.a;
    }

    rginline constexpr void operator *= ( const ColourF& rhs )
    {
        r *= rhs.r;
        g *= rhs.g;
        b *= rhs.b;
        a *= rhs.a;
    }

    rginline constexpr void operator /= ( const float rhs )
    {
        r /= rhs;
        g /= rhs;
        b /= rhs;
        a /= rhs;
    }

    rginline constexpr ColourF operator* ( const float rhs ) const
    {
        return ColourF(
            r * rhs,
            g * rhs,
            b * rhs,
            a * rhs );
    }

    rginline constexpr ColourF operator* ( const ColourF& rhs ) const
    {
        return ColourF(
            r * rhs.r,
            g * rhs.g,
            b * rhs.b,
            a * rhs.a );
    }

    rginline constexpr ColourF operator+ ( const ColourF& rhs ) const
    {
        return ColourF(
            r + rhs.r,
            g + rhs.g,
            b + rhs.b,
            a + rhs.a );
    }

    friend rginline ColourF operator*( const float lhs, const ColourF& rhs )
    {
        return ColourF( rhs.r * lhs, rhs.g * lhs, rhs.b * lhs, rhs.a * lhs );
    }

    rginline constexpr float ComputeIntensity() const
    {
        const float lum_blend = ( r * 0.2989f ) + ( g * 0.5870f ) + ( b * 0.1140f );

        return saturate( lum_blend );
    }

    rginline static ColourF gradientViridis( const float t )
    {
        constexpr static ColourF c0 {  0.2777273272234177f,  0.005407344544966578f,  0.3340998053353061f  };
        constexpr static ColourF c1 {  0.1050930431085774f,  1.404613529898575f,     1.384590162594685f   };
        constexpr static ColourF c2 { -0.3308618287255563f,  0.214847559468213f,     0.09509516302823659f };
        constexpr static ColourF c3 { -4.634230498983486f,  -5.799100973351585f,   -19.33244095627987f    };
        constexpr static ColourF c4 {  6.228269936347081f,  14.17993336680509f,     56.69055260068105f    };
        constexpr static ColourF c5 {  4.776384997670288f, -13.74514537774601f,    -65.35303263337234f    };
        constexpr static ColourF c6 { -5.435455855934631f,   4.645852612178535f,    26.3124352495832f     };

        return (c0 + t * (c1 + t * (c2 + t * (c3 + t * (c4 + t * (c5 + t * c6))))));
    }
    rginline static ColourF gradientPlasma( const float t )
    {
        constexpr static ColourF c0 {   0.05873234392399702f, 0.02333670892565664f, 0.5433401826748754f  };
        constexpr static ColourF c1 {   2.176514634195958f,   0.2383834171260182f,  0.7539604599784036f  };
        constexpr static ColourF c2 {  -2.689460476458034f,  -7.455851135738909f,   3.110799939717086f   };
        constexpr static ColourF c3 {   6.130348345893603f,  42.3461881477227f,   -28.51885465332158f    };
        constexpr static ColourF c4 { -11.10743619062271f,  -82.66631109428045f,   60.13984767418263f    };
        constexpr static ColourF c5 {  10.02306557647065f,   71.41361770095349f,  -54.07218655560067f    };
        constexpr static ColourF c6 {  -3.658713842777788f, -22.93153465461149f,   18.19190778539828f    };

        return (c0 + t * (c1 + t * (c2 + t * (c3 + t * (c4 + t * (c5 + t * c6))))));
    }
};

// ------------------------------------------------------------------------------------------------
// RGBA, 8-bit uint
//
class ColourI8
{
public:
    uint8_t     r, g, b, a;

    static const ColourI8 s_transparent;
    static const ColourI8 s_black;
    static const ColourI8 s_white;
    static const ColourI8 s_red;


    rginline constexpr ColourI8(
        const uint8_t _r,
        const uint8_t _g,
        const uint8_t _b,
        const uint8_t _a = 255 )
        : r( _r )
        , g( _g )
        , b( _b )
        , a( _a )
    {}

    rginline constexpr ColourI8(
        const uint8_t _mono,
        const uint8_t _a = 255 )
        : r( _mono )
        , g( _mono )
        , b( _mono )
        , a( _a )
    {}


    ColourI8() {}

    explicit ColourI8( const ColourF&   in_f );
    explicit ColourI8( const ColourHSV& in_hsv );

    rginline constexpr void Set(
        const uint8_t _r,
        const uint8_t _g,
        const uint8_t _b,
        const uint8_t _a )
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    rginline constexpr void ClearA1()
    {
        r = g = b = 0;
        a = 255;
    }

    rginline constexpr void ClearA0()
    {
        r = g = b = a = 0;
    }

    rginline constexpr uint8_t ComputeIntensity() const
    {
        // do compute through float conversion
        ColourF to_float( *this );
        return (uint8_t)( to_float.ComputeIntensity() * 255.0f );
    }
};

// ------------------------------------------------------------------------------------------------
constexpr ColourF::ColourF( const ColourI8&   in_i8 )
    : r( (float)in_i8.r / 255.0f )
    , g( (float)in_i8.g / 255.0f )
    , b( (float)in_i8.b / 255.0f )
    , a( (float)in_i8.a / 255.0f )
{
}

// ------------------------------------------------------------------------------------------------
// Hue-Sat-Value-Alpha, float
//
class ColourHSV
{
public:
    float       h, s, v, a;

    ColourHSV() {}

    explicit ColourHSV( const ColourF&  in_f );
    explicit ColourHSV( const ColourI8& in_i8 );
};



// ------------------------------------------------------------------------------------------------
// http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
//
// inputs and outputs are in 0..1 range
//
rginline void RGB_to_HSV(
    float in_r, 
    float in_g,
    float in_b,
    float &out_h,
    float &out_s,
    float &out_v )
{
    const float flt_avoid_dbz = 1e-20f;

    float K = 0.f;

    if ( in_g < in_b )
    {
        std::swap( in_g, in_b );
        K = -1.0f;
    }

    float min_gb = in_b;
    if ( in_r < in_g )
    {
        std::swap( in_r, in_g );
        K = -2.0f / 6.0f - K;

        min_gb = std::min( in_g, in_b );
    }

    const float chroma = in_r - min_gb;

    out_h = fabs( K + ( in_g - in_b ) / ( 6.0f * chroma + flt_avoid_dbz ) );
    out_s = chroma / ( in_r + flt_avoid_dbz );
    out_v = in_r;
}

// ------------------------------------------------------------------------------------------------
rginline void HSV_to_RGB(
    const float in_h, 
    const float in_s,
    const float in_v,
    float &out_r, 
    float &out_g,
    float &out_b )
{
    float r, g, b;

    if ( in_s <= 0.0f ) 
    {
        out_r = in_v;
        out_g = in_v;
        out_b = in_v;
        return;
    }

    float i = floor( in_h * 6.0f );
    float f = in_h * 6.0f - i;
    float p = in_v * ( 1.0f - in_s );
    float q = in_v * ( 1.0f - f * in_s );
    float t = in_v * ( 1.0f - ( 1.0f - f ) * in_s );

    switch ( (int32_t)i % 6 ) 
    {
        case 0: r = in_v, g = t, b = p; break;
        case 1: r = q, g = in_v, b = p; break;
        case 2: r = p, g = in_v, b = t; break;
        case 3: r = p, g = q, b = in_v; break;
        case 4: r = t, g = p, b = in_v; break;
        case 5: r = in_v, g = p, b = q; break;
    }

    out_r = r;
    out_g = g;
    out_b = b;
}








