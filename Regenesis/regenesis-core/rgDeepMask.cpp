/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#include "rgCommon.h"
#include "rgDeepMask.h"
#include "rgTween.h"
#include "rgMath.h"

#include "FastNoise.h"

// enable this define to have debug images exported into local directories; /debug/blur, /debug/fill, /debug/sdf, /debug/udf
// note that it expects the directories to already exist currently, it won't go and make them itself
// #define DEBUG_DEEP_MASK_PHASES
#define DEBUG_DEEP_MASK_RELDIR      "./debug/"

// ---------------------------------------------------------------------------------------------
MaskElement FloodFill(
    BitBuffer&              maskPixels,             // all mask pixels tagged on
    MonoBufferU8&           maskElements,           // output IDs buffer, each that is part of the mask holding which element # it is part of
    const uint8_t           maskCurrentElement,     // ... and which element we're currently building
    const int32_t           startPixelX,
    const int32_t           startPixelY )
{
    const int32_t maskWidth     = maskPixels.getWidth();
    const int32_t maskHeight    = maskPixels.getHeight();

    // allocate a buffer to track which pixels we want to examine for neighbouring mask pixels to expand the fill into
    Buffer2D<bool> toExplore( maskWidth, maskHeight );

    // to massively speed up multi-pass filling we flip between TL->BR and BR->TL each time
    // otherwise if the process hits a feature that goes up-right, it takes a pass for each climb, as we only
    // check one pixel upwards each time
    bool flipFillDirection = false;


    // track how many pixels we successfully fill into (to later check we have found every marked mask pixel in the image)
    uint32_t remaskedPixels = 0;

    // track how many passes it took, for debug/perf analysis reasons
    uint32_t floodFillePasses = 0;

    // each filled element has a centroid calculated on the fly
    Point2d centroid( 0.0f, 0.0f );


    // offsets to check from a given explorable pixel, searching for adjacent mask pixels to fill
    //  [ ]  [.]  [ ]
    //  [ ]  [+]  [.]
    //  [.]  [.]  [ ]
    static const std::array<int32_t, 4> offsetY{ { -1,  0,   1,  1 } };
    static const std::array<int32_t, 4> offsetX{ { 0,  1,  -1,  0 } };

    // kick things off by tagging the start pixel to begin exploring from
    toExplore( startPixelX, startPixelY ) = true;

#ifdef DEBUG_DEEP_MASK_PHASES
    ColourI8Buffer snapshot( maskWidth, maskHeight );
    snapshot.clear( ColourI8::s_black );
#endif // DEBUG_DEEP_MASK_PHASES

    bool newPixelsToExplore;
    do
    {

        // if we add any new pixels to explore, we need to run another pass to make sure they are all examined;
        // 
        newPixelsToExplore = false;

#ifdef DEBUG_DEEP_MASK_PHASES
        // debug: colours each pass distinctly, previously touched pixels shift to red
        snapshot.forEach( []( const int32_t index, ColourI8& c ) {
            if ( c.g > 250 )
            {
                c.r = c.g;
                c.g = 50;
            }
        });

        if ( remaskedPixels > 0 )
        {
            int32_t ctX = (int32_t)(centroid.x / (double)remaskedPixels);
            int32_t ctY = (int32_t)(centroid.y / (double)remaskedPixels);
            snapshot( ctX, ctY ).r = 150;
            snapshot( ctX, ctY ).b = 255;
        }
#endif // DEBUG_DEEP_MASK_PHASES

        for ( int32_t _y = 0; _y < maskHeight; _y++ )
        {
            for ( int32_t _x = 0; _x < maskWidth; _x++ )
            {
                // optionally mirror _x, _y if we're reading BR->TL
                const int32_t readX = flipFillDirection ? ( ( maskWidth - 1 ) - _x ) : _x;
                const int32_t readY = flipFillDirection ? ( ( maskHeight - 1 ) - _y ) : _y;


                if ( toExplore( readX, readY ) )
                {
                    // stash the current flag into the mask buffer, ensuring we are not overwriting a previous result
                    ensure( maskElements( readX, readY ) == 0 );
                    maskElements( readX, readY ) = maskCurrentElement;
                    maskPixels( readX, readY ) = false;

                    // we've processed this pixel now, untouch it
                    toExplore( readX, readY ) = false;

                    // keep track of how many pixels we hit
                    remaskedPixels ++;

                    // .. and sum up all the positions so we can easily compute a per-element centroid
                    centroid.x += (double)readX;
                    centroid.y += (double)readY;

                    // walk pixels around the current explorable x,y
                    for ( int32_t offsetIdx = 0; offsetIdx < 4; offsetIdx ++ )
                    {
                        const int32_t off_x = flipFillDirection ? -offsetX[offsetIdx] : offsetX[offsetIdx];
                        const int32_t off_y = flipFillDirection ? -offsetY[offsetIdx] : offsetY[offsetIdx];

                        // make sure we are still in-bounds
                        const int32_t testX = readX + off_x;
                        const int32_t testY = readY + off_y;
                        if ( testX >= 0 &&
                             testY >= 0 &&
                             testX < maskWidth &&
                             testY < maskHeight )
                        {
                            if ( maskPixels( testX, testY ) )
                            {
                                // tag this pixel as one we want to investigate in the future
                                toExplore( testX, testY ) = true;

#ifdef DEBUG_DEEP_MASK_PHASES
                                snapshot( testX, testY ).g = 255;
#endif // DEBUG_DEEP_MASK_PHASES

                                newPixelsToExplore = true;
                            }

                        } // in-bounds check

                    } // offsetIdx 

                } // is pixel explorable

            } // _x
        } // _y

        floodFillePasses ++;

        // flip the direction for the next pass
        flipFillDirection = !flipFillDirection;


#ifdef DEBUG_DEEP_MASK_PHASES
        {
            char _snapPath[255];
            sprintf( _snapPath, DEBUG_DEEP_MASK_RELDIR "fill/pass_%02x_%u.png", maskCurrentElement, floodFillePasses );
            writeOutBufferAsImage( snapshot, _snapPath, OutputFormat::AsPNG, true );
        }
#endif // DEBUG_DEEP_MASK_PHASES


        // as soon as we do a pass where no new pixels are found, we can assume the shape is filled
    } while ( newPixelsToExplore );


    centroid /= (double)remaskedPixels;

    return MaskElement( remaskedPixels, floodFillePasses, centroid );
}


