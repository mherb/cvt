#include "gfx/Image.h"
#include "math/Math.h"
#include "util/SIMD.h"
#include "util/Exception.h"

#include <cvt/gfx/IConvert.h>

namespace cvt {

	float* Image::imageToKernel( const Image& kernel, bool normalize ) const
	{
		float* pksrc;
		float* ret;
		float* pw;
		size_t kstride;
		size_t i, k;
		float norm = 0.0f;

		ret = new float[ kernel._mem->_width * kernel._mem->_height ];

		pw = ret;

		i = kernel._mem->_height;
		const uint8_t* data = kernel.map( &kstride );
		while( i-- ) {
			pksrc = ( float* )( data + i * kstride + sizeof( float ) * kernel._mem->_width );
			k = kernel._mem->_width;
			while( k-- ) {
				*pw++ = *( --pksrc );
				norm += *pksrc;
			}
		}
		kernel.unmap( data );

		/* normalize if needed and norm != 0 */
		if( normalize && Math::abs( norm - 1.0f ) > Math::EPSILONF && Math::abs( norm ) > Math::EPSILONF ) {
			i = kernel._mem->_height * kernel._mem->_width;
			norm = 1.0f / norm;
			pw = ret;
			while( i-- )
				*pw++ *= norm;
		}

		return ret;
	}

	void Image::convert( Image& dst ) const
	{
		IConvert::convert( dst, *this );
	}
	
	void Image::convert( Image & dst, const IFormat & dstFormat ) const
	{
		dst.reallocate( _mem->_width, _mem->_height, dstFormat, dst.memType() );
		IConvert::convert( dst, *this );
	}
	
