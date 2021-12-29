/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#pragma once

constexpr uint32_t c_hashPrime32_1 = 0x85ebca6b;
constexpr uint32_t c_hashPrime32_2 = 0x1b873593;


// ---------------------------------------------------------------------------------------------
rginline float constrainf(const float v, const float low, const float high)
{
    float t = fmodf(v - low, high - low);
    return t < 0 ? t + high : t + low;
}

// ---------------------------------------------------------------------------------------------
rginline double constraind(const double v, const double low, const double high)
{
    double t = fmod(v - low, high - low);
    return t < 0 ? t + high : t + low;
}

// ---------------------------------------------------------------------------------------------
rginline constexpr uint32_t hashMixUInt32( uint32_t u32v, uint32_t uMix = c_hashPrime32_1 )
{
    dbg_ensure( uMix != 0 );

    u32v = ( u32v ^ 61 ) ^ ( u32v >> 16 );
    u32v = u32v + ( u32v << 3 );
    u32v = u32v ^ ( u32v >> 4 );
    u32v = u32v * uMix;
    u32v = u32v ^ ( u32v >> 15 );

    return u32v;
}

// ---------------------------------------------------------------------------------------------
rginline constexpr uint32_t wangHash32( uint32_t seed )
{
    seed = ( seed ^ 61 ) ^ ( seed >> 16 );
    seed *= 9;
    seed = seed ^ ( seed >> 4 );
    seed *= 0x27d4eb2d;
    seed = seed ^ ( seed >> 15 );

    return seed;
}


//----------------------------------------------------------------------------------------------------------------------
// simple RNG 
class RNG
{
public:

    rginline RNG( const uint32_t newseed )
    {
        reseed( newseed );
    }

    rginline explicit RNG( const float floatBits )
    {
        typedef union {
            uint32_t i;
            float    f;
        } u;
        u u1;
        u1.f = floatBits;

        reseed( wangHash32( u1.i ) );
    }

    rginline void reseed( const uint32_t newseed )
    {
        m_seed[0] = newseed + !newseed;
        m_seed[1] = ( ( newseed << 16 ) | ( newseed >> 16 ) ) ^ newseed;
    }

    // random float in range [0..1]
    rginline float genFloat();

    // random float between rmin-rmax
    rginline float genFloat( const float rmin, const float rmax );

    rginline uint32_t genUInt32();

protected:

    uint32_t  m_seed[2];
};

//----------------------------------------------------------------------------------------------------------------------
rginline float RNG::genFloat()
{
    return ( genUInt32() & ( ( 1 << 23 ) - 1 ) ) * 0.00000011920928955078125f;
}

//----------------------------------------------------------------------------------------------------------------------
rginline float RNG::genFloat( float low, float high )
{
    if ( high < low )
        std::swap( high, low );

    return ( genFloat() * (high - low ) + low );
}

//----------------------------------------------------------------------------------------------------------------------
rginline uint32_t RNG::genUInt32()
{
    /*
    http://cliodhna.cop.uop.edu/~hetrick/na_faq.html

    [George Marsaglia]:
    Here is a simple version, one so simple and good I predict
    it will be the system generator for many future computers:
    x(n)=a*x(n-1)+carry mod 2^32
    With multiplier 'a' chosen from a large set of easily found
    integers, the period is a*2^31-1, around 2^60, and
    I have yet to find a test it will not pass!

    The 'carry' works like this, which shows how well this
    method will serve as a system RNG:
    Have seed x and c.  Form a*x+c in 64 bits.  All modern
    CPU's have instructions for this: sparc, intel, etc.
    Then the new x is the bottom 32 bits.
    the new carry is the top 32 bits.

    The period is the order of b=2^32 in the group of residues
    relatively prime to m=a*2^32-1.  One need only choose a's
    of some 30 bits for which m=a*2^32-1 is a safeprime:
    both m and (m-1)/2 are prime.  There are thousands of them.

    In general, for any choice of 'a', let m=a*2^32-1.  If both m
    and (m-1)/2 are prime then the period will be (m-1)/2.
    Even if 'a' is, say, your social security number, the
    period will most likely be on the order of 2^50 or more.
    (For mine, it is 2^54.8).

    For 32-bit generators, some possible values of 'a' are:
    1967773755 1517746329 1447497129 1655692410 1606218150
    2051013963 1075433238 1557985959 1781943330 1893513180
    */

    uint64_t temp = (uint64_t)1447497129 * m_seed[0] + m_seed[1];

    m_seed[0] = uint32_t( temp & ~0u );
    m_seed[1] = uint32_t( ( temp >> 32 ) & ~0u );

    return m_seed[0];
}


// ---------------------------------------------------------------------------------------------
// https://graphics.pixar.com/library/MultiJitteredSampling/
//
// Correlated Multi-Jittered Sampling
// Andrew Kensler

namespace cmjSampling {

    rginline unsigned permute( unsigned i, const unsigned l, const unsigned p )
    {
        unsigned w = l - 1;

        w |= w >> 1;
        w |= w >> 2;
        w |= w >> 4;
        w |= w >> 8;
        w |= w >> 16;

        do {
            i ^= p;
            i *= 0xe170893d;
            i ^= p >> 16;
            i ^= ( i & w ) >> 4;
            i ^= p >> 8;
            i *= 0x0929eb3f;
            i ^= p >> 23;
            i ^= ( i & w ) >> 1;
            i *= 1 | p >> 27;
            i *= 0x6935fa69;
            i ^= ( i & w ) >> 11;
            i *= 0x74dcb303;
            i ^= ( i & w ) >> 2;
            i *= 0x9e501cc3;
            i ^= ( i & w ) >> 2;
            i *= 0xc860a3df;
            i &= w;
            i ^= i >> 5;

        } while ( i >= l );

        return ( i + p ) % l;
    }

    rginline float randfloat( unsigned i, const unsigned p )
    {
        i ^= p;
        i ^= i >> 17;
        i ^= i >> 10;
        i *= 0xb36534e5;
        i ^= i >> 12;
        i ^= i >> 21;
        i *= 0x93fc4795;
        i ^= 0xdf6e307f;
        i ^= i >> 17;
        i *= 1 | p >> 18;
        return i * ( 1.0f / 4294967808.0f );
    }

    // s     = sample #
    // m x n = sample size
    // p     = pattern id
    rginline Point2f generate( const int sample, const int m, const int n, const int pattern )
    {
        const int32_t sx = permute( sample % m, m, pattern * 0xa511e9b3 );
        const int32_t sy = permute( sample / m, n, pattern * 0x63d83595 );

        const float jx = randfloat( sample, pattern * 0xa399d265 );
        const float jy = randfloat( sample, pattern * 0x711ad6a5 );

        const float s1 = ( sample % m + ( sy + jx ) / n ) / m;
        const float s2 = ( sample / m + ( sx + jy ) / m ) / n;

        return Point2f( s1, s2 );
    }

} // namespace cmjSampling