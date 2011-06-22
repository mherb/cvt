#include <cvt/gfx/Image.h>
#include <cvt/gfx/ImageAllocatorMem.h>
#include <cvt/gfx/ImageAllocatorCL.h>
#include <cvt/gfx/ImageAllocatorGL.h>
#include <cvt/math/Math.h>
#include <cvt/util/SIMD.h>
#include <cvt/util/Exception.h>
#include <cvt/util/PluginManager.h>

#include <fstream>

namespace cvt {

	Image::Image( size_t w, size_t h, const IFormat & format, IAllocatorType memtype )
	{
		if( memtype == IALLOCATOR_CL )
			_mem = new ImageAllocatorCL();
		else if( memtype == IALLOCATOR_GL )
			_mem = new ImageAllocatorGL();
		else
			_mem = new ImageAllocatorMem();
	    _mem->alloc( w, h, format );
	}


	Image::Image( size_t w, size_t h, const IFormat & format, uint8_t* data, size_t stride )
	{
		_mem = new ImageAllocatorMem();
		ImageAllocatorMem * memAllocator = (ImageAllocatorMem *)_mem;
		memAllocator->alloc( w, h, format, data, stride );
	}


	Image::Image( const Image& img, IAllocatorType memtype )
	{
		if( memtype == IALLOCATOR_CL )
			_mem = new ImageAllocatorCL();
		else if( memtype == IALLOCATOR_GL )
			_mem = new ImageAllocatorGL();
		else
			_mem = new ImageAllocatorMem();
		_mem->copy( img._mem );
	}

	Image::Image( const std::string & fileName, IAllocatorType memtype )
	{
		if( memtype == IALLOCATOR_CL )
			_mem = new ImageAllocatorCL();
		else if( memtype == IALLOCATOR_GL )
			_mem = new ImageAllocatorGL();
		else
			_mem = new ImageAllocatorMem();
		this->load( fileName.c_str() );
	}

	Image::Image( const Image& source, const Recti* roi, bool ref, IAllocatorType memtype )
	{
		if( !ref ){
			if( memtype == IALLOCATOR_CL )
				_mem = new ImageAllocatorCL();
			else if( memtype == IALLOCATOR_GL )
				_mem = new ImageAllocatorGL();
			else
				_mem = new ImageAllocatorMem();
			_mem->copy( source._mem, roi );
		} else {
			throw CVTException("Shared image memory not implemented yet");
		}
	}

	void Image::reallocate( size_t w, size_t h, const IFormat & format, IAllocatorType memtype )
	{
		if( _mem->_width == w && _mem->_height == h && _mem->_format == format && _mem->type() == memtype )
			return;
		if( _mem->type() != memtype ) {
			delete _mem;
			if( memtype == IALLOCATOR_CL )
				_mem = new ImageAllocatorCL();
			else if( memtype == IALLOCATOR_GL )
				_mem = new ImageAllocatorGL();
			else
				_mem = new ImageAllocatorMem();
		}
		_mem->alloc( w, h, format );
	}

	void Image::copy( const Image& img )
	{
		if( this == &img )
			return;

		checkFormatAndSize( img, __PRETTY_FUNCTION__, __LINE__ );
		_mem->copy( img._mem );
	}

	void Image::copyRect( int x, int y, const Image& img, const Recti & rect )
	{
		checkFormat( img, __PRETTY_FUNCTION__, __LINE__, _mem->_format );
		int tx, ty;

		tx = -x + rect.x;
		ty = -y + rect.y;
		Recti rdst( 0, 0, ( int ) _mem->_width, ( int ) _mem->_height );
		rdst.translate( tx, ty );
		Recti rsrc( 0, 0, ( int ) img._mem->_width, ( int ) img._mem->_height );
		rsrc.intersect( rect );
		rsrc.intersect( rdst );
		if( rsrc.isEmpty() )
			return;
		rdst.copy( rsrc );
		rdst.translate( -tx, -ty );

		SIMD* simd = SIMD::instance();
		size_t dstride;
		uint8_t* dst = map( &dstride );
		uint8_t* dbase = dst;
		dst += rdst.y * dstride + bpp() * rdst.x;

		size_t sstride;
		const uint8_t* src = img.map( &sstride );
		const uint8_t* sbase = src;
		src += rsrc.y * sstride + rsrc.x * img.bpp();

		size_t n = rsrc.width * img.bpp();
		size_t i = rsrc.height;

		while( i-- ) {
			simd->Memcpy( dst, src, n );
			src += sstride;
			dst += dstride;
		}
		img.unmap( sbase );
		unmap( dbase );
	}


	Image::~Image()
	{
		delete _mem;
	}

	Image& Image::operator=( const Color& c )
	{
		fill( c );
		return *this;
	}

	std::ostream& operator<<( std::ostream &out, const Image &f )
	{
		static const char* _mem_string[] = {
			"MEM",
			"CL",
			"GL"
		};

		out << "Size: " << f.width() << " x " << f.height() << " "
			<< f.format()
			<< " Memory:" << _mem_string[ f.memType() ] << std::endl;
		return out;
	}

	void Image::checkFormat( const Image & img, const char* func, size_t lineNum, const IFormat & format ) const
	{
		if( format != img.format() ){
			throw Exception("Image formats differ: channel order check failed", "Image", lineNum, func);
		}
	}

	void Image::checkSize( const Image & img, const char* func, size_t lineNum, size_t w, size_t h ) const
	{
		if( w != img.width() ){
			throw Exception("Image formats differ: width check failed", "Image", lineNum, func);
		}

		if( h != img.height() ){
			throw Exception("Image formats differ: height check failed", "Image", lineNum, func);
		}
	}

	void Image::checkFormatAndSize( const Image & img, const char* func, size_t lineNum ) const
	{
		checkFormat(img, func, lineNum, _mem->_format );
		checkSize(img, func, lineNum, _mem->_width, _mem->_height );
	}

	void Image::load( const String& path, ILoader* loader )
	{
		if( !loader ) {
			loader = PluginManager::instance().getILoaderForFilename( path );
			if( !loader )
				throw CVTException( "No ILoader for file available" );
		}
		loader->load( *this, path );
	}

	void Image::save( const String& path, ISaver* saver )
	{
		if( !saver ) {
			saver = PluginManager::instance().getISaverForFilename( path );
			if( !saver )
				throw CVTException( "No ISaver for file available" );
		}
		saver->save( path, *this );
	}

}
