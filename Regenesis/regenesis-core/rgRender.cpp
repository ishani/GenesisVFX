/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#include "rgCommon.h"
#include "rgRender.h"
#include "rgDeepMask.h"
#include "rgIO.h"
#include "rgNoise.h"
#include "rgMath.h"
#include "rgTween.h"

// ---------------------------------------------------------------------------------------------
struct FlareRenderer::LayerContext
{
    LayerContext(
        const RenderOutput& routput,
        const GfxFile::FlareBlob& blob,
        const RenderTweaks& tweaks )
        : m_pos( blob.m_pos_keys[0] )
        , m_shape( blob.m_shape_keys[0] )
        , m_lightMode( ( gfxdata::LightMode ) blob.m_header.m_light )
        , m_blobOffScreenRender( blob.m_header.m_offScreenRender )
        , m_blobRealign( blob.m_header.m_reAlign )
        , m_blobPolar( blob.m_header.m_polar )
        , m_hackInversion( strstr(blob.m_header.m_name, "~INVERT") != nullptr )
        , m_allElementOverload( tweaks.m_allElementOverload )
        , m_hsvShift( routput.m_hueShift, routput.m_saturationShift, routput.m_valueShift )
        , m_hsvShiftApply( routput.m_hueShift != 0.0f || routput.m_saturationShift != 0.0f || routput.m_valueShift != 0.0f )
        , m_fractal( nullptr )
    {
        m_layerWidth      = (float)routput.m_result->getWidth();
        m_layerHeight     = (float)routput.m_result->getHeight();

        m_layerWidthRecp  = 1.0f / m_layerWidth;
        m_layerHeightRecp = 1.0f / m_layerHeight;

        m_renderSize      = m_pos.m_flareSize * routput.m_globalScale;
        m_radiusM         = c_FlareResF / ( m_renderSize * routput.m_flareSizeEdge );

        if ( blob.m_header.m_linked )
        {
            m_renderIntensity = blob.m_header.m_IR2 / ( m_renderSize * m_renderSize );
        }
        else
        {
            m_renderIntensity = m_pos.m_intensity;
        }


        m_baseColourise = routput.m_colourise;

        m_renderIntensityColour.r = ( ( 1.0f - m_pos.m_colourise ) + m_pos.m_colourise * m_baseColourise.r ) * m_renderIntensity;
        m_renderIntensityColour.g = ( ( 1.0f - m_pos.m_colourise ) + m_pos.m_colourise * m_baseColourise.g ) * m_renderIntensity;
        m_renderIntensityColour.b = ( ( 1.0f - m_pos.m_colourise ) + m_pos.m_colourise * m_baseColourise.b ) * m_renderIntensity;
        m_renderIntensityColour.a = ( ( 1.0f - m_pos.m_colourise ) + m_pos.m_colourise * m_baseColourise.a ) * m_renderIntensity;


        if ( blob.m_inferno.m_useFracNoise ||
             blob.m_inferno.m_useSimpleNoise )
        {
            m_fractal              = &blob.m_inferno;

            m_inferno.m_renderSize = m_renderSize * routput.m_flareSizeEdge;
            m_inferno.m_time       = routput.m_time;

            m_hades = std::make_unique< Hades::FractalConfig >( blob.m_inferno.m_seed, blob.m_inferno.m_fracFreq );
        }
    }

    const gfxdata::FlarePosAnim     m_pos;
    const FlareRenderShape          m_shape;

    const gfxdata::LightMode        m_lightMode;
    const int32_t                   m_blobOffScreenRender;
    const bool                      m_blobRealign;
    const bool                      m_blobPolar;

    const bool                      m_hackInversion;

    const int32_t                   m_allElementOverload;

    const Point3f                   m_hsvShift;
    const bool                      m_hsvShiftApply;

    Inferno::Setup                  m_inferno;
    const gfxdata::Fractal*         m_fractal;
    Hades::ConfigPtr                m_hades;


    float                           m_layerWidth;
    float                           m_layerHeight;

    float                           m_layerWidthRecp;
    float                           m_layerHeightRecp;

    float                           m_renderSize;
    float                           m_radiusM;

    float                           m_renderIntensity;
    ColourF                         m_renderIntensityColour;

    ColourF                         m_baseColourise;
};


// ---------------------------------------------------------------------------------------------
FlareRenderer::FlareRenderer( const RenderTweaks& tweaks )
    : m_tweaks( tweaks )
{
}

