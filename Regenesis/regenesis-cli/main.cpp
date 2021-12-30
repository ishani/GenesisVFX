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
#include "rgBuffer2D.h"
#include "rgDeepMask.h"
#include "rgRender.h"
#include "rgMath.h"

// ------------------------------------------------------------------------------------------------

#include "console/CLI11.hpp"
#include "console/rang.hpp"

// ------------------------------------------------------------------------------------------------

#include <stdarg.h>
#include <iostream>


// command line options
static std::string cli_bgImage;
static std::string cli_maskImage;
static std::string cli_outImage;
static std::string cli_gfxFile;
static std::vector< float > cli_center;
static std::vector< float > cli_recolour;
static bool        cli_verbose = false;


void rgLogOutput( const LogLevel ll, const char* fmt, ... )
{
    if ( ll == LogLevel::Verbose && !cli_verbose )
        return;

    va_list args;
    char buf[1000];
    va_start( args, fmt );
    vsnprintf( buf, sizeof( buf ), fmt, args );
    va_end( args );

    if ( ll != LogLevel::Default )
        std::cout << rgLogLevelText( ll );

    std::cout << buf << std::endl;
}

void rgAssertHandler( const char* func, const uint64_t line, const char* failed_expr )
{
    printf( "[Assert Failed]\n%s (%" PRIu64 ") - %s\n\n", func, line, failed_expr );
#ifdef WIN32
    __debugbreak();
#else
    __builtin_trap();
#endif
    exit( -1 );
}

void rgProgressHandler( const char* task, const float current, const float total )
{
    printf( "> %-50s  .. %.1f%%\n", task, (current / total) * 100.0f );
}

// ------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    ColourF foo(0.8f, 0.5f, 0.2f);
    ColourHSV hsv(foo);
    hsv.s = 1.0f;
    ColourF out(hsv);


    CLI::App app( "CLI GenesisVFX Renderer | Harry Denholm, ishani.org" );

    CLI::Option* opt_infile    = app.add_option( "-i,--in",         cli_bgImage,    "background image"                            )->required( true )->check( CLI::ExistingFile );
    CLI::Option* opt_maskfile  = app.add_option( "-m,--mask",       cli_maskImage,  "mask image"                                  )->check( CLI::ExistingFile );
    CLI::Option* opt_gfx       = app.add_option( "-g,--gfx",        cli_gfxFile,    "gfx flare file"                              )->required( true )->check( CLI::ExistingFile );
    CLI::Option* opt_outfile   = app.add_option( "-o,--out",        cli_outImage,   "output image"                                )->required( true );
    CLI::Option* opt_center    = app.add_option( "-c,--center",     cli_center,     "center coordinate pair [x,y] (image coords)" )->expected( 2 );
    CLI::Option* opt_recolour  = app.add_option( "-r,--recolour",   cli_recolour,   "colour multiplier [r, g, b] (0..1         )" )->expected( 3 );
    CLI::Option* opt_verbose   = app.add_flag(   "-v,--verbose",    cli_verbose,    "verbose logging"                             );

    CLI11_PARSE( app, argc, argv );

    printf(OPT_VERSION "\n");

    regenesis::init();


    GfxFile gfx;
    GfxFile::LoadResult lres = gfx.LoadFile( cli_gfxFile.c_str() );

    if ( lres != GfxFile::LoadResult::Ok )
    {
        printf( "Failed to load GFX" );
        return 1;
    }

    auto backgroundImage = bufferFromImage< ColourF >( cli_bgImage.c_str() );
    if ( backgroundImage == nullptr )
    {
        printf( "Failed to load checker input" );
        return 1;
    }

    DeepMask* deepMask = nullptr;
    ColourFBuffer* maskBuffer = nullptr;
    ColourFBuffer* maskOrigin = nullptr;

    if ( *opt_maskfile )
    {
        deepMask = new DeepMask(
            backgroundImage->getWidth(),
            backgroundImage->getHeight());

        auto maskBufferImage = bufferFromImage< ColourF >( cli_maskImage.c_str() );
        if ( maskBufferImage == nullptr )
        {
            printf( "Failed to load mask input" );
            return 1;
        }

        if ( maskBufferImage->getWidth()  == backgroundImage->getWidth() &&
             maskBufferImage->getHeight() == backgroundImage->getHeight() )
        {
            deepMask->preprocess( *maskBufferImage, 0.5f, 1 );

            maskBuffer = new ColourFBuffer( backgroundImage->getWidth(), backgroundImage->getHeight() );
            maskBuffer->bufferCopyFrom( *maskBufferImage );
        }
        else
        {
            printf("Resampling mask input to fit...\n");

            ColourFBuffer resizedMask( backgroundImage->getWidth(), backgroundImage->getHeight() );
            resizedMask.resampleFrom( *maskBufferImage );

            deepMask->preprocess( resizedMask, 0.5f, 1 );

            maskBuffer = new ColourFBuffer( backgroundImage->getWidth(), backgroundImage->getHeight() );
            maskBuffer->bufferCopyFrom(resizedMask);
        }

        maskOrigin = new ColourFBuffer( backgroundImage->getWidth(), backgroundImage->getHeight() );
        maskOrigin->bufferCopyFrom(*backgroundImage);
    }


    RenderOutput renderOut;

    if ( opt_recolour->count() )
    {
        renderOut.m_colourise.r = cli_recolour[0];
        renderOut.m_colourise.g = cli_recolour[1];
        renderOut.m_colourise.b = cli_recolour[2];
    }

    MaskProcessSettings maskSettings;
    maskSettings.m_distortionMode = MaskProcessSettings::DistortionMode::None;

    RenderTweaks tweaks;
    tweaks.masking.m_renderMaskElementsSeparately = false;
    tweaks.masking.m_perElementRandomisationTime = 5.0f;
    tweaks.masking.m_perElementRandomisationRotation = 320.0f;

    tweaks.m_protectAlpha = true;

    RenderGlobals globals;
    globals.m_scale = 0.2f;
 
    if ( opt_center->count() )
    {
        globals.m_center.x = cli_center[0];
        globals.m_center.y = cli_center[1];
    }

    regenesis::render(
        gfx,
        globals,
        *backgroundImage,
        tweaks,
        renderOut,
        maskSettings,
        deepMask );

    if ( maskBuffer != nullptr )
    {
        AUTO_PARALLELIZE_LOOP
        for ( int32_t y = 0; y < backgroundImage->getHeight(); y++ )
        {
            for ( int32_t x = 0; x < backgroundImage->getWidth(); x++ )
            {
                const float maskValue = saturate( (*maskBuffer)(x, y).ComputeIntensity() );
                if ( maskValue > 0.0f )
                {
                    (*renderOut.m_result)(x, y).LerpTo( (*maskOrigin)(x, y), maskValue );
                }
            }
        }
    }


    writeOutBufferAsImage( *renderOut.m_result, cli_outImage.c_str(), OutputFormat::AsPNG );

    delete deepMask;

    return 0;
}

