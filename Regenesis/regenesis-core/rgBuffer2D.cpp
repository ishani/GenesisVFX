/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#include "rgCommon.h"
#include "rgBuffer2D.h"

// ------
#include "stb_image.h"
#include "stb_image_write.h"
// ------


namespace _internal
{
    bool image_file_encode_png( const std::string& filename, const std::vector<uint8_t>& image, int32_t w, int32_t h )
    {
        int32_t writeError = stbi_write_png( filename.c_str(), w, h, 4, image.data(), sizeof( uint32_t ) * w );

        if ( !writeError )
        {
            return false;
        }
        return true;
    }

    bool image_file_encode_jpeg( const std::string& filename, const std::vector<uint8_t>& image, int32_t w, int32_t h )
    {
        int32_t writeError = stbi_write_jpg( filename.c_str(), w, h, 4, image.data(), 99 );

        if ( !writeError )
        {
            return false;
        }
        return true;
    }

    // ------------------------------------------------------------------------------------------------
    bool image_file_decode( std::vector<uint8_t>& image, int32_t& w, int32_t& h, const std::string& filename )
    {
        int32_t width, height, components;
        uint8_t* imageData = stbi_load( filename.c_str(), &width, &height, &components, 0 );

        if ( imageData == nullptr )
            return false;

        w = width;
        h = height;

        if ( components == 3 )
        {
            // manually fill in missing alpha component
            image.resize( width * height * 4 );

            AUTO_PARALLELIZE_LOOP
            for ( int32_t y = 0; y < height; y++ )
            {
                for ( int32_t x = 0; x < width; x++ )
                {
                    const int32_t inputIndex  = ( y * width * 3 ) + ( x * 3 );
                    const int32_t outputIndex = ( y * width * 4 ) + ( x * 4 );

                    image[outputIndex + 0] = imageData[inputIndex + 0];
                    image[outputIndex + 1] = imageData[inputIndex + 1];
                    image[outputIndex + 2] = imageData[inputIndex + 2];
                    image[outputIndex + 3] = 255;
                }
            }
            return true;
        }
        else if ( components == 4 )
        {
            image.assign( imageData, imageData + ( width * height * components * sizeof( uint8_t ) ) );
            return true;
        }

        rgLogOutput( LogLevel::Error, "[%s] failed to load, expected 3 or 4 components but got %i\n", filename.c_str(), components );
        return false;
    }

    // ------------------------------------------------------------------------------------------------
    const char* image_error_text()
    {
        return stbi_failure_reason();
    }

} // namespace _internal


