/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#pragma once

//----------------------------------------------------------------------------------------------------------------------
// as this is a rewrite, we have a bunch of tricks and extensions available to augment
// the original Genesis code / results; these are set and passed in when creating FlareRenderer instances
// and passed down into subsystems that might want to have user-toggled improvements or tunings
//
struct RenderTweaks
{
    rginline RenderTweaks()
        : m_allElementOverload( -1 )
        , m_protectAlpha( true )
    {}

    // extra controls for masked rendering
    struct Masking
    {
        rginline Masking()
            : m_renderMaskElementsSeparately( false )
            , m_perElementRandomisationSeed( 12345U )
            , m_perElementRandomisationTime( 0.0f )
            , m_perElementRandomisationRotation( 0.0f )
            , m_perElementRandomisationScaleMin( 1.0f )
            , m_perElementRandomisationScaleMax( 1.0f )
        {}

        bool        m_renderMaskElementsSeparately;         // instead of producing a single SDF for the mask, render each separate component and then blend them;
                                                            // this can produce a much nicer, integrated effect, depending on the mask
        uint32_t    m_perElementRandomisationSeed;          // .. use this value as a RNG seed when applying Time/Rotation adjustments
        float       m_perElementRandomisationTime;          // .. apply a random Time offset for each layer, so you don't get the same result for each
        float       m_perElementRandomisationRotation;      // .. and similarly for their Rotation values
        float       m_perElementRandomisationScaleMin;      //
        float       m_perElementRandomisationScaleMax;      // .. and similarly for a local scale

    }               masking;


    // slightly nicer rendering for displacement lightmodes
    struct Displacement
    {
        rginline Displacement()
            : m_smoothing( true )
            , m_smoothingDistance( 2.0f )
            , m_smoothingSampleGrid( 5 )
        {}

        // enable jittered sampling to smooth out displacement results for LM_Radial_Displace_, LM_Angular_Displace_ 
        bool        m_smoothing;

        // pixel distance to jitter from original source sample; larger values means more smoothing / blurring
        float       m_smoothingDistance;

        // half-dimension of the sample grid; pass 5 for a 5x5 sample grid, so 25 samples; the higher, the slower, obviously
        int32_t     m_smoothingSampleGrid;

    }               displacement;


    // switches for using the newer noise library, as well as options to rescale / improve noise sampling
    struct Noise
    {
        rginline Noise()
            : m_boostVariance( false )
            , m_boostVarianceScale( 16.0f )
            , m_useHadesNoise( false )
            , m_globalNoiseScale( 1.0f )
            , m_additionalSeed( 0 )
        {}

        bool        m_boostVariance;
        float       m_boostVarianceScale;
        
        bool        m_useHadesNoise;
        float       m_globalNoiseScale;

        uint32_t    m_additionalSeed;

    }               noise;

    int32_t         m_allElementOverload;

    // in Negative or Subtractive modes, ignore alpha values to avoid punching transparency through results (otherwise it looks a bit broken, imho)
    bool            m_protectAlpha;

};
