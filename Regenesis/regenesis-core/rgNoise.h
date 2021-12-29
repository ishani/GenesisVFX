/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#pragma once

namespace gfxdata { struct Fractal; }

class FastNoise;
struct RenderTweaks;

// Inferno :- from the original implementation, this wraps up code in Fractal .h/.cpp 
namespace Inferno {

    struct Setup
    {
        float m_renderSize;
        float m_cenx;
        float m_ceny;
        float m_time;
    };

    void Distort( 
        const gfxdata::Fractal& iconfig,    // gfx state configuration
        const Setup& isetup,                // render-time setup block
        const RenderTweaks& tweaks,
        float &xnew, 
        float &ynew, 
        float xin, 
        float yin );

} // namespace Inferno


// Hades :- our new, fancier, faster noise implementation
namespace Hades {

    struct FractalConfig
    {
        FractalConfig( const int32_t seed, const float frequency );
        ~FractalConfig();

        std::unique_ptr<FastNoise> m_fn;
    };

    using ConfigPtr = std::unique_ptr<FractalConfig>;

    void Distort(
        const gfxdata::Fractal& iconfig,    // gfx state configuration
        const FractalConfig&    fconfig,    // preprocessed config from gfx state
        const Inferno::Setup&   isetup,     // render-time setup block
        const RenderTweaks&     tweaks,
        float &xnew,
        float &ynew,
        float xin,
        float yin );

} // namespace Hades