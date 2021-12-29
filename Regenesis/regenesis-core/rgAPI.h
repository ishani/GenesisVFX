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


class GfxFile;
struct RenderTweaks;
struct RenderOutput;
struct MaskProcessSettings;
class DeepMask;

// ------------------------------------------------------------------------------------------------
struct RenderGlobals
{
    rginline RenderGlobals()
        : m_time( 0.0f )
        , m_scale( 1.0f )
        , m_rotation( 0.0f )
        , m_center( 0.0f, 0.0f )
    {}

    float           m_time;
    float           m_scale;
    float           m_rotation;
    Point2f         m_center;
};


// our condensed public API
namespace regenesis {

// ------------------------------------------------------------------------------------------------
// call once before anything else, builds LUTs etc
void init();

// ------------------------------------------------------------------------------------------------
// execute a GenesisVFX file
void render(
    const GfxFile&              gfx,                // genesis fx file to apply
    const RenderGlobals&        globals,            // common globals
          ColourFBuffer&        background,         // background plate; an image, or cleared to a colour, also used as a working buffer, hence not const
    const RenderTweaks&         tweaks,             // custom tweak options
          RenderOutput&         renderOut,          // output rendering configuration and target buffer
    const MaskProcessSettings&  deepMaskSettings,   // [ optional ] configure how to process mask layers, if specified
          DeepMask*             deepMask            // [ optional ] a populated mask instance that will be used - if !null - for flowed rendering
    );

} // namespace regenesis