// ---------------------------------------------------------------------------------------------
void UDF(
    MonoBufferF&            udfOut,                         // resulting distance field to use to render flares
    const BitBuffer&        maskInput,                      // input mask to seed the field with 
    const MonoBufferU8*     maskElementBuffer = nullptr,    // [optional] can just build from a specific mask element, with isolateElement 
    const int32_t           isolateElement = -1             // [optional] choose which mask ID to use, -1 for all
)
{
    const int32_t maskWidth     = maskInput.getWidth();
    const int32_t maskHeight    = maskInput.getHeight();
    const int32_t maskPixels    = maskWidth * maskHeight;

    ensure( maskWidth  == udfOut.getWidth() );
    ensure( maskHeight == udfOut.getHeight() );


    // optionally burn out mid-processing states of the UDF to see how the values are swept around
#ifdef DEBUG_DEEP_MASK_PHASES
    char _snapPath[255];

    ColourFBuffer snapshot( maskWidth, maskHeight );
    snapshot.clear( ColourF::s_black );

    // define a function that will walk the current udf and emit the distance values as a gradient, with the core mask tagged white
    auto do_snapshot = [&](const char* phase) {
        static const float gradientScaleRecp = 3.0f / std::max( maskWidth, maskHeight );

        snapshot.forEach( [&]( const int32_t index, ColourF& c ) {
            if ( udfOut[index] >= FLT_MAX )
            {
                c.r = 0.0f;
            }
            else if ( udfOut[index] > 0.0f )
            {
                float hue = (float)udfOut[index] * gradientScaleRecp;
                c = ColourF::gradientPlasma( 1.0f - pingpong01(hue) );
            }
            else
            {
                c.r = c.g = c.b = 1.0f;
            }
        });

        sprintf( _snapPath, DEBUG_DEEP_MASK_RELDIR "udf/%s.png", phase );
        writeOutBufferAsImage( snapshot, _snapPath, OutputFormat::AsPNG, true );
    };
#endif // DEBUG_DEEP_MASK_PHASES



    // seed the output buffer for the distance processing
    // we write very large values into the areas we want to 'bleed' distance values into from
    // the masked components (which are set to 0)
    // .. the precise 'why' of that setup is better explained with pictures, or by looking at the
    // distance field code below this
    //
    for ( int32_t _p = 0; _p < maskPixels; _p++ )
    {
        // default is something big
        float dfSeed = (float)std::max( maskWidth, maskHeight ) * 3.0f;

        // if this pixel is in the mask, we clear it to 0 ..
        if ( maskInput[_p] )
        {
            // check if we should only be clearing a particular element's pixels
            if ( maskElementBuffer != nullptr && isolateElement >= 0 )
            {
                const uint8_t maskID = (*maskElementBuffer)[_p];
                if ( maskID == isolateElement )
                {
                    dfSeed = 0.0f;
                }
            }
            else
            {
                dfSeed = 0.0f;
            }
        }

        udfOut[_p] = dfSeed;
    }


    //
    //      XX
    //   XX --
    //

    for ( int32_t x = 1; x != maskWidth; x++ )
    {
        const float x1 = udfOut( x - 1, 0 ) + 1;
        if ( x1 < udfOut( x, 0 ) )
        {
            udfOut( x, 0 ) = x1;
        }
    }

    for ( int32_t y = 1; y != maskHeight; y++ )
    {
        {
            const float y1 = udfOut( 0, y - 1 ) + 1;
            if ( y1 < udfOut( 0, y ) )
            {
                udfOut( 0, y ) = y1;
            }
        }

        for ( int32_t x = 1; x != maskWidth; x++ )
        {
            const float x1 = udfOut( x - 1, y ) + 1;
            const float y1 = udfOut( x, y - 1 ) + 1;

            if ( x1 < udfOut( x, y ) )
                udfOut( x, y ) = x1;

            if ( y1 < udfOut( x, y ) )
                udfOut( x, y ) = y1;
        }
    }

#ifdef DEBUG_DEEP_MASK_PHASES
    do_snapshot( "sweep1" );
#endif // DEBUG_DEEP_MASK_PHASES

    //
    //   -- XX
    //   XX 
    //

    for ( int32_t x = maskWidth - 2; x >= 0; x-- )
    {
        const float x1 = udfOut( x + 1, 0 ) + 1;
        if ( x1 < udfOut( x, 0 ) )
            udfOut( x, 0 ) = x1;
    }

    for ( int32_t y = maskHeight - 2; y >= 0; y-- )
    {
        {
            const float y1 = udfOut( 0, y + 1 ) + 1;
            if ( y1 < udfOut( 0, y ) )
                udfOut( 0, y ) = y1;
        }

        for ( int32_t x = maskWidth - 2; x >= 0; x-- )
        {
            const float x1 = udfOut( x + 1, y ) + 1;
            const float y1 = udfOut( x, y + 1 ) + 1;

            if ( x1<udfOut( x, y ) )
                udfOut( x, y ) = x1;

            if ( y1<udfOut( x, y ) )
                udfOut( x, y ) = y1;
        }
    }

#ifdef DEBUG_DEEP_MASK_PHASES
    do_snapshot( "sweep2" );
#endif // DEBUG_DEEP_MASK_PHASES


    //
    //   XX
    //   -- XX
    //

    for ( int32_t x = maskWidth - 2; x != 0; x-- )
    {
        const float x1 = udfOut( x + 1, 0 ) + 1;
        if ( x1 < udfOut( x, 0 ) )
            udfOut( x, 0 ) = x1;
    }

    for ( int32_t y = 1; y != maskHeight; y++ )
    {
        {
            const float y1 = udfOut( 0, y - 1 ) + 1;
            if ( y1 < udfOut( 0, y ) )
                udfOut( 0, y ) = y1;
        }

        for ( int32_t x = maskWidth - 2; x != 0; x-- )
        {
            const float x1 = udfOut( x + 1, y ) + 1;
            const float y1 = udfOut( x, y - 1 ) + 1;

            if ( x1<udfOut( x, y ) )
                udfOut( x, y ) = x1;
            if ( y1<udfOut( x, y ) )
                udfOut( x, y ) = y1;
        }
    }

    //
    //   XX --
    //      XX
    //

    for ( int32_t x = 1; x != maskWidth; x++ )
    {
        const float x1 = udfOut( x - 1, 0 ) + 1;
        if ( x1 < udfOut( x, 0 ) )
            udfOut( x, 0 ) = x1;
    }

    for ( int32_t y = maskHeight - 2; y >= 0; y-- )
    {
        {
            const float y1 = udfOut( 0, y + 1 ) + 1;
            if ( y1 < udfOut( 0, y ) )
                udfOut( 0, y ) = y1;
        }

        for ( int32_t x = 1; x != maskWidth; x++ )
        {
            const float x1 = udfOut( x - 1, y ) + 1;
            const float y1 = udfOut( x, y + 1 ) + 1;

            if ( x1 < udfOut( x, y ) )
                udfOut( x, y ) = x1;
            if ( y1 < udfOut( x, y ) )
                udfOut( x, y ) = y1;
        }
    }

#ifdef DEBUG_DEEP_MASK_PHASES
    do_snapshot( "sweep3" );
#endif // DEBUG_DEEP_MASK_PHASES
}


