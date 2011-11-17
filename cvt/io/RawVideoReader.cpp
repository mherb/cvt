#include <cvt/io/RawVideoReader.h>
#include <cvt/util/Exception.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

namespace cvt
{
	RawVideoReader::RawVideoReader( const String & filename, bool autoRewind ):
		_fd( -1 ),
		_format( IFormat::RGBA_UINT8 ),
		_autoRewind( autoRewind ),
		_map( 0 ),
		_mappedSize( 0 ),
		_ptr( 0 )
	{
		// create the file (open, truncate to header size)
		_fd = open( filename.c_str(), O_RDWR , 0 );
		if( _fd < 0 ){
			char * err = strerror( errno );
			String msg( "Could not open file: " );
			msg += err;
			throw CVTException( msg.c_str() );
		}

		_pageSize = sysconf( _SC_PAGE_SIZE );
	
		struct stat fileInfo;
		if( fstat( _fd, &fileInfo ) == -1 ){
			char * err = strerror( errno );
			String msg( "fstat error: " );
			msg += err;
			throw CVTException( msg.c_str() );
		}	

		_mappedSize = fileInfo.st_size;
		_map = mmap( 0, _mappedSize, PROT_READ, MAP_PRIVATE, _fd, 0 );
		if( _map == MAP_FAILED ){
			char * err = strerror( errno );
			String msg( "Could not map file: " );
			msg += err;
			_map = 0;
			throw CVTException( msg.c_str() );
		}
		_ptr = ( uint8_t* )_map;

		readHeader();	
	}

	RawVideoReader::~RawVideoReader()
	{
		if( _fd != -1 ){
			if( _map != 0 ){
				if( munmap( _map, _mappedSize ) != 0 ){
					char * err = strerror( errno );
					String msg( "Could not unmap memory: " );
					msg += err;
					throw CVTException( msg.c_str() );
				}
			}

			if( close( _fd ) < 0 ){
				char * err = strerror( errno );
				String msg( "Could not close file: " );
				msg += err;
				throw CVTException( msg.c_str() );
			}
		}
	}

	void RawVideoReader::readHeader()
	{
		_width = *( ( uint32_t* ) _ptr );
		_ptr += sizeof( uint32_t );
		_height = *( ( uint32_t* ) _ptr );
		_ptr += sizeof( uint32_t );
		_stride = *( ( uint32_t* ) _ptr );
		_ptr += sizeof( uint32_t );
		_format = IFormat::formatForId( (IFormatID ) *( ( uint32_t* ) _ptr ) );
		_ptr += sizeof( uint32_t );

		uint32_t dataSize = _mappedSize - ( 4 * sizeof( uint32_t ) );
		uint32_t frameSize = _stride * _height;

		_numFrames = dataSize / frameSize;
		_currentFrame = 0;

		_frame.reallocate( _width, _height, _format );
	}


	void RawVideoReader::nextFrame()
	{
		if( _currentFrame < _numFrames ){
			size_t istride;
			uint8_t* iptr = _frame.map<uint8_t>( &istride );

			SIMD* simd = SIMD::instance();
			if( istride == _stride ){
				size_t num = _height * _stride;
				simd->Memcpy( iptr, _ptr, num );
				_ptr += num;
			} else {
				size_t h = _height;
				uint8_t * p = iptr;
				size_t bytesPerRow = _width * _frame.bpp();
				while( h-- ){
					simd->Memcpy( p, _ptr, bytesPerRow );
					_ptr += _stride;
					p += istride;
				}
			}

			_frame.unmap( iptr );

		}
	}
}

