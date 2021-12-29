/*
     __   ___  __   ___       ___  __     __
    |__) |__  / _` |__  |\ | |__  /__` | /__`
    |  \ |___ \__> |___ | \| |___ .__/ | .__/

        Remix of GenesisVFX by Harry Denholm, ishani.org 2018-2021
            Original code by Andrew Cross
                Photoshop version by Richard Wilson

*/

#include "rgCommon.h"
#include "rgIO.h"

// ------------------------------------------------------------------------------------------------
GfxFile::LoadResult GfxFile::LoadMemory( const std::vector<uint8_t>& memory_stream )
{
    memcpy( &header, memory_stream.data(), sizeof(header) );

    // saved from GenesisVFX 2.x, we can't load this yet because the format changed and expanded
    // .. and we don't have the code for that
    if ( ( strncmp( header.m_version, "GFX110", 6 ) == 0 ) )
    {
        return LoadResult::Genesis110NotSupported;
    }

    // missing the magic code? file probably not a workable gfx
    if ( ( strncmp( header.m_version, "GFX100", 6 ) != 0 ) &&
         ( strncmp( header.m_version, "GFX101", 6 ) != 0 ) )
    {
        return LoadResult::InvalidHeader;
    }

    size_t copyCursor = sizeof(header);
    #define COPY_OUT( _dst, _amt )         memcpy( &_dst, memory_stream.data() + copyCursor, _amt ); copyCursor += _amt;


    flares.resize( header.m_numFlares );

    for ( FlareBlob& blob : flares )
    {
        COPY_OUT( blob.m_header, sizeof( gfxdata::FlareHeader ) );

        int32_t num_pos_keys;
        COPY_OUT( num_pos_keys, sizeof( num_pos_keys ) );

        blob.m_pos_keys.resize( num_pos_keys );
        for ( gfxdata::FlarePosAnim& fpa : blob.m_pos_keys )
        {
            COPY_OUT( fpa, sizeof( fpa ) );
        }

        int32_t num_shape_keys;
        COPY_OUT( num_shape_keys, sizeof( num_shape_keys ) );

        blob.m_shape_keys.resize( num_shape_keys );
        for ( gfxdata::FlareShpAnim& fsa : blob.m_shape_keys )
        {
            COPY_OUT( fsa, sizeof( fsa ) );
        }

        COPY_OUT( blob.m_inferno, sizeof( gfxdata::Fractal ) );
        COPY_OUT( blob.m_build, sizeof( gfxdata::FlareBuild ) );
    }

    return LoadResult::Ok;
}

// ------------------------------------------------------------------------------------------------
GfxFile::LoadResult GfxFile::LoadFile( const char* input_file )
{
    FILE* fileIn = nullptr;
#ifdef MSC_VER
    fopen_s(&fileIn, input_file, "rbS");
#else
    fileIn = fopen(input_file, "rb");
#endif 

    if ( fileIn == nullptr )
        return LoadResult::FileNotFound;

    fseek( fileIn, 0, SEEK_END );
    auto file_length = ftell( fileIn );
    fseek( fileIn, 0, SEEK_SET );

    if ( file_length < sizeof(header) )
        return LoadResult::InvalidHeader;

    std::vector<uint8_t> fileBuffer;
    fileBuffer.resize( file_length );
    fread( fileBuffer.data(), file_length, 1, fileIn );
    fclose( fileIn );

    return LoadMemory( fileBuffer );
}