// ---------------------------------------------------------------------------------------------
FlareRenderer::~FlareRenderer()
{
}

// ---------------------------------------------------------------------------------------------
void FlareRenderer::Prepare(
    const RenderOutput&         routput,
    const GfxFile::FlareBlob&   blob )
{
    context = std::make_unique<LayerContext>( routput, blob, m_tweaks );
}


// ---------------------------------------------------------------------------------------------
void FlareRenderer::ComputeForCenter(
    const Point2f&              centerPt,
    RenderCoordinate&           coordinate ) const
{
    const LayerContext& layer = *context;

    float local_centroidX = centerPt.x;
    float local_centroidY = centerPt.y;

    if ( layer.m_blobOffScreenRender & 2 )
    {
        coordinate.m_screenCX = local_centroidX; local_centroidX -= layer.m_layerWidth;
        coordinate.m_screenCY = local_centroidY;
    }
    else
    {
        coordinate.m_screenCX = ( 0.5f + layer.m_pos.m_positionX * 0.5f ) * layer.m_layerWidth;
        coordinate.m_screenCY = ( 0.5f + layer.m_pos.m_positionY * 0.5f ) * layer.m_layerHeight;
    }


    float dx1 = local_centroidX - coordinate.m_screenCX;
    float dy1 = local_centroidY - coordinate.m_screenCY;

    float dx2 = -dy1;
    float dy2 = dx1;


    float coordTrueX;
    float coordTrueY;

    // void ConvertCoordinates( void )
    {
        if ( !layer.m_blobPolar )
        {
            coordTrueX = layer.m_pos.m_position;
            coordTrueY = layer.m_pos.m_angle;
        }
        else
        {
            coordTrueX = layer.m_pos.m_position * (float)cos( layer.m_pos.m_angle * c_pi );
            coordTrueY = layer.m_pos.m_position * (float)sin( layer.m_pos.m_angle * c_pi );
        }
    }

    coordinate.m_renderCX = ( coordTrueX * dx1 ) + ( coordTrueY * dx2 );
    coordinate.m_renderCY = ( coordTrueX * dy1 ) + ( coordTrueY * dy2 );

    coordinate.m_renderCX += coordinate.m_screenCX;
    coordinate.m_renderCY += coordinate.m_screenCY;

    if ( layer.m_blobOffScreenRender & 2 )
    {
        coordinate.m_screenCX = ( 0.5f + layer.m_pos.m_positionX * 0.5f ) * layer.m_layerWidth;
        coordinate.m_screenCY = ( 0.5f + layer.m_pos.m_positionY * 0.5f ) * layer.m_layerHeight;
    }


    coordinate.m_angleAdd = ( -0.5f * layer.m_pos.m_angleOffSet * c_FlareResF );
    if ( layer.m_blobRealign )
    {
        coordinate.m_angleAdd -= c_FlareResF * (float)atan2(
            coordinate.m_renderCY - coordinate.m_screenCY,
            coordinate.m_renderCX - coordinate.m_screenCX ) / c_two_pi;
    }
    while ( coordinate.m_angleAdd < 0 )
        coordinate.m_angleAdd += c_FlareResF;
}


// ---------------------------------------------------------------------------------------------
Point2f FlareRenderer::Displace(
    const Point2f&              in,
    const RenderCoordinate&     coordinate ) const
{
    float outX = in.x;
    float outY = in.y;

    if ( context->m_fractal != nullptr )
    {
        // create a noise configuration with our current render center
        Inferno::Setup localInferno( context->m_inferno );
        localInferno.m_cenx = coordinate.m_renderCX;
        localInferno.m_ceny = coordinate.m_renderCY;

        // push m_time based on XY position, causing additional subtle noise variance across
        // the image; can help break up repetative effects when used on masks
        if ( m_tweaks.noise.m_boostVariance )
        {
            const float boostMultiplier = m_tweaks.noise.m_boostVarianceScale;

            localInferno.m_time += in.x * boostMultiplier * context->m_layerWidthRecp;
            localInferno.m_time += in.y * boostMultiplier * context->m_layerHeightRecp;
        }

        // optionally switch to the remixed 'Hades' noise library that offers some mutation 
        // options compared to the original Inferno
        if ( m_tweaks.noise.m_useHadesNoise )
        {
            Hades::Distort(
                *( context->m_fractal ),
                *( context->m_hades ),
                localInferno,
                m_tweaks,
                outX,
                outY,
                in.x,
                in.y );
        }
        else
        {
            Inferno::Distort(
                *( context->m_fractal ),
                localInferno,
                m_tweaks,
                outX,
                outY,
                in.x,
                in.y );
        }
    }

    outX -= coordinate.m_renderCX;
    outY -= coordinate.m_renderCY;

    return Point2f( outX, outY );
}

