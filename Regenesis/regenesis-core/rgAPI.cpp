/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#include "rgCommon.h"
#include "rgAPI.h"
#include "rgColour.h"
#include "rgIO.h"
#include "rgNoise.h"
#include "rgDeepMask.h"
#include "rgRender.h"
#include "rgMath.h"


// original noise, turbulence, etc; init() builds lut
namespace original_noise {

    void init();

} // namespace original_noise


// ------------------------------------------------------------------------------------------------
static void RenderFlareBlob(
    const GfxFile::FlareBlob&   blob,
    const RenderTweaks&         tweaks,
    const DistanceBuffer*       dmSGF,
    const RenderOutput&         renderOut,
    ColourFBuffer&              previousLayer,
    const Point2f&              centerPoint,
    const float                 rotation,
    const float                 localScale )
{
    const int32_t pipelineWidth  = renderOut.m_result->getWidth();
    const int32_t pipelineHeight = renderOut.m_result->getHeight();

    ColourFBuffer& workingBuffer = *renderOut.m_result;

    {
        FlareRenderer flare( tweaks );
        flare.Prepare( renderOut, blob );

        RenderCoordinate renderCoord;
        flare.ComputeForCenter( centerPoint, renderCoord );

        renderCoord.m_angleAdd += rotation;

        AUTO_PARALLELIZE_LOOP
        for ( int32_t _y = 0; _y < pipelineHeight; _y++ )
        {
            RenderCoordinate localRenderCoord = renderCoord;

            for ( int32_t _x = 0; _x < pipelineWidth; _x++ )
            {
                Point2f pixelPoint( (float)_x, (float)_y );

                RenderSamplePoint rsp;
                rsp.m_original   = pixelPoint;
                rsp.m_sampleHash = wangHash32( _x ^ _y );

                if ( dmSGF )
                {
                    pixelPoint -= ( *dmSGF )( _x, _y );

                    flare.ComputeForCenter( pixelPoint, localRenderCoord );

                    localRenderCoord.m_angleAdd += rotation;
                }

                const Point2f sPt = flare.Displace( rsp.m_original, localRenderCoord );
                rsp.calculateDerived( sPt );

                rsp.m_distValue *= localScale;

                flare.Render(
                    rsp,
                    localRenderCoord,
                    previousLayer,
                    workingBuffer( _x, _y ) );
            }
        }

        previousLayer.bufferCopyFrom( workingBuffer );
    }
}


namespace regenesis {

void init()
{
    original_noise::init();
}

// ------------------------------------------------------------------------------------------------
void render(
    const GfxFile&              gfx,                // genesis fx file to apply
    const RenderGlobals&        globals,            // common globals
          ColourFBuffer&        background,         // background plate; an image, or cleared to a colour, also used as a working buffer, hence not const
    const RenderTweaks&         tweaks,             // custom tweak options
          RenderOutput&         renderOut,          // output rendering configuration and target buffer
    const MaskProcessSettings&  deepMaskSettings,   // [ optional ] configure how to process mask layers, if specified
          DeepMask*             deepMask            // [ optional ] a populated mask instance that will be used - if !null - for flowed rendering
    )
{
    // instantiate the output buffer using the background input as reference
    renderOut.m_result = std::make_unique< ColourFBuffer >( background.getWidth(), background.getHeight() );
    renderOut.m_result->bufferCopyFrom( background );

    // rotation and center can be modified during rendering
    float   currentRotation = globals.m_rotation;
    Point2f currentCenterPt = globals.m_center;

    // .. mask settings can change too, if we are rendering each layer separately
    MaskProcessSettings currentMaskSettings = deepMaskSettings;

    auto deepMaskElementCount = ( deepMask != nullptr ) ? deepMask->getElements().size() : 0;

    // render each mask element individually
    if ( tweaks.masking.m_renderMaskElementsSeparately && 
         deepMaskElementCount > 1 )
    {
        int32_t currentMask = 1;

        RNG lrng( tweaks.masking.m_perElementRandomisationSeed );

        const MaskElements& maskElem = deepMask->getElements();
        for ( const MaskElement& me : maskElem )
        {
            currentMaskSettings.m_isolateElement = currentMask++;
            deepMask->postprocess( currentMaskSettings );

            currentCenterPt  = deepMask->getMaskedCentroid();
            float localScale = 1.0f;

            if ( tweaks.masking.m_perElementRandomisationTime > 0 )
            {
                renderOut.m_time += lrng.genFloat( 
                    0.0f, 
                    tweaks.masking.m_perElementRandomisationTime );
            }
            else { lrng.genUInt32(); }

            if ( tweaks.masking.m_perElementRandomisationRotation > 0 )
            {
                currentRotation  += lrng.genFloat( 
                   -tweaks.masking.m_perElementRandomisationRotation, 
                    tweaks.masking.m_perElementRandomisationRotation );
            }
            else { lrng.genUInt32(); }

            if ( tweaks.masking.m_perElementRandomisationScaleMin != tweaks.masking.m_perElementRandomisationScaleMax )
            {
                localScale       *= lrng.genFloat( 
                    tweaks.masking.m_perElementRandomisationScaleMin, 
                    tweaks.masking.m_perElementRandomisationScaleMax );
            }
            else { lrng.genUInt32(); }


            ScopedTimer _t( "Flare" );

            for ( const GfxFile::FlareBlob& blob : gfx.flares )
            {
                RenderFlareBlob(
                    blob,
                    tweaks,
                    deepMask->getSGFIfProcessed(),
                    renderOut,
                    background,
                    currentCenterPt,
                    currentRotation,
                    localScale );
            }

            background.bufferCopyFrom( *renderOut.m_result );
        }
    }
    // .. or render the mask or image in one go
    else
    {
        if ( deepMaskElementCount > 0 )
        {
            deepMask->postprocess( currentMaskSettings );
            currentCenterPt = deepMask->getMaskedCentroid();
        }

        ScopedTimer _t( "Flare" );

        for ( const GfxFile::FlareBlob& blob : gfx.flares )
        {
            RenderFlareBlob(
                blob,
                tweaks,
                deepMask ? deepMask->getSGFIfProcessed() : nullptr,
                renderOut,
                background,
                currentCenterPt,
                currentRotation,
                1.0f );
        }
    }
}

} // namespace regenesis