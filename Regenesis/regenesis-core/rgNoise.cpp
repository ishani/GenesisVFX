/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#include "rgCommon.h"
#include "rgNoise.h"
#include "rgIO.h"
#include "rgTweaks.h"

// Jordan Peck's awesome noise library 
#include "FastNoise.h"



// ------------------------------------------------------------------------------------------------
//
// straight port of the original Genesis noise library for equivalence testing
//
// ------------------------------------------------------------------------------------------------

namespace original_noise {


    rginline float noise3( const Point3f& pt3 );

    rginline float bias( float a, float b )
    {
        return (float)( pow( a, log( b ) / log( 0.5 ) ) );
    }

    rginline float gain( float a, float b )
    {
        float p = (float)( log( 1. - b ) / log( 0.5 ) );

        if ( a < .001 )
            return 0.0f;
        else if ( a > 0.999f )
            return 1.0f;
        if ( a < 0.5f )
            return (float)( pow( 2.0 * a, p ) / 2.0 );
        else
            return (float)( 1.0 - pow( 2.0 * ( 1.0f - a ), p ) / 2.0 );
    }

    rginline float turbulence( float *v, float freq )
    {
        float t;
        Point3f vec;

        for ( t = 0.0f; freq >= 1.0f; freq /= 2.0f ) {
            vec.x = freq * v[0];
            vec.y = freq * v[1];
            vec.z = freq * v[2];
            t += (float)( fabs( noise3( vec ) ) / freq );
        }
        return t;
    }

    /* noise functions over 1, 2, and 3 dimensions */

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

    static int p[B + B + 2];
    static float g3[B + B + 2][3];
    static float g2[B + B + 2][2];
    static float g1[B + B + 2];


#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
        t = vec[i] + N;\
        b0 = ((int)t) & BM;\
        b1 = (b0+1) & BM;\
        r0 = t - (int)t;\
        r1 = r0 - 1.0f;



    float noise3( const Point3f& pt3 )
    {
        const float* vec = &pt3.x;

        int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
        float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
        int i, j;

        setup( 0, bx0, bx1, rx0, rx1 );
        setup( 1, by0, by1, ry0, ry1 );
        setup( 2, bz0, bz1, rz0, rz1 );

        i = p[bx0];
        j = p[bx1];

        b00 = p[i + by0];
        b10 = p[j + by0];
        b01 = p[i + by1];
        b11 = p[j + by1];

        t = s_curve( rx0 );
        sy = s_curve( ry0 );
        sz = s_curve( rz0 );

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

        q = g3[b00 + bz0]; u = at3( rx0, ry0, rz0 );
        q = g3[b10 + bz0]; v = at3( rx1, ry0, rz0 );
        a = lerp( t, u, v );

        q = g3[b01 + bz0]; u = at3( rx0, ry1, rz0 );
        q = g3[b11 + bz0]; v = at3( rx1, ry1, rz0 );
        b = lerp( t, u, v );

        c = lerp( sy, a, b );

        q = g3[b00 + bz1]; u = at3( rx0, ry0, rz1 );
        q = g3[b10 + bz1]; v = at3( rx1, ry0, rz1 );
        a = lerp( t, u, v );

        q = g3[b01 + bz1]; u = at3( rx0, ry1, rz1 );
        q = g3[b11 + bz1]; v = at3( rx1, ry1, rz1 );
        b = lerp( t, u, v );

        d = lerp( sy, a, b );

        return lerp( sz, c, d );
    }

    static void normalize2( float v[2] )
    {
        float s;

        s = (float)sqrt( v[0] * v[0] + v[1] * v[1] );
        v[0] = v[0] / s;
        v[1] = v[1] / s;
    }

    static void normalize3( float v[3] )
    {
        float s;

        s = (float)sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
        v[0] = v[0] / s;
        v[1] = v[1] / s;
        v[2] = v[2] / s;
    }

    void init( void )
    {
        int i, j, k;

        for ( i = 0; i < B; i++ ) {
            p[i] = i;

            g1[i] = (float)( ( rand() % ( B + B ) ) - B ) / B;

            for ( j = 0; j < 2; j++ )
                g2[i][j] = (float)( ( rand() % ( B + B ) ) - B ) / B;
            normalize2( g2[i] );

            for ( j = 0; j < 3; j++ )
                g3[i][j] = (float)( ( rand() % ( B + B ) ) - B ) / B;
            normalize3( g3[i] );
        }

        while ( --i ) {
            k = p[i];
            p[i] = p[j = rand() % B];
            p[j] = k;
        }

        for ( i = 0; i < B + 2; i++ ) {
            p[B + i] = p[i];
            g1[B + i] = g1[i];
            for ( j = 0; j < 2; j++ )
                g2[B + i][j] = g2[i][j];
            for ( j = 0; j < 3; j++ )
                g3[B + i][j] = g3[i][j];
        }
    }


    float Turb( const Point3f& p, const int32_t levels )
    {
        float sum = 0.0f;
        float l, f = 1.0f;

        for ( l = (float)levels; l >= 1.0f; l -= 1.0f )
        {
            sum += noise3( p*f ) / f;
            f *= 2.0f;
        }

        if ( l > 0.0f )	sum += l * noise3( p*f ) / f;

        return sum;
    }

} // namespace original_noise


// ------------------------------------------------------------------------------------------------
namespace Inferno {

