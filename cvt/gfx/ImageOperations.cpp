#include "gfx/Image.h"
#include "math/Math.h"
#include "util/SIMD.h"
#include "util/Exception.h"

namespace cvt {

	float* Image::imageToKernel( const Image& kernel, bool normalize ) const
	{
		float* pksrc;
		float* ret;
		float* pw;
		size_t i, k;
		float norm = 0.0f;

		ret = new float[ kernel._mem->_width * kernel._mem->_height ];

		pw = ret;

		i = kernel._mem->_height;
		const uint8_t* data = kernel.map();
		while( i-- ) {
			pksrc = ( float* )( data + i * kernel._mem->_stride + sizeof( float ) * kernel._mem->_width );
			k = kernel._mem->_width;
			while( k-- ) {
				*pw++ = *( --pksrc );
				norm += *pksrc;
			}
		}
		kernel.unmap();

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

#define CONV( func, dsttype, srctype, width )							\
	{																	\
		src = map();													\
		dst = img.map();												\
		h = _mem->_height;												\
		while( h-- ) {													\
			simd->func( ( dsttype ) dst, ( const srctype ) src, width );\
			src += _mem->_stride;										\
			dst += img._mem->_stride;									\
		}																\
		unmap();														\
		img.unmap();													\
		return;															\
	}

	void Image::convert( Image& img, IOrder order, IType type ) const
	{
		SIMD* simd = SIMD::get();
		const uint8_t* src;
		uint8_t* dst;
		size_t h;

		checkSize( img, __PRETTY_FUNCTION__, __LINE__, _mem->_width, _mem->_height );
		
		if( _mem->_order == order && _mem->_type == type ) {
			img.copy( *this );
			return;
		}

		/* FIXME: no gamma correction for gray/grayalpha */
		/* FIXME: missing conversion routines */

		if( _mem->_type == IType::UBYTE && type == IType::UBYTE ) {
			switch( _mem->_order.id ) {
				case ICHANNELORDER_GRAY:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAYALPHA:
							case ICHANNELORDER_RGBA:
							case ICHANNELORDER_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case ICHANNELORDER_GRAYALPHA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_RGBA:
							case ICHANNELORDER_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case ICHANNELORDER_RGBA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
								break;
							case ICHANNELORDER_BGRA:
								CONV( Conv_XYZAu8_to_ZYXAu8, uint8_t*, uint8_t*, _mem->_width  )
						}
					}
				case ICHANNELORDER_BGRA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
								break;
							case ICHANNELORDER_RGBA:
								CONV( Conv_XYZAu8_to_ZYXAu8, uint8_t*, uint8_t*, _mem->_width  )
						}
					}
			}
		} else if( _mem->_type == IType::FLOAT && type == IType::FLOAT ) {
			switch( _mem->_order.id ) {
				case ICHANNELORDER_GRAY:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAYALPHA:
							case ICHANNELORDER_RGBA:
							case ICHANNELORDER_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case ICHANNELORDER_GRAYALPHA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_RGBA:
							case ICHANNELORDER_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case ICHANNELORDER_RGBA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case ICHANNELORDER_BGRA:
								CONV( Conv_XYZAf_to_ZYXAf, float*, float*, _mem->_width  )
						}
					}
				case ICHANNELORDER_BGRA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case ICHANNELORDER_RGBA:
								CONV( Conv_XYZAf_to_ZYXAf, float*, float*, _mem->_width  )
						}
					}
			}
		} else if( _mem->_type == IType::UBYTE && type == IType::FLOAT ) {
			switch( _mem->_order.id ) {
				case ICHANNELORDER_GRAY:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
								CONV( Conv_u8_to_f, float*, uint8_t*, _mem->_width  )
							case ICHANNELORDER_GRAYALPHA:
							case ICHANNELORDER_RGBA:
							case ICHANNELORDER_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case ICHANNELORDER_GRAYALPHA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_RGBA:
							case ICHANNELORDER_BGRA:
								throw CVTException( "Color conversion not implemented" );
							case ICHANNELORDER_GRAYALPHA:
								CONV( Conv_u8_to_f, float*, uint8_t*, _mem->_width * _mem->_order.channels )
						}
					}
				case ICHANNELORDER_RGBA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
								CONV( Conv_RGBAu8_to_GRAYf, float*, uint8_t*, _mem->_width )
							case ICHANNELORDER_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case ICHANNELORDER_RGBA:
								CONV( Conv_XXXAu8_to_XXXAf, float*, uint8_t*, _mem->_width )
							case ICHANNELORDER_BGRA:
								CONV( Conv_XYZAu8_to_ZYXAf, float*, uint8_t*, _mem->_width )
						}
					}
				case ICHANNELORDER_BGRA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
								CONV( Conv_BGRAu8_to_GRAYf, float*, uint8_t*, _mem->_width )
							case ICHANNELORDER_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
								break;
							case ICHANNELORDER_BGRA:
								CONV( Conv_XXXAu8_to_XXXAf, float*, uint8_t*, _mem->_width )
							case ICHANNELORDER_RGBA:
								CONV( Conv_XYZAu8_to_ZYXAf, float*, uint8_t*, _mem->_width )
						}
					}
			}
		} else if( _mem->_type == IType::FLOAT && type == IType::UBYTE ) {
			switch( _mem->_order.id ) {
				case ICHANNELORDER_GRAY:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
								CONV( Conv_f_to_u8, uint8_t*, float*, _mem->_width )
							case ICHANNELORDER_GRAYALPHA:
							case ICHANNELORDER_RGBA:
							case ICHANNELORDER_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case ICHANNELORDER_GRAYALPHA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_RGBA:
							case ICHANNELORDER_BGRA:
								throw CVTException( "Color conversion not implemented" );
							case ICHANNELORDER_GRAYALPHA:
								CONV( Conv_f_to_u8, uint8_t*, float*, _mem->_width * _mem->_order.channels )
						}
					}
				case ICHANNELORDER_RGBA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case ICHANNELORDER_RGBA:
								CONV( Conv_XXXAf_to_XXXAu8, uint8_t*, float*, _mem->_width )
							case ICHANNELORDER_BGRA:
								CONV( Conv_XYZAf_to_ZYXAu8, uint8_t*, float*, _mem->_width )
						}
					}
				case ICHANNELORDER_BGRA:
					{
						switch( order.id ) {
							case ICHANNELORDER_GRAY:
							case ICHANNELORDER_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case ICHANNELORDER_BGRA:
								CONV( Conv_XXXAf_to_XXXAu8, uint8_t*, float*, _mem->_width )
							case ICHANNELORDER_RGBA:
								CONV( Conv_XYZAf_to_ZYXAu8, uint8_t*, float*, _mem->_width )
						}
					}
			}
		} else
			throw CVTException( "Color conversion not implemented" );
	}

