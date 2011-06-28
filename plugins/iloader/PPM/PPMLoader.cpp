#include <cvt/util/PluginManager.h>
#include <cvt/gfx/Image.h>
#include <cvt/util/Exception.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace cvt {
	class PPMLoader : public ILoader
	{
		public:
			PPMLoader() {}
			void load( Image& dst, const String& file );
			const String& extension( size_t n ) const { return _extension[ n ]; }
			size_t sizeExtensions() const { return 4; }
			const String& name() const { return _name; }

		private:
            enum PGMType
            {
                P2_GRAY_ASCII,
                P3_RGB_ASCII,
                P5_GRAY_BINARY,
                P6_RGB_BINARY
            };

			static String _name;
			static String _extension[ 4 ];

            size_t tokenizeLine( const uint8_t * data, size_t maxSize, std::vector<String> & tokens );
            size_t skipLine( const uint8_t *& data );
            PGMType parseMagicNumber( const std::vector<String> & tokens );

            void parseBinary( Image & img, const uint8_t * data );
            void parseASCII( Image & img, const uint8_t * data, size_t n );
	};

	String PPMLoader::_name = "PPM";
	String PPMLoader::_extension[] = { ".ppm", ".PPM", ".pgm", ".PGM" };

    size_t PPMLoader::tokenizeLine( const uint8_t* data, size_t numData, std::vector<String> & tokens )
    {
        // skip whitespace, \t and \n
        const uint8_t * start = data;
        size_t bytesConsumed = 0;

        for( ;; ){
            // comments:
            if( *data == '#' ) {
                bytesConsumed += skipLine( data );
                return bytesConsumed;
            }

            // skip
            while( *data != ' '  &&
                   *data != '\t' &&
                   *data != '\r' &&
                   *data != '\n'){
                data++;
                bytesConsumed++;
				if( bytesConsumed > numData )
					throw CVTException( "PPM header corrupt!" );
            }

            // token runs from start to data - 1
            tokens.push_back( String( (const char*)start, (data - start) ) );
            if( *data == '\n' ){
                data++;
                bytesConsumed++;
                return bytesConsumed;
            } else {
                data++;
                bytesConsumed++;
				if( bytesConsumed > numData )
					throw CVTException( "PPM header corrupt!" );
            }
            start = data;
        }
        return bytesConsumed;
    }

    size_t PPMLoader::skipLine( const uint8_t* & data )
    {
        size_t n = 0;
        while ( *data != '\n' ) {
            data++;
            n++;
        }
        data++;
		n++;
		return n;
    }

	void PPMLoader::load( Image& img, const String& path )
	{
		int fp = open( path.c_str(), O_RDONLY );
		if( fp < 0 ){
			String message( "Cannot open PPM image file: " );
			message += path;
			throw CVTException( message.c_str() );
		}

        struct stat fileStats;
        if( fstat( fp, &fileStats ) == -1 ){
            throw CVTException( "Cannot open PPM image file -> Could not determine file stats" );
        }
		int size = fileStats.st_size;

        uint8_t * data = ( uint8_t* )mmap( 0, size, PROT_READ, MAP_PRIVATE, fp, 0 );
		uint8_t * ptrSave = data;
		int sizeSave = size;

		if( data == MAP_FAILED ){
			throw CVTException( "Could not map file" );
		}

        std::vector<String> tokens;

        size_t n;

        // get next full line as tokens, skipping comments
        while( tokens.size() == 0 ){
            n = tokenizeLine( data, size, tokens );
            data+=n;
            size-=n;
			if( size < 0 )
				throw CVTException( "PPM Header corrupt!" );
        }

        PGMType type;
        if( tokens.size() != 1 ){
            throw CVTException( "Expected Magic Number" );
        } else {
            if( tokens[ 0 ] == "P2") type = P2_GRAY_ASCII;
            if( tokens[ 0 ] == "P3") type = P3_RGB_ASCII;
            if( tokens[ 0 ] == "P5") type = P5_GRAY_BINARY;
            if( tokens[ 0 ] == "P6") type = P6_RGB_BINARY;
        }
        tokens.clear();

        // get next full line as tokens, skipping comments
        while( tokens.size() == 0 ){
            n = tokenizeLine( data, size, tokens );
            data+=n;
            size-=n;
			if( size < 0 )
				throw CVTException( "PPM Header corrupt!" );
        }

        // parse width and height
        size_t width, height;
        if( tokens.size() != 2 ){
            throw CVTException("PPM File Error: expected width and height");
        } else {
            width = tokens[ 0 ].toInteger();
            height = tokens[ 1 ].toInteger();
        }

        // get next full line as tokens, skipping comments
        tokens.clear();
        while( tokens.size() == 0 ){
            n = tokenizeLine( data, size, tokens );
            data+=n;
            size-=n;
			if( size < 0 )
				throw CVTException( "PPM Header corrupt!" );
        }

        size_t colorRange;
        if( tokens.size() == 1 ){
            colorRange = tokens[ 0 ].toInteger();
        } else {
            throw CVTException("Expected CHANNEL depth");
        }

        switch ( type ) {
            case P2_GRAY_ASCII:
                if( colorRange > 255 )
                    img.reallocate( width, height, IFormat::GRAY_UINT16 );
                else
                    img.reallocate( width, height, IFormat::GRAY_UINT8 );
                parseASCII( img, data, size );
                break;
            case P3_RGB_ASCII:
                if( colorRange > 255 )
                    img.reallocate( width, height, IFormat::RGBA_UINT16 );
                else
                    img.reallocate( width, height, IFormat::RGBA_UINT8 );
                parseASCII( img, data, size );
                break;
            case P5_GRAY_BINARY:
                if( colorRange > 255 )
                    img.reallocate( width, height, IFormat::GRAY_UINT16 );
                else
                    img.reallocate( width, height, IFormat::GRAY_UINT8 );
                parseBinary( img, data );
                break;
            case P6_RGB_BINARY:
                if( colorRange > 255 )
                    img.reallocate( width, height, IFormat::RGBA_UINT16 );
                else
                    img.reallocate( width, height, IFormat::RGBA_UINT8 );
                parseBinary( img, data );
                break;
        }

		// unmap
		munmap( ptrSave, sizeSave );

		// close file
		close( fp );
	}

    void PPMLoader::parseBinary( Image & img, const uint8_t * data )
    {
        size_t stride;
        uint8_t * iptr = img.map( &stride );
        uint8_t * ptr = iptr;

        size_t height = img.height();

        size_t n;
        switch ( img.format().formatID ) {
            case IFORMAT_RGBA_UINT8:
            case IFORMAT_RGBA_UINT16:
            case IFORMAT_RGBA_FLOAT:
                n = img.width() * 3;
                break;
            case IFORMAT_GRAY_UINT8:
            case IFORMAT_GRAY_UINT16:
            case IFORMAT_GRAY_FLOAT:
                n = img.width();
                break;
            default:
                throw CVTException( "Unsupported image format for PPM" );
                break;
        }

        SIMD* simd = SIMD::instance();

        switch ( img.format().type ) {
            case IFORMAT_TYPE_FLOAT:
                while ( height-- ) {
                    simd->Conv_XXXf_to_XXXAf( ( float* )ptr, ( const float* )data, n );
                    ptr += stride;
                    data += n * sizeof( float );
                }
                break;
            case IFORMAT_TYPE_UINT8:
                while ( height-- ) {
                    simd->Conv_XXXu8_to_XXXAu8( ptr, data, n );
                    ptr += stride;
                    data += n;
                }
                break;
            default:
                throw CVTException( "FORMAT NOT SUPPORTED" );
                break;
        }
        img.unmap( iptr );
    }

    void PPMLoader::parseASCII( Image & img, const uint8_t * data, size_t n )
    {
        size_t stride;
        uint8_t * iptr = img.map( &stride );
        uint8_t * ptr = iptr;

		size_t h = img.height();

		int remainingData = n;

		uint8_t * end;
        switch ( img.format().type ) {
            case IFORMAT_TYPE_UINT8:
                while ( h-- ){
					for( size_t w = 0; w < img.width(); w++ ){
						if( img.channels() == 4 ){
							for( int i = 0; i < 3; i++ ){
								long v = strtol( (const char*)data, (char**)&end, 10 );
								remainingData -= ( end - data );
								if( remainingData < 0 ){
									throw CVTException( "Corrupt PPM File" );
								}
								while( *end == '\n'||
									   *end == ' ' ||
								       *end == '\r'||
								       *end == '\t' ){
									end++;
									remainingData--;
									if( remainingData < 0 ){
										throw CVTException( "Corrupt PPM File" );
									}
								}
								ptr[ w * 4 + i ] = ( uint8_t )v;
								data = end;
							}
							ptr[ w * 4 + 3 ] = 255;
						} else {
							long v = strtol( (const char*)data, (char**)&end, 10 );
							remainingData -= ( end - data );
							if( remainingData < 0 ){
								throw CVTException( "Corrupt PPM File" );
							}
							while( *end == '\n' ||
								   *end == '\r'||
							       *end == ' '  ||
							       *end == '\t' ){
								end++;
								remainingData--;
								if( remainingData < 0 ){
									throw CVTException( "Corrupt PPM File" );
								}
							}
							ptr[ w ] = ( uint8_t )v;
							data = end;
						}
					}
                    ptr += stride;
                }
                break;
            default:
                throw CVTException( "Format not supported in ASCII" );
                break;
        }

        img.unmap( iptr );
    }

}

static void _init( cvt::PluginManager* pm )
{
	cvt::ILoader* ppm = new cvt::PPMLoader();
	pm->registerPlugin( ppm );
}

CVT_PLUGIN( _init )