	void Image::fill( const Color& c )
	{
		size_t h, stride;
		uint8_t* dst;
		uint8_t* dbase;
		SIMD* simd = SIMD::instance();
		
		switch ( _mem->_format.formatID ) {
			case IFORMAT_GRAY_UINT8:
				{
					uint8_t v = ( uint8_t ) ( 255.0f * c.gray() );
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValueU8( dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;				
			case IFORMAT_GRAY_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAY_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAY_FLOAT:
				{
					float v = c.gray();
					
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValue1f( ( float* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			
			case IFORMAT_GRAYALPHA_UINT8:
				{
					uint16_t v = ( uint16_t ) ( ( ( uint16_t ) ( 255.0f * c.alpha() ) ) << 8 );
					v |= ( ( uint16_t ) ( 255.0f * c.gray() ));
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValueU16( ( uint16_t* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_GRAYALPHA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAYALPHA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_GRAYALPHA_FLOAT:
				{
					float v[ 2 ] = { c.gray(), c.alpha() };
					
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValue2f( ( float* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_UINT8:
				{
					uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
					v |= ( ( uint32_t ) ( 255.0f * c.blue() ) ) << 16;
					v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
					v |= ( ( uint32_t ) ( 255.0f * c.red() ) );
					
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValueU32( ( uint32_t* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_RGBA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
					
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValue4f( ( float* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_UINT8:
				{
					uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
					v |= ( ( uint32_t ) ( 255.0f * c.red() ) ) << 16;
					v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
					v |= ( ( uint32_t ) ( 255.0f * c.blue() ) );
					
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValueU32( ( uint32_t* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}	
				break;
			case IFORMAT_BGRA_UINT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_BGRA_INT16:
				throw CVTException("Unimplemented");
				break;
			case IFORMAT_BGRA_FLOAT:
				{						
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
					
					h = _mem->_height;
					dbase = dst = map( &stride );
					while( h-- ) {
						simd->SetValue4f( ( float* ) dst, v, _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
				
			case IFORMAT_BAYER_RGGB_UINT8:
				throw CVTException("Unimplemented");
				break;
			default:
				throw CVTException("Unimplemented");
				break;				
		}
	}

	void Image::add( float alpha )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}


	void Image::add( const Color& c )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.formatID ) {
			case IFORMAT_GRAY_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( & stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;				
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( float alpha )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( const Color& c )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.formatID ) {
			case IFORMAT_GRAY_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
				
				
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( float alpha )
	{
		SIMD* simd = SIMD::instance();
		
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;

					while( h-- ) {
						simd->Mul( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( const Color& c )
	{
		SIMD* simd = SIMD::instance();
		switch( _mem->_format.formatID ) {
			case IFORMAT_GRAY_FLOAT:
				{
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Mul( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_RGBA_FLOAT:
				{
					float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Mul( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;
			case IFORMAT_BGRA_FLOAT:
				{
					float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
					size_t stride;
					uint8_t* dst = map( &stride );
					uint8_t* dbase = dst;
					size_t h = _mem->_height;
					
					while( h-- ) {
						simd->Mul( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_format.channels );
						dst += stride;
					}
					unmap( dbase );
				}
				break;				
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::add( const Image& i )
	{
		if( _mem->_width != i._mem->_width || 
		    _mem->_height != i._mem->_height ||
		    _mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t sstride, dstride;
					const uint8_t* src = i.map( &sstride );
					const uint8_t* sbase = src;
					uint8_t* dst = map( &dstride );
					uint8_t* dbase = dst;

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_format.channels );
						src += sstride;
						dst += dstride;
					}
					unmap( dbase );
					i.unmap( sbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( const Image& i )
	{
		if( _mem->_width != i._mem->_width || 
		    _mem->_height != i._mem->_height ||
		    _mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t sstride, dstride;
					const uint8_t* src = i.map( &sstride );
					const uint8_t* sbase = src;
					uint8_t* dst = map( &dstride );
					uint8_t* dbase = dst;

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_format.channels );
						src += sstride;
						dst += dstride;
					}
					unmap( dbase );
					i.unmap( sbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( const Image& i )
	{
		if( _mem->_width != i._mem->_width || 
		    _mem->_height != i._mem->_height ||
		    _mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t sstride, dstride;
					const uint8_t* src = i.map( &sstride );
					const uint8_t* sbase = src;
					uint8_t* dst = map( &dstride );
					uint8_t* dbase = dst;

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Mul(( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_format.channels );
						src += sstride;
						dst += dstride;
					}
					unmap( dbase);
					i.unmap( sbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mad( const Image& i, float alpha )
	{
		if( _mem->_width != i._mem->_width ||
		    _mem->_height != i._mem->_height ||
		    _mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::instance();
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t sstride, dstride;
					const uint8_t* src = i.map( &sstride );
					const uint8_t* sbase = src;
					uint8_t* dst = map( &dstride );
					uint8_t* dbase = dst;

					size_t h = _mem->_height;
					while( h-- ) {
						simd->MulAdd( ( float* ) dst, ( float* ) src, alpha, _mem->_width * _mem->_format.channels );
						src += sstride;
						dst += dstride;
					}
					unmap( dbase);
					i.unmap( sbase );
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}
	
	float Image::ssd( const Image& i ) const
	{
		if( _mem->_width != i._mem->_width ||
		    _mem->_height != i._mem->_height ||
		    _mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");
		
		SIMD* simd = SIMD::instance();
		
		float ssd = 0.0f;
		
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
			{
				size_t astride, bstride;
				const uint8_t* srcA = i.map( &astride );
				const uint8_t* baseA = srcA;
				const uint8_t* srcB = map( &bstride );
				const uint8_t* baseB = srcB;
											
				size_t h = _mem->_height;
				while( h-- ) {
					ssd += simd->SSD( ( float* ) srcA, ( float* )srcB, _mem->_width * _mem->_format.channels );
					srcA += astride;
					srcB += bstride;
				}
				unmap( baseB );
				i.unmap( baseA );
			}
				break;
			case IFORMAT_TYPE_UINT8:
			{				
				size_t astride, bstride;
				const uint8_t* srcA = i.map( &astride );
				const uint8_t* baseA = srcA;
				const uint8_t* srcB = map( &bstride );
				const uint8_t* baseB = srcB;
				
				size_t h = _mem->_height;
				while( h-- ) {
					ssd += simd->SSD( srcA, srcB, _mem->_width * _mem->_format.channels );
					srcA += astride;
					srcB += bstride;
				}
				unmap( baseB );
				i.unmap( baseA );
			}
				break;
			default:
				throw CVTException("Unimplemented");
				
		}
		return ssd;
	}
	
	float Image::sad( const Image& i ) const
	{
		if( _mem->_width != i._mem->_width || 
		    _mem->_height != i._mem->_height ||
		    _mem->_format != i._mem->_format )
			throw CVTException("Image mismatch");
		
		SIMD* simd = SIMD::instance();
		
		float sad = 0.0f;
		
		switch( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				{
					size_t astride, bstride;
					const uint8_t* srcA = i.map( &astride );
					const uint8_t* baseA = srcA;
					const uint8_t* srcB = map( &bstride );
					const uint8_t* baseB = srcB;

					size_t h = _mem->_height;
					while( h-- ) {
						sad += simd->SAD( ( float* ) srcA, ( float* )srcB, _mem->_width * _mem->_format.channels );
						srcA += astride;
						srcB += bstride;
					}
					unmap( baseB );
					i.unmap( baseA );
				}
				break;
			case IFORMAT_TYPE_UINT8:
			{
				size_t astride, bstride;
				const uint8_t* srcA = i.map( &astride );
				const uint8_t* baseA = srcA;
				const uint8_t* srcB = map( &bstride );
				const uint8_t* baseB = srcB;
				
				size_t h = _mem->_height;
				while( h-- ) {
					sad += simd->SAD( srcA, srcB, _mem->_width * _mem->_format.channels );
					srcA += astride;
					srcB += bstride;
				}
				unmap( baseB );
				i.unmap( baseA );
			}
				break;
			default:
				throw CVTException("Unimplemented");
				
		}
		
		return sad ;
	}	

	void Image::convolve( Image& idst, const Image& kernel, bool normalize ) const
	{
		if( kernel._mem->_format == IFormat::GRAY_FLOAT &&
		    _mem->_format.type == IFORMAT_TYPE_FLOAT )
			convolveFloat( idst, kernel, normalize );
		else {
			throw CVTException("Unimplemented");
		}
	}

	void Image::convolveFloat( Image& idst, const Image& kernel, bool normalize ) const
	{
		float* weights;
		float* pweights;
		const uint8_t* src;
		const uint8_t* osrc;
		const uint8_t* psrc;
		uint8_t* dst;
		uint8_t* odst;
		size_t i, k, b1, b2;
		size_t sstride, dstride;
		void (SIMD::*convfunc)( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
		void (SIMD::*convaddfunc)( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
		SIMD* simd = SIMD::instance();

		if( _mem->_format.channels == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1f;
			convaddfunc = &SIMD::ConvolveClampAdd1f;
		} else if( _mem->_format.channels == 2 ) {
			convfunc = &SIMD::ConvolveClampSet2f;
			convaddfunc = &SIMD::ConvolveClampAdd2f;
		} else {
			convfunc = &SIMD::ConvolveClampSet4f;
			convaddfunc = &SIMD::ConvolveClampAdd4f;
		}


		/* kernel should at least fit once into the image */
		if( _mem->_width < kernel._mem->_width || _mem->_height < kernel._mem->_height ) {
			throw CVTException( "Image smaller than convolution kernel");
		}

		checkFormatAndSize( idst, __PRETTY_FUNCTION__, __LINE__ );

		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );

		/* flip and normalize kernel image */
		weights = imageToKernel( kernel, normalize );

		b1 = ( kernel._mem->_height - ( 1 - ( kernel._mem->_height & 1 ) ) ) / 2;
		b2 = ( kernel._mem->_height + ( 1 - ( kernel._mem->_height & 1 ) ) ) / 2;

		/* upper border */
		i = b1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) src, _mem->_width, pweights, kernel._mem->_width );
			pweights += kernel._mem->_width;
			k = i;
			while( k-- ) {
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
				pweights += kernel._mem->_width;
			}
			k = kernel._mem->_height - ( i + 1 );
			while( k-- ) {
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
				psrc += sstride;
				pweights += kernel._mem->_width;
			}
			dst += dstride;
		}

		/* center */
		i = _mem->_height - kernel._mem->_height + 1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			k = kernel._mem->_height - 1;
			while( k-- ) {
				psrc += sstride;
				pweights += kernel._mem->_width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			}
			dst += dstride;
			src += sstride;
		}

		/* lower border */
		i = b2;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			k = b1 + i;
			while( k-- ) {
				psrc += sstride;
				pweights += kernel._mem->_width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			}
			k = b2 - i;
			while( k-- ) {
				pweights += kernel._mem->_width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			}
			dst += dstride;
			src += sstride;
		}

		unmap( osrc );
		idst.unmap( odst );
		delete[] weights;
	}

	void Image::scale( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		switch ( _mem->_format.type ) {
			case IFORMAT_TYPE_FLOAT:
				scaleFloat( idst, width, height, filter );
				break;
			case IFORMAT_TYPE_UINT8:
				scaleFixedu8( idst, width, height, filter );
				break;
			default:
				throw CVTException("Unimplemented");
				break;
		}		
	}

	void Image::scaleFloat( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		IConvolveAdaptivef scalerx;
		IConvolveAdaptivef scalery;
		IConvolveAdaptiveSize* pysw;
		float* pyw;
		const uint8_t* src;
		const uint8_t* osrc;
		const uint8_t* send;
		uint8_t* dst;
		uint8_t* odst;
		size_t sstride, dstride;
		size_t i, l;
		int32_t k;
		float** buf;
		size_t bufsize;
		size_t curbuf;
		void (SIMD::*scalex_func)( float* _dst, float const* _src, const size_t width, IConvolveAdaptivef* conva ) const;
		SIMD* simd = SIMD::instance();


		if( _mem->_format.channels == 1 ) {
			scalex_func = &SIMD::ConvolveAdaptiveClamp1f;
		} else if( _mem->_format.channels == 2 ) {
			scalex_func = &SIMD::ConvolveAdaptiveClamp2f;
		} else {
			scalex_func = &SIMD::ConvolveAdaptiveClamp4f;
		}

		//checkFormat( idst, __PRETTY_FUNCTION__, __LINE__, _mem->_format );
		//checkSize( idst, __PRETTY_FUNCTION__, __LINE__, width, height );
		idst.reallocate( width, height, this->format() );

		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );
		send = src + sstride * _mem->_height;

		bufsize = filter.getAdaptiveConvolutionWeights( height, _mem->_height, scalery, true );
		filter.getAdaptiveConvolutionWeights( width, _mem->_width, scalerx, false );

		buf = new float*[ bufsize ];
		/* allocate and fill buffer */
		for( i = 0; i < bufsize; i++ ) {
			if( posix_memalign( ( void** ) &buf[ i ], 16, sizeof( float ) * width * _mem->_format.channels) )
				throw CVTException("Out of memory");
			( simd->*scalex_func )( ( float* ) buf[ i ], ( float* ) src, width, &scalerx );
			src += sstride;
		}
		curbuf = 0;

		pysw = scalery.size;
		pyw = scalery.weights;

		while( height-- ) {
			if( pysw->incr ) {
				for( k = 0; k < pysw->incr && src < send ; k++ ) {
					( simd->*scalex_func )( ( float* ) buf[ ( curbuf + k ) % bufsize ], ( float* ) src, width, &scalerx );
					src += sstride;
				}
				curbuf = ( curbuf + pysw->incr ) % bufsize;
			}

			l = 0;
			while( Math::abs( *pyw ) < Math::EPSILONF ) {
				l++;
				pyw++;
			}
			simd->Mul( ( float* ) dst, buf[ ( curbuf + l ) % bufsize ], *pyw++, width * _mem->_format.channels );
			l++;
			for( ; l < pysw->numw; l++ ) {
				if( Math::abs( *pyw ) > Math::EPSILONF )
					simd->MulAdd( ( float* ) dst, buf[ ( curbuf + l ) % bufsize ], *pyw, width * _mem->_format.channels );
				pyw++;
			}
			pysw++;
			dst += dstride;
		}

		idst.unmap( odst );
		unmap( osrc );

		for( i = 0; i < bufsize; i++ )
			free( buf[ i ] );
		delete[] buf;
		delete[] scalerx.size;
		delete[] scalerx.weights;
		delete[] scalery.size;
		delete[] scalery.weights;
	}
	
	void Image::scaleFixedu8( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		IConvolveAdaptiveFixed scalerx;
		IConvolveAdaptiveFixed scalery;
		
		IConvolveAdaptiveSize* pysw;
		Fixed* pyw;
		
		const uint8_t* src;
		const uint8_t* osrc;
		const uint8_t* send;
		uint8_t* dst;
		uint8_t* odst;
		size_t sstride, dstride;
		size_t i, l;
		int32_t k;
		Fixed** buf;
		size_t bufsize;
		size_t curbuf;
		void (SIMD::*scalex_func)( Fixed* _dst, uint8_t const* _src, const size_t width, IConvolveAdaptiveFixed* conva ) const;
		SIMD* simd = SIMD::instance();		
		
		if( _mem->_format.channels == 1 ) {
			scalex_func = &SIMD::ConvolveAdaptive1Fixed;
		} else if( _mem->_format.channels == 2 ) {
			scalex_func = &SIMD::ConvolveAdaptive2Fixed;
		} else {
			scalex_func = &SIMD::ConvolveAdaptive4Fixed;
		}
		
		idst.reallocate( width, height, this->format() );
		
		osrc = src = map( &sstride );
		odst = dst = idst.map( &dstride );
		send = src + sstride * _mem->_height;
		
		bufsize = filter.getAdaptiveConvolutionWeights( height, _mem->_height, scalery, true );
		filter.getAdaptiveConvolutionWeights( width, _mem->_width, scalerx, false );
		
		buf = new Fixed*[ bufsize ];
		// allocate and fill buffer
		for( i = 0; i < bufsize; i++ ) {
			if( posix_memalign( ( void** ) &buf[ i ], 16, sizeof( Fixed ) * width * _mem->_format.channels) )
				throw CVTException("Out of memory");
			( simd->*scalex_func )( buf[ i ], src, width, &scalerx );
			src += sstride;
		}
		curbuf = 0;
		
		pysw = scalery.size;
		pyw = scalery.weights;
		
		Fixed* accumBuf = new Fixed[ width * _mem->_format.channels ];
		while( height-- ) {
			if( pysw->incr ) {
				for( k = 0; k < pysw->incr && src < send ; k++ ) {
					( simd->*scalex_func )( buf[ ( curbuf + k ) % bufsize ], src, width, &scalerx );
					src += sstride;
				}
				curbuf = ( curbuf + pysw->incr ) % bufsize;
			}
			
			l = 0;
			while( *pyw == ( Fixed )0.0f ) {
				l++;
				pyw++;
			}
			simd->Mul( accumBuf, buf[ ( curbuf + l ) % bufsize ], *pyw++, width * _mem->_format.channels );
			l++;
			for( ; l < pysw->numw; l++ ) {
				if( *pyw != ( Fixed )0.0f )
					simd->MulAdd( accumBuf, buf[ ( curbuf + l ) % bufsize ], *pyw, width * _mem->_format.channels );
				pyw++;
			}
			
			for( size_t w = 0;  w < width * _mem->_format.channels; w++ ){
				dst[ w ] = Math::clamp( accumBuf[ w ].round(), 0, 255 );
			}
			
			pysw++;
			dst += dstride;
		}
		
		idst.unmap( odst );
		unmap( osrc );
		
		for( i = 0; i < bufsize; i++ )
			free( buf[ i ] );
		delete[] buf;
		delete[] scalerx.size;
		delete[] scalerx.weights;
		delete[] scalery.size;
		delete[] scalery.weights;
		delete[] accumBuf;
	}

	void Image::warpBilinear( Image& idst, const Image& warp ) const
	{
		size_t m, n, k, K;
		size_t sstride, dstride, wstride;

		if( _mem->_format.type == IFORMAT_TYPE_FLOAT && 
			warp._mem->_format == IFormat::GRAYALPHA_FLOAT ) {
			const uint8_t* src;
			const uint8_t* osrc;
			uint8_t* dst;
			uint8_t* odst;
			const uint8_t* wrp;
			const uint8_t* owrp;
			float* pdst;
			const float* pwrp;
			float data[ 4 ];

			checkFormatAndSize( idst, __PRETTY_FUNCTION__, __LINE__ );

			osrc = src = map( &sstride );
			odst = dst = idst.map( &dstride );
			owrp = wrp = warp.map( &wstride );
			K = channels();

			for( n = 0; n < warp._mem->_height; n++ ) {
				pdst = ( float* ) dst;
				pwrp = ( float* ) wrp;
				for( m = 0; m < warp._mem->_width; m++ ) {
					float x, y, alpha, beta;
					size_t ix[ 2 ], iy[ 2 ];
					x = *pwrp++;
					y = *pwrp++;
					alpha = x - Math::floor( x );
					beta  = y - Math::floor( y );
					ix[ 0 ] = ( size_t ) Math::clamp( ( float ) m + x, 0.0f, ( float ) ( _mem->_width - 1 ) );
					iy[ 0 ] = ( size_t ) Math::clamp( ( float ) n + y, 0.0f, ( float ) ( _mem->_height - 1 ) );
					ix[ 1 ] = Math::min( ix[ 0 ] + 1, _mem->_width - 1 );
					iy[ 1 ] = Math::min( iy[ 0 ] + 1, _mem->_height - 1 );
					for( k = 0; k < K; k++ ) {
						data[ 0 ] = *( ( float* ) ( src + sstride * iy[ 0 ] + ( ix[ 0 ] * K + k ) * sizeof( float ) ) );
						data[ 1 ] = *( ( float* ) ( src + sstride * iy[ 0 ] + ( ix[ 1 ] * K + k ) * sizeof( float ) ) );
						data[ 2 ] = *( ( float* ) ( src + sstride * iy[ 1 ] + ( ix[ 0 ] * K + k ) * sizeof( float ) ) );
						data[ 3 ] = *( ( float* ) ( src + sstride * iy[ 1 ] + ( ix[ 1 ] * K + k ) * sizeof( float ) ) );
						data[ 0 ] = Math::mix( data[ 0 ], data[ 1 ], alpha );
						data[ 2 ] = Math::mix( data[ 2 ], data[ 3 ], alpha );
						*pdst++ = Math::mix( data[ 0 ], data[ 2 ], beta );
					}
				}
				dst += dstride;
				wrp += wstride;
			}
			warp.unmap( owrp );
			idst.unmap( odst );
			unmap( osrc );
		} else
			throw CVTException("Unimplemented");
	}
}