#undef CONV

	void Image::convert( Image& dst ) const
	{
		this->convert( dst, dst.order(), dst.type());
	}

	void Image::fill( const Color& c )
	{
		size_t h;
		uint8_t* dst;
		SIMD* simd = SIMD::get();

		if( _mem->_type == IType::UBYTE ) {
			switch( _mem->_order.id ) {
				case ICHANNELORDER_GRAY:
					{
						uint8_t v = ( uint8_t ) ( 255.0f * c.gray() );
						h = _mem->_height;
						dst = map();
						while( h-- ) {
							simd->SetValueU8( dst, v, _mem->_width );
							dst += _mem->_stride;
						}
						unmap();
					}
					break;
				case ICHANNELORDER_GRAYALPHA:
					{
						uint16_t v = ( uint16_t ) ( ( ( uint16_t ) ( 255.0f * c.alpha() ) ) << 8 );
						v |= ( ( uint16_t ) ( 255.0f * c.gray() ));
						h = _mem->_height;
						dst = map();
						while( h-- ) {
							simd->SetValueU16( ( uint16_t* ) dst, v, _mem->_width );
							dst += _mem->_stride;
						}
						unmap();
					}
					break;
				case ICHANNELORDER_RGBA:
					{
						uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
						v |= ( ( uint32_t ) ( 255.0f * c.blue() ) ) << 16;
						v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
						v |= ( ( uint32_t ) ( 255.0f * c.red() ) );

						h = _mem->_height;
						dst = map();
						while( h-- ) {
							simd->SetValueU32( ( uint32_t* ) dst, v, _mem->_width );
							dst += _mem->_stride;
						}
						unmap();
					}
					break;
				case ICHANNELORDER_BGRA:
					{
						uint32_t v = ( ( uint32_t ) ( 255.0f * c.alpha() ) ) << 24;
						v |= ( ( uint32_t ) ( 255.0f * c.red() ) ) << 16;
						v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
						v |= ( ( uint32_t ) ( 255.0f * c.blue() ) );

						h = _mem->_height;
						dst = map();
						while( h-- ) {
							simd->SetValueU32( ( uint32_t* ) dst, v, _mem->_width );
							dst += _mem->_stride;
						}
						unmap();
					}
					break;
			}
		} else if( _mem->_type == IType::FLOAT ) {
			switch( _mem->_order.id ) {
				case ICHANNELORDER_GRAY:
					{
						float v = c.gray();

						h = _mem->_height;
						dst = map();
						while( h-- ) {
							simd->SetValue1f( ( float* ) dst, v, _mem->_width );
							dst += _mem->_stride;
						}
						unmap();
					}
					break;
				case ICHANNELORDER_GRAYALPHA:
					{
						float v[ 2 ] = { c.gray(), c.alpha() };

						h = _mem->_height;
						dst = map();
						while( h-- ) {
							simd->SetValue2f( ( float* ) dst, v, _mem->_width );
							dst += _mem->_stride;
						}
						unmap();
					}
					break;
				case ICHANNELORDER_RGBA:
					{
						float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };

						h = _mem->_height;
						dst = map();
						while( h-- ) {
							simd->SetValue4f( ( float* ) dst, v, _mem->_width );
							dst += _mem->_stride;
						}
						unmap();
					}
					break;
				case ICHANNELORDER_BGRA:
					{
						float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };

						h = _mem->_height;
						dst = map();
						while( h-- ) {
							simd->SetValue4f( ( float* ) dst, v, _mem->_width );
							dst += _mem->_stride;
						}
						unmap();
					}
					break;

			}
		} else
			throw CVTException("Unimplemented");
	}

	void Image::add( float alpha )
	{
		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					uint8_t* dst = map();
					size_t h = _mem->_height;
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_order.channels );
						dst += _mem->_stride;
					}
					unmap();
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}


	void Image::add( const Color& c )
	{
		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					switch( _mem->_order.id ) {
						case ICHANNELORDER_GRAY:
							{
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Add( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
						case ICHANNELORDER_RGBA:
							{
								float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Add( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_order.channels );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
						case ICHANNELORDER_BGRA:
							{
								float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Add( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_order.channels );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( float alpha )
	{
		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					uint8_t* dst = map();

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_order.channels );
						dst += _mem->_stride;
					}
					unmap();
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( const Color& c )
	{
		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					switch( _mem->_order.id ) {
						case ICHANNELORDER_GRAY:
							{
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Sub( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
						case ICHANNELORDER_RGBA:
							{
								float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Sub( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_order.channels );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
						case ICHANNELORDER_BGRA:
							{
								float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Sub( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_order.channels );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( float alpha )
	{
		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					uint8_t* dst = map();

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Mul( ( float* ) dst, ( float* ) dst, alpha, _mem->_width * _mem->_order.channels );
						dst += _mem->_stride;
					}
					unmap();
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( const Color& c )
	{
		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					switch( _mem->_order.id ) {
						case ICHANNELORDER_GRAY:
							{
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Mul( ( float* ) dst, ( float* ) dst, c.gray(), _mem->_width );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
						case ICHANNELORDER_RGBA:
							{
								float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Mul( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_order.channels );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
						case ICHANNELORDER_BGRA:
							{
								float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
								uint8_t* dst = map();
								size_t h = _mem->_height;

								while( h-- ) {
									simd->Mul( ( float* ) dst, ( float* ) dst, v, _mem->_width * _mem->_order.channels );
									dst += _mem->_stride;
								}
								unmap();
							}
							break;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::add( const Image& i )
	{
		if( _mem->_width != i._mem->_width || _mem->_height != i._mem->_height ||
		   _mem->_type != i._mem->_type || _mem->_order != i._mem->_order )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					const uint8_t* src = i.map();
					uint8_t* dst = map();

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_order.channels );
						src += i._mem->_stride;
						dst += _mem->_stride;
					}
					unmap();
					i.unmap();
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( const Image& i )
	{
		if( _mem->_width != i._mem->_width || _mem->_height != i._mem->_height ||
		   _mem->_type != i._mem->_type || _mem->_order != i._mem->_order )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					const uint8_t* src = i.map();
					uint8_t* dst = map();

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_order.channels );
						src += i._mem->_stride;
						dst += _mem->_stride;
					}
					unmap();
					i.unmap();
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( const Image& i )
	{
		if( _mem->_width != i._mem->_width || _mem->_height != i._mem->_height ||
		   _mem->_type != i._mem->_type || _mem->_order != i._mem->_order )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					const uint8_t* src = i.map();
					uint8_t* dst = map();

					size_t h = _mem->_height;
					while( h-- ) {
						simd->Mul(( float* ) dst, ( float* ) dst, ( float* ) src, _mem->_width * _mem->_order.channels );
						src += i._mem->_stride;
						dst += _mem->_stride;
					}
					unmap();
					i.unmap();
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mad( const Image& i, float alpha )
	{
		if( _mem->_width != i._mem->_width || _mem->_height != i._mem->_height ||
		   _mem->_type != i._mem->_type || _mem->_order != i._mem->_order )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::get();
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					const uint8_t* src = i.map();
					uint8_t* dst = map();

					size_t h = _mem->_height;
					while( h-- ) {
						simd->MulAdd( ( float* ) dst, ( float* ) src, alpha, _mem->_width * _mem->_order.channels );
						src += i._mem->_stride;
						dst += _mem->_stride;
					}
					unmap();
					i.unmap();
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}
	
	float Image::ssd( const Image& i ) const
	{
		if( _mem->_width != i._mem->_width || _mem->_height != i._mem->_height ||
		   _mem->_type != i._mem->_type || _mem->_order != i._mem->_order )
			throw CVTException("Image mismatch");
		
		SIMD* simd = SIMD::get();
		
		float ssd = 0.0f;
		
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
			{
				const uint8_t* srcA = i.map();
				const uint8_t* srcB = map();
											
				size_t h = _mem->_height;
				while( h-- ) {
					ssd += simd->SSD( ( float* ) srcA, ( float* )srcB, _mem->_width * _mem->_order.channels );
					srcA += i._mem->_stride;
					srcB += _mem->_stride;
				}
				unmap();
				i.unmap();
			}
				break;
			default:
				throw CVTException("Unimplemented");
				
		}
		return ssd;
	}
	
	float Image::sad( const Image& i ) const
	{
		if( _mem->_width != i._mem->_width || _mem->_height != i._mem->_height ||
		   _mem->_type != i._mem->_type || _mem->_order != i._mem->_order )
			throw CVTException("Image mismatch");
		
		SIMD* simd = SIMD::get();
		
		float sad = 0.0f;
		
		switch( _mem->_type.id ) {
			case ICHANNELTYPE_FLOAT:
				{
					const uint8_t* srcA = i.map();
					const uint8_t* srcB = map();

					size_t h = _mem->_height;
					while( h-- ) {
						sad += simd->SAD( ( float* ) srcA, ( float* )srcB, _mem->_width * _mem->_order.channels );
						srcA += i._mem->_stride;
						srcB += _mem->_stride;
					}
					unmap();
					i.unmap();
				}
				break;
			default:
				throw CVTException("Unimplemented");
				
		}
		
		return sad ;
	}	

	void Image::convolve( Image& idst, const Image& kernel, bool normalize ) const
	{
		if( kernel._mem->_order == IOrder::GRAY && kernel._mem->_type == IType::FLOAT && _mem->_type == IType::FLOAT )
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
		const uint8_t* psrc;
		const uint8_t* dst;
		size_t i, k, b1, b2;
		void (SIMD::*convfunc)( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
		void (SIMD::*convaddfunc)( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
		SIMD* simd = SIMD::get();

		if( _mem->_order.channels == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1f;
			convaddfunc = &SIMD::ConvolveClampAdd1f;
		} else if( _mem->_order.channels == 2 ) {
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

		src = map();
		dst = idst.map();

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
				psrc += _mem->_stride;
				pweights += kernel._mem->_width;
			}
			dst += idst._mem->_stride;
		}

		/* center */
		i = _mem->_height - kernel._mem->_height + 1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			k = kernel._mem->_height - 1;
			while( k-- ) {
				psrc += _mem->_stride;
				pweights += kernel._mem->_width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			}
			dst += idst._mem->_stride;
			src += _mem->_stride;
		}

		/* lower border */
		i = b2;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			k = b1 + i;
			while( k-- ) {
				psrc += _mem->_stride;
				pweights += kernel._mem->_width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			}
			k = b2 - i;
			while( k-- ) {
				pweights += kernel._mem->_width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _mem->_width, pweights, kernel._mem->_width );
			}
			dst += idst._mem->_stride;
			src += _mem->_stride;
		}

		unmap();
		idst.unmap();
		delete[] weights;
	}

	void Image::scale( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		if(_mem->_type == IType::FLOAT )
			scaleFloat( idst, width, height, filter );
		else
			throw CVTException("Unimplemented");
	}

	void Image::scaleFloat( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		IConvolveAdaptivef scalerx;
		IConvolveAdaptivef scalery;
		IConvolveAdaptiveSize* pysw;
		float* pyw;
		const uint8_t* src;
		const uint8_t* send;
		uint8_t* dst;
		size_t i, l;
		int32_t k;
		float** buf;
		size_t bufsize;
		size_t curbuf;
		void (SIMD::*scalex_func)( float* _dst, float const* _src, const size_t width, IConvolveAdaptivef* conva ) const;
		SIMD* simd = SIMD::get();


		if( _mem->_order.channels == 1 ) {
			scalex_func = &SIMD::ConvolveAdaptiveClamp1f;
		} else if( _mem->_order.channels == 2 ) {
			scalex_func = &SIMD::ConvolveAdaptiveClamp2f;
		} else {
			scalex_func = &SIMD::ConvolveAdaptiveClamp4f;
		}

		checkFormat( idst, __PRETTY_FUNCTION__, __LINE__, _mem->_order, _mem->_type );
		checkSize( idst, __PRETTY_FUNCTION__, __LINE__, width, height );

		src = map();
		dst = idst.map();
		send = src + _mem->_stride * _mem->_height;

		bufsize = filter.getAdaptiveConvolutionWeights( height, _mem->_height, scalery, true );
		filter.getAdaptiveConvolutionWeights( width, _mem->_width, scalerx, false );

		buf = new float*[ bufsize ];
		/* allocate and fill buffer */
		for( i = 0; i < bufsize; i++ ) {
			if( posix_memalign( ( void** ) &buf[ i ], 16, sizeof( float ) * width * _mem->_order.channels) )
				throw CVTException("Out of memory");
			( simd->*scalex_func )( ( float* ) buf[ i ], ( float* ) src, width, &scalerx );
			src += _mem->_stride;
		}
		curbuf = 0;

		pysw = scalery.size;
		pyw = scalery.weights;

		while( height-- ) {
			if( pysw->incr ) {
				for( k = 0; k < pysw->incr && src < send ; k++ ) {
					( simd->*scalex_func )( ( float* ) buf[ ( curbuf + k ) % bufsize ], ( float* ) src, width, &scalerx );
					src += _mem->_stride;
				}
				curbuf = ( curbuf + pysw->incr ) % bufsize;
			}

			l = 0;
			while( Math::abs( *pyw ) < Math::EPSILONF ) {
				l++;
				pyw++;
			}
			simd->Mul( ( float* ) dst, buf[ ( curbuf + l ) % bufsize ], *pyw++, width * _mem->_order.channels );
			l++;
			for( ; l < pysw->numw; l++ ) {
				if( Math::abs( *pyw ) > Math::EPSILONF )
					simd->MulAdd( ( float* ) dst, buf[ ( curbuf + l ) % bufsize ], *pyw, width * _mem->_order.channels );
				pyw++;
			}
			pysw++;
			dst += idst._mem->_stride;
		}

		idst.unmap();
		unmap();

		for( i = 0; i < bufsize; i++ )
			free( buf[ i ] );
		delete[] buf;
		delete[] scalerx.size;
		delete[] scalerx.weights;
		delete[] scalery.size;
		delete[] scalery.weights;
	}

	void Image::warpBilinear( Image& idst, const Image& warp ) const
	{
		size_t m, n, k, K;
		size_t sstride, dstride, wstride;

		if( _mem->_type == IType::FLOAT && warp._mem->_type == IType::FLOAT && warp._mem->_order == IOrder::GRAYALPHA ) {
			const uint8_t* src;
			uint8_t* dst;
			const uint8_t* wrp;
			const float* psrc;
			float* pdst;
			const float* pwrp;
			float data[ 4 ];

			checkFormatAndSize( idst, __PRETTY_FUNCTION__, __LINE__ );

			src = map();
			sstride = _mem->_stride;
			dst = idst.map();
			dstride = idst._mem->_stride;
			wrp = warp.map();
			wstride = warp._mem->_stride;
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
			warp.unmap();
			idst.unmap();
			unmap();
		} else
			throw CVTException("Unimplemented");
	}

	void Image::debayer( Image& idst, IBayerPattern_t pattern ) const
	{
		const uint32_t* src1;
		const uint32_t* src2;
		const uint32_t* src3;
		uint32_t* dst;
		size_t sstride;
		size_t dstride;
		size_t h;
		size_t w, i;
		void (SIMD::*debayereven)( uint32_t*, const uint32_t* , const uint32_t*, const uint32_t*, const size_t ) const;
		void (SIMD::*debayerodd)( uint32_t*, const uint32_t* , const uint32_t*, const uint32_t*, const size_t ) const;
		SIMD* simd = SIMD::get();

		checkSize( idst, __PRETTY_FUNCTION__, __LINE__, _mem->_width, _mem->_height );
		checkFormat( *this, __PRETTY_FUNCTION__, __LINE__, IOrder::GRAY, IType::UBYTE );
		if( !( idst._mem->_type == IType::UBYTE && ( idst._mem->_order == IOrder::RGBA || idst._mem->_order == IOrder::BGRA || idst._mem->_order == IOrder::GRAY ) ) )
			throw CVTException("Unsupported destination format!");

		if( idst._mem->_order == IOrder::RGBA ) {
			debayereven = &SIMD::debayer_EVEN_RGGBu8_RGBAu8;
			debayerodd = &SIMD::debayer_ODD_RGGBu8_RGBAu8;
		} else if( idst._mem->_order == IOrder::BGRA ) {
			debayereven = &SIMD::debayer_EVEN_RGGBu8_BGRAu8;
			debayerodd = &SIMD::debayer_ODD_RGGBu8_BGRAu8;
		} else {
			debayereven = &SIMD::debayer_EVEN_RGGBu8_GRAYu8;
			debayerodd = &SIMD::debayer_ODD_RGGBu8_GRAYu8;
		}


		src1 = ( uint32_t* ) map();
		sstride = stride() >> 2;
		src2 = src1 + sstride;
		src3 = src2 + sstride;
		w = width();
		h = height();
		dst = ( uint32_t* ) idst.map();
		dstride = idst.stride() >> 2;

		( simd->*debayerodd )( dst, src2, src1, src2, w >> 2 );
		dst += dstride;
		h = h - 2 ;
		while( h-- ) {
			i = ( w >> 2 ) - 2;
			if(  h & 1 ) {
				( simd->*debayereven )( dst, src1, src2, src3, w >> 2 );
			} else {
				( simd->*debayerodd )( dst, src1, src2, src3, w >> 2 );
			}
			dst += dstride;
			src1 += sstride;
			src2 += sstride;
			src3 += sstride;
		}
		( simd->*debayerodd )( dst, src2, src1, src2, w >> 2 );
		idst.unmap();
		unmap();
	}
}
