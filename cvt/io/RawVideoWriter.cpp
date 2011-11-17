#include <cvt/io/RawVideoWriter.h>
#include <cvt/util/Exception.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

namespace cvt
{
	RawVideoWriter::RawVideoWriter( const String & filename ):
		_fd( -1 ),
		_currSize( 0 ),
		_maxSize( 0 ),
		_map( 0 ),
		_mappedSize( 0 ),
		_pos( 0 ),
		_width( 0 ),
		_height( 0 ),
		_stride( 0 ),
		_formatID( 0 )
	{
		// create the file (open, truncate to header size)
		_fd = open( filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG );
		if( _fd < 0 ){
			char * err = strerror( errno );
			String msg( "Could not open file: " );
			msg += err;
			throw CVTException( msg.c_str() );
		}

		_currSize = 4 * sizeof( size_t );
		_maxSize = _currSize;
		resizeFile();

		// get the system page size
		_pageSize = sysconf( _SC_PAGE_SIZE );
	}

	RawVideoWriter::~RawVideoWriter()
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

			// truncate to current size 
			if( ftruncate( _fd, _currSize ) < 0){
				char * err = strerror( errno );
				String msg( "Could not resize file: " );
				msg += err;
				throw CVTException( msg.c_str() );
			}

			if( close( _fd ) < 0 ){
				char * err = strerror( errno );
				String msg( "Could not close file: " );
				msg += err;
				throw CVTException( msg.c_str() );
			}
		}

	}

	void RawVideoWriter::write( const Image & img )
	{
		size_t iStride;
		const uint8_t * imgData = img.map<uint8_t>( &iStride );
		if( _width == 0 ){
			_width = img.width();
			_height = img.height();
			_stride = iStride;
			_formatID = (size_t)img.format().formatID;
			_imgSize = _height * _stride;

			// need to write the header first
			remapFile();
			writeHeader();
		} else {
			// check size and format
			if( _width != img.width() ||
				_height != img.height() ||
				_stride != iStride ||
				_formatID != ( size_t )img.format().formatID ){
				throw CVTException( "Trying to mix different image resolution or format in a single video!");
			}
		}

		// check if we need to remap
		if( ( _currSize + _imgSize ) > _maxSize ){
			remapFile();
		}

		SIMD * simd = SIMD::instance();
		simd->Memcpy( _pos, imgData, _imgSize );
		img.unmap( imgData );

		_pos += _imgSize;
		_currSize += _imgSize;
	}

	void RawVideoWriter::remapFile()
	{
		if( _currSize == _maxSize ){
			if( _map != 0 ){
				
				if( msync( _map, _mappedSize, MS_ASYNC ) == -1 ){
					char * err = strerror( errno );
					String msg( "Could not msync: " );
					msg += err;
					throw CVTException( msg.c_str() );
				}

				if( munmap( _map, _mappedSize ) != 0 ){
					char * err = strerror( errno );
					String msg( "Could not unmap memory: " );
					msg += err;
					_map = 0;
					_mappedSize = 0;
					throw CVTException( msg.c_str() );
				}
				_map = 0;
				_mappedSize = 0;
			}

			_maxSize += ( 10 * _imgSize );
			resizeFile();
		}

		// file has maxSize 
		size_t mapSize;
		size_t offset;
		size_t ptrOffset;
		if( _pos == 0 ){
			// we are mapping the first time
			mapSize = _maxSize;
			offset = 0;
			ptrOffset = 0;
		} else {
			ldiv_t palignment = ldiv( _currSize, _pageSize );
			offset = _pageSize * palignment.quot;
			mapSize = _maxSize - offset;
			ptrOffset = _currSize - offset;
		}

		_map = mmap( 0, mapSize, PROT_WRITE, MAP_SHARED, _fd, offset );
		if( _map == MAP_FAILED ){
			char * err = strerror( errno );
			String msg( "Could not map file: " );
			msg += err;
			_map = 0;
			throw CVTException( msg.c_str() );
		} 
			
		// map successful
		_pos = ( uint8_t* )_map;
		_pos += ptrOffset;
		_mappedSize = mapSize;
		
		if( madvise( _map, _mappedSize, MADV_SEQUENTIAL ) != 0 ){
			char * err = strerror( errno );
			String msg( "Memadvise failed: " );
			msg += err;
			throw CVTException( msg.c_str() );
		}
	}

	void RawVideoWriter::writeHeader()
	{
		*( ( uint32_t* )_pos ) = _width; _pos += sizeof( uint32_t );
		*( ( uint32_t* )_pos ) = _height; _pos += sizeof( uint32_t );
		*( ( uint32_t* )_pos ) = _stride; _pos += sizeof( uint32_t );
		*( ( uint32_t* )_pos ) = _formatID; _pos += sizeof( uint32_t );
	}

	void RawVideoWriter::resizeFile()
	{
		int res = lseek( _fd, _maxSize - 1, SEEK_SET );
		if( res == -1 ){
			char * err = strerror( errno );
			String msg( "File resize failed: " );
			msg += err;
			throw CVTException( msg.c_str() );
		}

		res = ::write( _fd, "", 1 );
		if( res == -1 ){
			char * err = strerror( errno );
			String msg( "File resize failed: " );
			msg += err;
			throw CVTException( msg.c_str() );
		}
	}
}