// ---------------------------------------------------------------------------------------------
// a multi-threaded, separable 2D blur filter for single channel float arrays
//
void BlurMono(
    MonoBufferF&            buffer,
    const float             blurSize )
{
    if ( blurSize <= 0.0f )
        return;

    const int32_t   bufWidth  = buffer.getWidth();
    const int32_t   bufHeight = buffer.getHeight();

    const float	    sizeRecp  = 1.0f / blurSize;
    const int32_t	sizeInt   = (int32_t)floor( blurSize );

    // blurLut holds 2 x [blurSize] cells; it defines the influence of pixels 
    // as we walk away from each side of a target pixel being blurred 
    //
    // <-- [ 0.1 ] [ 0.4 ] [ 0.9 ] [ 1.0 ] [ 0.9 ] [ 0.4 ] [ 0.1 ] --> influence decays in each direction
    //                               /\
    //          blurLutMid __________/
    //  ( the pixel being blurred )
    //
    float *blurLut = (float *)alloca( sizeof( float ) * ((2 * sizeInt + 1)) );

    // because the blur effect can hit the edges of the image, it clamps the blurSize so that 
    // it won't read off the edges; so that we correctly average the pixels that are read in these 
    // situations, we have a LUT that holds the sum of how many cells of blurLut are in use for
    // that size.
    //
    // eg. if you are blurring a pixel and have to clamp to '3' instead of the blurSize of '10', 
    //         you average with the value blurSum[3] rather than blurSum[10]
    //
    float *blurSum = (float *)alloca( sizeof( float ) * ( sizeInt + 1 ) );

    // get a pointer to the middle of the falloff lut, so it's natural to access it with -ve
    // indices without reading off the edge of memory
    float *blurLutMid = blurLut + sizeInt;


    // build a falloff table for the blur as described above
    for ( int32_t lut = -sizeInt; lut <= sizeInt; lut++ )
    {
        const float blurAmt = (float)lut * sizeRecp;
        blurLutMid[lut] = 1.0f - ( blurAmt * blurAmt );
    }

    for ( int32_t lut = 0; lut <= sizeInt; lut++ )
    {
        blurSum[lut] = 0;

        // sum up the blurLut values from -lut .. lut 
        // this gives us the total influence values that would be used for
        // this blur size
        for ( int32_t blr = -lut; blr <= lut; blr++ )
        {
            blurSum[lut] += blurLutMid[blr];
        }

        // store as reciprocal value so we can use a faster multiply to average values in the blur loop
        blurSum[lut] = 1.0f / blurSum[lut];
    }

    // we need a single scanline of working space when blurring each axis; to support running multiple threads, we allocate
    // a max-threads number of them and then access each one by thread-id during the parallel blur loops.
    // .. we also make an attempt to keep thrashing to a minimum by avoiding scanline scratch space crossing cache lines
    constexpr size_t cacheLineSize      = 64;
    constexpr size_t scanlineAlignment  = cacheLineSize / sizeof( float );
    const int32_t largestDimension      = ( std::max( bufWidth, bufHeight ) + scanlineAlignment) & ~( scanlineAlignment - 1 );
    const size_t  maxThreadsToPrep      = omp_get_max_threads();
    float *tempScan                     = allocateAlign<float>( largestDimension * maxThreadsToPrep, cacheLineSize );


    // --- --- --- --- BLUR IN X --- --- --- ---

    AUTO_PARALLELIZE_LOOP
    for ( int32_t y = 0; y < bufHeight; y++ )
    {
        // get thread ID for this block, choose the section of tempScan to use
        const size_t tid        = omp_get_thread_num();
        const size_t scanOffset = tid * largestDimension;   // accessing as float[] so don't * by sizeof(float)

        for ( int32_t x = 0; x < bufWidth; x++ )
        {
            float pixelSum = 0.0f;

            int32_t clippedSize = sizeInt;

            // clip the blur size to the edges of the image
            if ( x - clippedSize < 0 )
                clippedSize = x;
            if ( x + clippedSize >= bufWidth )
                clippedSize = bufWidth - x - 1;

            // run across the pixels inside the blur, sum up their value * the blur LUT
            for ( int32_t lut = -clippedSize; lut <= clippedSize; lut++ )
            {
                pixelSum += blurLutMid[lut] * buffer( x + lut, y );
            }

            // divide by the total influence to get the average and
            // stash the value; don't want to be editing the values for this scanline
            // while we're in the middle of blurring them
            tempScan[scanOffset + x] = pixelSum * blurSum[clippedSize];
        }

        // copy the working memory back across; could do this as a memcpy
        for ( int32_t x = 0; x != bufWidth; x++ )
        {
            buffer( x, y ) = tempScan[scanOffset + x];
        }
    }

    // --- --- --- --- BLUR IN Y --- --- --- ---

    AUTO_PARALLELIZE_LOOP
    for ( int32_t x = 0; x < bufWidth; x++ )
    {
        // get thread ID for this block, choose the section of tempScan to use
        const size_t tid        = omp_get_thread_num();
        const size_t scanOffset = tid * largestDimension;   // accessing as float[] so don't * by sizeof(float)

        for ( int32_t y = 0; y < bufHeight; y++ )
        {
            float pixelSum = 0.0f;

            int32_t clippedSize = sizeInt;

            // clip the blur size to the edges of the image
            if ( y - clippedSize < 0 )
                clippedSize = y;
            if ( y + clippedSize >= bufHeight )
                clippedSize = bufHeight - y - 1;

            // run across the pixels inside the blur, sum up their value * the blur LUT
            for ( int32_t lut = -clippedSize; lut <= clippedSize; lut++ )
            {
                pixelSum += blurLutMid[lut] * buffer( x, y + lut );
            }

            // divide by the total influence to get the average and
            // stash the value; don't want to be editing the values for this scanline
            // while we're in the middle of blurring them
            tempScan[scanOffset + y] = pixelSum * blurSum[clippedSize];
        }

        // copy the working memory back across; could do this as a memcpy
        for ( int32_t y = 0; y != bufHeight; y++ )
        {
            buffer( x, y ) = tempScan[scanOffset + y];
        }
    }

    freeAlign( tempScan );

    // optionally snapshot the post-blur UDF result 
#ifdef DEBUG_DEEP_MASK_PHASES

    char _snapPath[255];

    ColourFBuffer snapshot( bufWidth, bufHeight );
    snapshot.clear( ColourF::s_black );

    static const float hueScaleRecp = 3.0f / std::max( bufWidth, bufHeight );

    snapshot.forEach( [&]( const int32_t index, ColourF& c ) {
        if ( buffer[index] > 0.0f )
        {
            float hue = (float)buffer[index] * hueScaleRecp;
            c = ColourF::gradientPlasma( 1.0f - pingpong01( hue ) );
        }
    });

    sprintf( _snapPath, DEBUG_DEEP_MASK_RELDIR "blur/result.png" );
    writeOutBufferAsImage( snapshot, _snapPath, OutputFormat::AsPNG, true );

#endif // DEBUG_DEEP_MASK_PHASES
}