    void Distort(
        const gfxdata::Fractal& iconfig,
        const Setup& isetup,
        const RenderTweaks& tweaks,
        float &xnew,
        float &ynew,
        float xin,
        float yin )
    {
        // nothing to do?
        if ( !iconfig.m_useFracNoise && !iconfig.m_useSimpleNoise )
        {
            xnew = xin;
            ynew = yin;
            return;
        }

        const float Mult = isetup.m_renderSize;
        const float invMult = 1.0f / Mult;

        const float fSeed = (float)iconfig.m_seed * 1000.0f;

        Point3f p;
        const float RealX = ( xin - isetup.m_cenx ) * invMult;
        const float RealY = ( yin - isetup.m_ceny ) * invMult;

        xnew = ynew = 0.0f;

        // Standard Fractal Noise
        if ( iconfig.m_useSimpleNoise )
        {
            const float Tm = iconfig.m_simplePhaseSpeed * ( isetup.m_time + fSeed );

            // X Perturbation
            p.x = RealX * iconfig.m_simpleXScale * tweaks.noise.m_globalNoiseScale;
            p.y = RealY * iconfig.m_simpleYScale * tweaks.noise.m_globalNoiseScale;
            p.z = Tm;
            xnew += original_noise::Turb( p, iconfig.m_simpleLevels ) * iconfig.m_simpleXSize;

            // Y Perturbation
            p.x += 23.0f;
            p.y += 6.0f;
            p.z += 1971.0f;
            ynew += original_noise::Turb( p, iconfig.m_simpleLevels ) * iconfig.m_simpleYSize;
        }

        // Fractal Noise
        if ( iconfig.m_useFracNoise )
        {
            const float Tm = iconfig.m_fracPhaseSpeed * ( isetup.m_time + fSeed );

            // X Perturbation
            p.x = RealX * iconfig.m_fracXScale * tweaks.noise.m_globalNoiseScale;
            p.y = RealY * iconfig.m_fracYScale * tweaks.noise.m_globalNoiseScale;
            p.z = Tm;
            xnew += ( original_noise::turbulence( &p.x, iconfig.m_fracFreq ) - 0.137f ) * iconfig.m_fracXSize;

            // Y Perturbation
            p.x += 23.0f; 
            p.y += 6.0f; 
            p.z += 1971.0f;
            ynew += ( original_noise::turbulence( &p.x, iconfig.m_fracFreq ) - 0.137f ) * iconfig.m_fracYSize;
        }

        // Scale And Prepare Output
        xnew = xin + ( xnew * Mult );
        ynew = yin + ( ynew * Mult );
    }

} // namespace Inferno


// ------------------------------------------------------------------------------------------------
namespace Hades {

    static std::unique_ptr<FastNoise> s_fastNoise;

    FractalConfig::FractalConfig( const int32_t seed, const float frequency )
    {
        m_fn = std::make_unique < FastNoise >( seed );
        m_fn->SetFrequency( frequency );

        m_fn->SetCellularDistanceFunction( FastNoise::Euclidean );
        m_fn->SetCellularReturnType( FastNoise::Distance2Add );
    }

    FractalConfig::~FractalConfig()
    {
    }


    // use the FastNoise lib to spice things up
    void Distort(
        const gfxdata::Fractal& iconfig,
        const FractalConfig&    fconfig,
        const Inferno::Setup&   isetup,
        const RenderTweaks&     tweaks,
        float &xnew,
        float &ynew,
        float xin,
        float yin )
    {
        // nothing to do?
        if ( !iconfig.m_useFracNoise && !iconfig.m_useSimpleNoise )
        {
            xnew = xin;
            ynew = yin;
            return;
        }

        const float Mult = isetup.m_renderSize;
        const float invMult = 1.0f / Mult;

        const float fSeed = (float)iconfig.m_seed * 1000.0f;

        Point3f p;
        const float RealX = ( xin - isetup.m_cenx ) * invMult;
        const float RealY = ( yin - isetup.m_ceny ) * invMult;


        xnew = ynew = 0.0f;

        if ( iconfig.m_useSimpleNoise )
        {
            const float Tm = iconfig.m_simplePhaseSpeed * ( isetup.m_time + fSeed );

            // X Perturbation
            p.x = RealX * iconfig.m_simpleXScale * 0.25f * tweaks.noise.m_globalNoiseScale;
            p.y = RealY * iconfig.m_simpleYScale * 0.25f * tweaks.noise.m_globalNoiseScale;
            p.z = Tm;
            xnew += ( -0.5f + fconfig.m_fn->GetCellular( p.x, p.y, p.z ) * 0.4f ) * iconfig.m_simpleXSize;

            // Y Perturbation
            p.x += 23.0f;
            p.y += 6.0f;
            p.z += 1971.0f;
            ynew += ( -0.5f + fconfig.m_fn->GetCellular( p.x, p.y, p.z ) * 0.4f ) * iconfig.m_simpleYSize;
        }

        if ( iconfig.m_useFracNoise )
        {
            const float Tm = iconfig.m_fracPhaseSpeed * ( isetup.m_time + fSeed );

            // X Perturbation
            p.x = RealX * iconfig.m_fracXScale * 2.0f * tweaks.noise.m_globalNoiseScale;
            p.y = RealY * iconfig.m_fracYScale * 2.0f * tweaks.noise.m_globalNoiseScale;
            p.z = Tm;
            xnew += ( fconfig.m_fn->GetCubicFractal( p.x, p.y, p.z ) ) * iconfig.m_fracXSize;

            // Y Perturbation
            p.x += 23.0f;
            p.y += 6.0f;
            p.z += 1971.0f;
            ynew += ( fconfig.m_fn->GetCubicFractal( p.x, p.y, p.z ) ) * iconfig.m_fracYSize;
        }


        xnew = xin + ( xnew * Mult );
        ynew = yin + ( ynew * Mult );
    }

} // namespace Hades