#ifdef DEBUG

// ---------------------------------------------------------------------------------------------
// debug function that tracks and reports on light modes we either don't recognise (and are
// probably from corrupt files) or those that we haven't implemented yet
//
static void LightModeDebugWarning( int32_t lm )
{
    static bool s_corruptWarning = false;
    if ( lm > gfxdata::LM_Image_Warp_RGB )
    {
        if ( !s_corruptWarning )
        {
            rgLogOutput( LogLevel::Error, "\nWARNING : corrupt/invalid light mode? %i!\n\n", lm );
            s_corruptWarning = true;
        }
    }
    else
    {
        static std::array< bool, 20 > s_warnings{ { false } };
        if ( !s_warnings[lm] )
        {
            rgLogOutput( LogLevel::Error, "\nWARNING : using unimplemented light mode %i!\n\n", lm );
            s_warnings[lm] = true;
        }
    }
}

#endif // DEBUG

// ---------------------------------------------------------------------------------------------
rginline void ShiftHSV( ColourF& input, const Point3f& shift )
{
    ColourHSV hsv(input);

    // push H around, clip it to fractional to wrap
    hsv.h  = fabsf( fractf( hsv.h + shift.x ) );

    if ( shift.y < 0 )
        hsv.s = rgLerp( hsv.s, 0.0f, -shift.y );
    else
        hsv.s = saturate( hsv.s * (1.0f + shift.y) );

    // multiply V
    hsv.v  = saturate( hsv.v * (1.0f + shift.z) );

    input  = ColourF(hsv);
}

// ---------------------------------------------------------------------------------------------
bool FlareRenderer::Sample(
    const float     sampleAngValue,
    const float     sampleDistValue,
    ColourF&        outputColour,
    ColourF&        outputStreak ) const
{
    const FlareRenderShape& renderShape = context->m_shape;

    float streakRadial = constrainf( sampleAngValue, 0.0f, c_FlareResF );

    const float shapeN = renderShape.lerp( true, renderShape.m_shape, streakRadial );
    const float flareDist = sampleDistValue / shapeN;

    if ( flareDist < gfxdata::FLARERESOLUTION )
    {
        streakRadial += renderShape.lerp( false, renderShape.m_shape2, flareDist );
        streakRadial  = constrainf( streakRadial, 0.0f, c_FlareResF );

        outputColour  = renderShape.lerp( false, renderShape.m_col, flareDist );
        outputStreak  = renderShape.lerp( true, renderShape.m_streaks, streakRadial );

        if ( context->m_hsvShiftApply )
        {
            ShiftHSV( outputColour, context->m_hsvShift );
            ShiftHSV( outputStreak, context->m_hsvShift );
        }

        return true;
    }

    return false;
}


// ---------------------------------------------------------------------------------------------
float RunExtraJitterSamples(
    const RenderTweaks&         tweaks,
    const RenderSamplePoint&    samplePoint,
    const float                 distanceMultiplier,
    const std::function<void( const Point2f& offset, const float contribution )>& sampleFunction )
{
    float contributionTotal = 1.0f;

    const int32_t sampleGrid         = tweaks.displacement.m_smoothingSampleGrid;
    const int32_t sampleCount        = (sampleGrid * sampleGrid);

    // spread out the jitter samples
    const float pixelSmoothingRadius = distanceMultiplier * tweaks.displacement.m_smoothingDistance;

    for (int32_t sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++)
    {
        // get a jittered offset, N of a MxM grid; these are 0..1 so -0.5 to change them to -0.5 .. 0.5
        Point2f offset = cmjSampling::generate( sampleIndex, sampleGrid, sampleGrid, (int32_t)samplePoint.m_sampleHash ) - 0.5f;

        // work out a contribution for this sample - based on how far from the original sample point it is
        const float sampleContribution = 1.0f - sqrtf( offset.lengthSquared() );
        contributionTotal += sampleContribution;

        // move this new sample away from the original
        offset *= pixelSmoothingRadius;

        sampleFunction(offset, sampleContribution);
    }

    return contributionTotal;
}