// ---------------------------------------------------------------------------------------------
// compute signed gradient field from the UDF; these values describe the +ve and -ve distances from
// the nearest processed mask input - these are then used when rendering flares, curving the effect
// around the given mask
void SGF(
    DistanceBuffer&     sgfOut,
    const MonoBufferF&  udfIn,
    const MaskProcessSettings& settings )
{
    const int32_t   bufWidth  = udfIn.getWidth();
    const int32_t   bufHeight = udfIn.getHeight();
    const float    widthDelta = 1.0f / bufWidth;
    const float   heightDelta = 1.0f / bufHeight;

    ensure( bufWidth  == sgfOut.getWidth()  );
    ensure( bufHeight == sgfOut.getHeight() );

    const bool applyTighten = ( settings.m_distanceTighten != 0.0f );


    FastNoise fn;

    if ( settings.m_distortionMode != MaskProcessSettings::DistortionMode::None )
    {
        fn.ConfigureWithSeed(settings.m_randomSeed);

        fn.SetFrequency(0.01f);
        fn.SetGradientPerturbAmp(settings.m_distortPerturb);
        fn.SetFractalType(FastNoise::Billow);
        fn.SetCellularDistanceFunction(FastNoise::Euclidean);

        switch (settings.m_distortionMode)
        {
            case MaskProcessSettings::DistortionMode::Celluar:
                fn.SetCellularReturnType(FastNoise::Distance);
                break;
            case MaskProcessSettings::DistortionMode::Lump:
                fn.SetCellularReturnType(FastNoise::Distance2Sub);
                break;

            default:
                break;
        }
    }


#if MASK_USE_SOBEL_FILTER

    // using Scharr coefficients
    const std::array<float, 6> sobelCoeff {{
            -3.0f, -10.0f, -3.0f, 
             3.0f,  10.0f,  3.0f 
        }};
    constexpr float sobelCoeffMagnitude = 32.0f;

#endif // MASK_USE_SOBEL_FILTER


    AUTO_PARALLELIZE_LOOP
    for ( int32_t y = 0; y < bufHeight; y++ )
    {
        float xF = -0.5f;
        float yF = xF + ( heightDelta * (float)y );

        for ( int32_t x = 0; x < bufWidth; x++ )
        {
            float dX = 0.0f;
            float dY = 0.0f;

            // border handling code 
            if ( x == 0 )
            {
                dX = udfIn( x + 1, y ) - udfIn( x, y );
                
                if ( y < bufHeight - 1 )
                    dY = udfIn( x, y + 1 ) - udfIn( x, y );
                else
                    dY = udfIn( x, y ) - udfIn( x, y - 1 );

#if MASK_USE_SOBEL_FILTER
                dX *= sobelCoeffMagnitude;
                dY *= sobelCoeffMagnitude;
#endif // MASK_USE_SOBEL_FILTER
            }
            else if ( y == 0 )
            {
                if ( x < bufWidth - 1 )
                    dX = udfIn( x + 1, y ) - udfIn( x, y );
                else
                    dX = udfIn( x, y ) - udfIn( x - 1, y );

                dY = udfIn( x, y + 1 ) - udfIn( x, y );

#if MASK_USE_SOBEL_FILTER
                dX *= sobelCoeffMagnitude;
                dY *= sobelCoeffMagnitude;
#endif // MASK_USE_SOBEL_FILTER
            }
            else if ( x == bufWidth - 1 )
            {
                dX = udfIn( x, y ) - udfIn( x - 1, y );
                
                if ( y < bufHeight - 1 )
                    dY = udfIn( x, y + 1 ) - udfIn( x, y );
                else
                    dY = udfIn( x, y ) - udfIn( x, y - 1 );

#if MASK_USE_SOBEL_FILTER
                dX *= sobelCoeffMagnitude;
                dY *= sobelCoeffMagnitude;
#endif // MASK_USE_SOBEL_FILTER
            }
            else if ( y == bufHeight - 1 )
            {
                if ( x < bufWidth - 1 )
                    dX = udfIn( x + 1, y ) - udfIn( x, y );
                else
                    dX = udfIn( x, y ) - udfIn( x - 1, y );

                dY = udfIn( x, y ) - udfIn( x, y - 1 );

#if MASK_USE_SOBEL_FILTER
                dX *= sobelCoeffMagnitude;
                dY *= sobelCoeffMagnitude;
#endif // MASK_USE_SOBEL_FILTER
            }
            // .. otherwise we are in the guts of the image so can do a full size filter
            else
            {
#if MASK_USE_SOBEL_FILTER
                {
                    #define UDF_OFFSET(ox, oy)   udfIn( x + ox, y + oy )

                    dX = UDF_OFFSET( -1, -1 ) * sobelCoeff[0] +                                       UDF_OFFSET( +1, -1 ) * sobelCoeff[3] +
                         UDF_OFFSET( -1,  0 ) * sobelCoeff[1] +                                       UDF_OFFSET( +1,  0 ) * sobelCoeff[4] +
                         UDF_OFFSET( -1, +1 ) * sobelCoeff[2] +                                       UDF_OFFSET( +1, +1 ) * sobelCoeff[5];


                    dY = UDF_OFFSET( -1, -1 ) * sobelCoeff[0] + UDF_OFFSET( 0, -1 ) * sobelCoeff[1] + UDF_OFFSET( +1, -1 ) * sobelCoeff[2] +

                         UDF_OFFSET( -1, +1 ) * sobelCoeff[3] + UDF_OFFSET( 0, +1 ) * sobelCoeff[4] + UDF_OFFSET( +1, +1 ) * sobelCoeff[5];

                    #undef UDF_OFFSET
                }
#else
                {
                    dX = udfIn( x + 1, y ) - udfIn( x, y );
                    dY = udfIn( x, y + 1 ) - udfIn( x, y );
                }
#endif // MASK_USE_SOBEL_FILTER
            }

            const float clampedIntensity = std::min( settings.m_distortIntensity, 0.99f );

            float change = ( dX * dX ) + ( dY * dY );
                  change = sqrtf( change ) * settings.m_distanceMultiplier;

            float dxF = xF     * settings.m_distortMult.x;
            float dyF = yF     * settings.m_distortMult.y;
            float dCF = 0;
            
            if ( settings.m_distortAltZAxis )
                dCF = udfIn(x, y) * settings.m_distortMult.z;
            else
                dCF = change * settings.m_distortMult.z;


            switch ( settings.m_distortionMode )
            {
                case MaskProcessSettings::DistortionMode::Noise:
                {
                    fn.GradientPerturbFractal( dxF, dyF, dCF );

                    const float distortion = fn.GetCubic( dxF, dyF, dCF );
                    const float adjusted   = std::max( ( settings.m_distortOffset * 2.0f ) + distortion, 0.0f );

                    change *= rgLerp( 1.0f, adjusted, clampedIntensity);
                }
                break;

                case MaskProcessSettings::DistortionMode::Celluar:
                case MaskProcessSettings::DistortionMode::Lump:
                {
                    dxF *= 0.5f;
                    dyF *= 0.5f;
                    dCF *= 0.5f;

                    fn.GradientPerturbFractal( dxF, dyF, dCF );

                    const float distortion = fn.GetCellular( dxF, dyF, dCF ) * 0.8f;
                    const float adjusted   = std::max( 0.5f + settings.m_distortOffset + distortion, 0.0f );

                    change *= rgLerp( 1.0f, adjusted, clampedIntensity);
                }
                break;

                default:
                    break;
            }

            if ( applyTighten )
                change = powf( change, settings.m_distanceTighten );

            if ( change == 0.0f )
            {
                sgfOut(x, y).x = 0.0f;
                sgfOut(x, y).y = 0.0f;
            }
            else
            {
                const float changeRecp = 1.0f / change;

                const float   inputUDF = udfIn( x, y );
                const float outputSGFx = inputUDF * ( dX * changeRecp );
                const float outputSGFy = inputUDF * ( dY * changeRecp );

                sgfOut( x, y ).x = outputSGFx;
                sgfOut( x, y ).y = outputSGFy;
            }

            xF += widthDelta;
        }
    }

    // optionally snapshot the SGF result 
#ifdef DEBUG_DEEP_MASK_PHASES
    char _snapPath[255];

    ColourFBuffer snapshot( bufWidth, bufHeight );
    snapshot.clear( ColourF::s_black );

    static const float vScaleRecp = 3.0f / (std::max(bufWidth, bufHeight) * 0.5f);

    snapshot.forEach( [&]( const int32_t index, ColourF& c ) {
        {
            const float vx     = sgfOut[index].x * vScaleRecp;
            const float abs_vx = std::abs( vx );

            const float vy     = sgfOut[index].y * vScaleRecp;
            const float abs_vy = std::abs( vy );

            c = ColourF::gradientPlasma( pingpong01( abs_vx ) );
            if ( vx < 0.0f )
                c = ColourF( c.b, c.g, c.r );

            ColourF c2 = ColourF::gradientPlasma( pingpong01( abs_vy ) );
            if ( vy < 0.0f )
                c2 = ColourF( c2.b, c2.g, c2.r );

            c.g = 0;
            c.b = c2.b;
        }
    });

    sprintf( _snapPath, DEBUG_DEEP_MASK_RELDIR "sgf/result_x.png" );
    writeOutBufferAsImage( snapshot, _snapPath, OutputFormat::AsPNG, true );

    snapshot.forEach( [&]( const int32_t index, ColourF& c ) {
        {
            const float vx = sgfOut[index].x * vScaleRecp;
            const float abs_vx = std::abs( vx );

            const float vy = sgfOut[index].y * vScaleRecp;
            const float abs_vy = std::abs( vy );

            c = ColourF::gradientViridis( std::max( abs_vx, abs_vy ) * 0.5f );
        }
    });

    sprintf( _snapPath, DEBUG_DEEP_MASK_RELDIR "sgf/result_y.png" );
    writeOutBufferAsImage( snapshot, _snapPath, OutputFormat::AsPNG, true );
    
#endif // DEBUG_DEEP_MASK_PHASES
}