// ---------------------------------------------------------------------------------------------
void FlareRenderer::Render(
    const RenderSamplePoint&    samplePoint,
    const RenderCoordinate&     coordinate,
    const ColourFBuffer&        sourceImage,
    ColourF&                    sourcePixel ) const
{
    const LayerContext&           layer = *context;
    const FlareRenderShape& renderShape = layer.m_shape;


    ColourF sampleColour;
    ColourF sampleStreak;

    const float sampleAngleIn    = ( c_FlareResRadial * samplePoint.m_angValue ) + coordinate.m_angleAdd;
    const float sampleDistanceIn = context->m_radiusM * samplePoint.m_distValue;

    bool sampleOk = Sample(
        sampleAngleIn,
        sampleDistanceIn,
        sampleColour,
        sampleStreak );

    if ( sampleOk )
    {
        const ColourF renderIntensity = layer.m_renderIntensityColour;

        ColourF blendColour = renderIntensity * sampleColour * sampleStreak;
        blendColour.ClampHigh();

        gfxdata::LightMode currentLightMode = layer.m_lightMode;
        if ( layer.m_allElementOverload >= 0 )
        {
            currentLightMode = (gfxdata::LightMode)layer.m_allElementOverload;
        }

        switch ( currentLightMode )
        {
            // unsupported light modes fall through to LM_Light, but warn on first usage
            default:
#ifdef DEBUG
                LightModeDebugWarning( currentLightMode );
#endif // DEBUG

            case gfxdata::LM_Light: // 0
            {
                sourcePixel.r = 1.0f - ( 1.0f - sourcePixel.r ) * ( 1.0f - blendColour.r );
                sourcePixel.g = 1.0f - ( 1.0f - sourcePixel.g ) * ( 1.0f - blendColour.g );
                sourcePixel.b = 1.0f - ( 1.0f - sourcePixel.b ) * ( 1.0f - blendColour.b );

                sourcePixel.a += blendColour.ColourAvg();
                sourcePixel.a = std::min( sourcePixel.a, 1.0f );
            }
            break;

            case gfxdata::LM_Object: // 1
            {
                sourcePixel.ObjectBlend( blendColour );
            }
            break;

            case gfxdata::LM_Tint: // 2
            {
                // avoid destroying alpha if tweak is set
                if (m_tweaks.m_protectAlpha)
                    blendColour.a = 1.0;

                sourcePixel *= blendColour;
            }
            break;

            case gfxdata::LM_Add: // 3
            {
                sourcePixel += blendColour;
                sourcePixel.ClampHigh();
            }
            break;

            case gfxdata::LM_Subtract:  // 4
            {
                // avoid subtracting alpha if tweak is set
                if ( m_tweaks.m_protectAlpha )
                    blendColour.a = 0.0;

                sourcePixel -= blendColour;
                sourcePixel.ClampLow();
            }
            break;

            case gfxdata::LM_Negative:  // 5
            {
                // avoid subtracting alpha if tweak is set
                if ( m_tweaks.m_protectAlpha )
                    blendColour.a = 0.0;

                sourcePixel.NegativeBlend( blendColour );
            }
            break;

            case gfxdata::LM_Angular_Displace_Intensity:    // 8
            case gfxdata::LM_Radial_Displace_Intensity:     // 10
            {
                const float intensity = blendColour.ColourAvg();
                const bool useAngular = ( currentLightMode == gfxdata::LM_Angular_Displace_Intensity );

                if ( intensity > 0.0f )
                {
                    float pixelShift = intensity;

                    if ( useAngular )
                    {
                        pixelShift *= c_pi;
                        pixelShift = std::min( pixelShift, c_pi );
                    }
                    else
                    {
                        pixelShift = std::max( 1.0f - pixelShift, 0.0f );
                    }

                    const float originX = samplePoint.m_original.x - coordinate.m_renderCX;
                    const float originY = samplePoint.m_original.y - coordinate.m_renderCY;

                    float displaceX = coordinate.m_renderCX;
                    float displaceY = coordinate.m_renderCY;

                    if ( useAngular )
                    {
                        float m_origAng;
                        float m_origDist;
                        RenderSamplePoint::outputsFromPoint( Point2f( originX, originY ), m_origAng, m_origDist );

                        displaceX += m_origDist * cos( m_origAng + pixelShift );
                        displaceY += m_origDist * sin( m_origAng + pixelShift );
                    }
                    else
                    {
                        displaceX += ( pixelShift * originX );
                        displaceY += ( pixelShift * originY );
                    }

                    // sample from the displaced location
                    ColourF base = bilinearSample( sourceImage, displaceX, displaceY );

                    // do multiple jittered samples instead of just the one?
                    if ( m_tweaks.displacement.m_smoothing )
                    {
                        float contributionTotal = 1.0f;

                        const int32_t sampleGrid = m_tweaks.displacement.m_smoothingSampleGrid;
                        const int32_t sampleCount = ( sampleGrid * sampleGrid );

                        // we use a multiplied intensity value to spread out the jitter samples;
                        // this means that near 0 it won't blur anything, nearer 1.0 it will
                        const float intensityBlend = std::min( 1.0f, intensity * 8.0f );
                        const float pixelSmoothingRadius = intensityBlend * m_tweaks.displacement.m_smoothingDistance;

                        for ( int32_t sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++ )
                        {
                            // get a jittered offset, N of a MxM grid; these are 0..1 so -0.5 to change them to -0.5 .. 0.5
                            Point2f offset = cmjSampling::generate( sampleIndex, sampleGrid, sampleGrid, (int32_t)samplePoint.m_sampleHash ) - 0.5f;

                            // work out a contribution for this sample - based on how far from the original sample point it is
                            const float sampleContribution = 1.0f - sqrtf( offset.lengthSquared() );
                            contributionTotal += sampleContribution;


                            // move this new sample away from the original
                            offset *= pixelSmoothingRadius;

                            // add in another sample
                            base += bilinearSample( sourceImage, displaceX + offset.x, displaceY + offset.y ) * sampleContribution;
                        }

                        // divide by total sample contribution to produce average
                        base /= contributionTotal;
                    }

                    // maintain original alpha
                    base.a = sourcePixel.a;

                    sourcePixel = base;
                }
            }
            break;

            case gfxdata::LM_Image_Warp_Intensity:    // 12
            {
                const float multiplier = context->m_hackInversion ? -1.0f : 1.0f;

                const float intensity = blendColour.ComputeIntensity();
                if ( intensity > 0.0f )
                {
                    ColourF sampleResult(0);

                    auto samplerFn = [&samplePoint, &sourceImage, &sampleResult, intensity, multiplier](const Point2f& offset, const float sampleContribution)
                    {
                        const float intensityOffset = intensity + offset.x;

                        const Point2f shiftPos = samplePoint.m_original + (samplePoint.m_displaced * intensityOffset * multiplier);

                        sampleResult += bilinearSample(sourceImage, shiftPos.x, shiftPos.y) * sampleContribution;
                    };

                    samplerFn( Point2f(0, 0), 1.0f );

                    if ( m_tweaks.displacement.m_smoothing )
                    {
                        float total = RunExtraJitterSamples(m_tweaks, samplePoint, intensity * 0.01f, samplerFn );
                        sampleResult /= total;
                    }

                    sampleResult.a = sourcePixel.a;
                    sourcePixel = sampleResult;
                }
            }
            break;

            case gfxdata::LM_Image_Warp_RGB:          // 13
            {
                const float multiplier = context->m_hackInversion ? -1.0f : 1.0f;

                ColourF result = sourcePixel;

                for ( size_t ch = 0; ch < 3; ch++ )
                {
                    const float intensity = blendColour[ch];
                    if ( intensity > 0.0f )
                    {
                        float sampleResult = 0;

                        auto samplerFn = [&samplePoint, &sourceImage, &sampleResult, intensity, multiplier, ch](const Point2f& offset, const float sampleContribution)
                        {
                            const float intensityOffset = intensity + offset.x;

                            const Point2f shiftPos = samplePoint.m_original + (samplePoint.m_displaced * intensityOffset * multiplier);

                            sampleResult += bilinearSampleChannel(sourceImage, shiftPos.x, shiftPos.y, ch) * sampleContribution;
                        };

                        samplerFn( Point2f(0, 0), 1.0f );

                        if ( m_tweaks.displacement.m_smoothing )
                        {
                            float total = RunExtraJitterSamples(m_tweaks, samplePoint, intensity * 0.01f, samplerFn );
                            sampleResult /= total;
                        }

                        result.setChannel( ch, sampleResult );
                    }
                }

                result.a = sourcePixel.a;
                sourcePixel = result;
            }
            break;
        }

    }
}