// ---------------------------------------------------------------------------------------------
void DeepMask::preprocess(
    const ColourFBuffer&    imageInput,
    const float             threshold,
    const uint32_t          minimumElementSizeInPixels )
{
    ScopedTimer _t( "DM-pre" );

    m_valid = false;

    const int32_t maskWidth     = getWidth();
    const int32_t maskHeight    = getHeight();

    ensure( maskWidth  == imageInput.getWidth() );
    ensure( maskHeight == imageInput.getHeight() );

    BitBuffer fillWorkBuffer( maskWidth, maskHeight );


    // keep track of how many pixels we consider part of a mask
    m_maskedPixels = 0;

    // while we build the buffer we can sum up all masked pixels' locations and therefore
    // compute a centre point for all masked pixels
    m_maskedCentroid.x = m_maskedCentroid.y = 0.0;
    double centroidContribution = 0.0;

    for ( int32_t _y = 0; _y < maskHeight; _y++ )
    {
        for ( int32_t _x = 0; _x < maskWidth; _x++ )
        {
            const float pixelI = imageInput( _x, _y ).ComputeIntensity();

            // threshold the input to choose what's in or out
            if ( pixelI > threshold )
            {
                // this value indicates the pixel is part of a mask
                m_bitMask(_x, _y) = true;

                // .. we set this in the temporary buffer used to drive the FloodFill process tomorrow
                fillWorkBuffer( _x, _y ) = true;


                // sum up the centroid
                m_maskedCentroid.x += (double)_x;
                m_maskedCentroid.y += (double)_y;
                centroidContribution += 1.0;

                m_maskedPixels ++;
            }
        }
    }

    if ( centroidContribution > 0.0f )
    {
        m_maskedCentroid /= centroidContribution;
    }


    uint32_t maskElementCurrent = 1;
    uint32_t remaskedPixels     = 0;
    uint32_t totalPasses        = 0;
    uint32_t totalElements      = 0;

    m_elementData.clear();

    for ( int32_t _y = 0; _y < maskHeight; _y++ )
    {
        if ( maskElementCurrent == 255 )
            break;

        for ( int32_t _x = 0; _x < maskWidth; _x++ )
        {
            if ( fillWorkBuffer( _x, _y ) )
            {
                MaskElement elem = FloodFill( fillWorkBuffer, m_elementBuffer, maskElementCurrent, _x, _y );

                // can choose a minimum pixel count for a filled element - to optionally ignore tiny fragments
                if ( elem.m_numPixels >= minimumElementSizeInPixels )
                {
                    m_elementData.emplace_back(elem);
                    maskElementCurrent++;
                }

                remaskedPixels += elem.m_numPixels;
                totalPasses    += elem.m_numPasses;

                if ( maskElementCurrent == 255 )
                    break;
            }
        }
    }

    totalElements = (uint32_t)m_elementData.size();

    // ensure that we cut out all the masked pixels as we were filling
    if ( remaskedPixels != m_maskedPixels )
    {
        // ran out of elements! probably a complex mask
        rgLogOutput( LogLevel::Default, "[%s]\nRan out of element IDs - mask too complex?", __FUNCTION__ );
    }

    rgLogOutput( LogLevel::Verbose, "{DeepMask} mask = %u of %i pixels", m_maskedPixels, maskWidth * maskHeight );
    rgLogOutput( LogLevel::Verbose, "{DeepMask} total elements = %u", totalElements );
}


// ---------------------------------------------------------------------------------------------
void DeepMask::postprocess(
    const MaskProcessSettings&  settings )
{
    {
        ScopedTimer _t( "DM-UDF" );
        UDF(
            m_udf,
            m_bitMask,
            settings.m_isolateElement >= 0 ? &m_elementBuffer : nullptr,
            settings.m_isolateElement );
    }
    {
        ScopedTimer _t( "DM-Blur" );
        BlurMono(
            m_udf,
            settings.m_blurSize );
    }
    {
        ScopedTimer _t( "TM-SGF" );
        SGF(
            m_sgf,
            m_udf,
            settings );
    }

    m_valid = true;
}