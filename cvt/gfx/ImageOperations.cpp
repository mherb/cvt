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

		ret = new float[ kernel._width * kernel._height ];

		pw = ret;

		i = kernel._height;
		while( i-- ) {
			pksrc = ( float* )( kernel._data + i * kernel._stride + sizeof( float ) * kernel._width );
			k = kernel._width;
			while( k-- ) {
				*pw++ = *( --pksrc );
				norm += *pksrc;
			}
		}

		/* normalize if needed and norm != 0 */
		if( normalize && Math::abs( norm - 1.0f ) > Math::EPSILONF && Math::abs( norm ) > Math::EPSILONF ) {
			i = kernel._height * kernel._width;
			norm = 1.0f / norm;
			pw = ret;
			while( i-- )
				*pw++ *= norm;
		}

		return ret;
	}


	void Image::convert( Image& img, ImageChannelOrder order, ImageChannelType type ) const
	{
		SIMD* simd = SIMD::get();
		img.reallocate( _width, _height, order, type );

		if( _order == order && _type == type ) {
			img.copy( *this );
			return;
		}

		/* FIXME: no gamma correction for gray/grayalpha */
		/* FIXME: missing conversion routines */
		uint8_t* src = _data;
		uint8_t* dst = img._data;
		size_t h = _height;

		if( _type == CVT_UBYTE && type == CVT_UBYTE ) {
			switch( _order ) {
				case CVT_GRAY:
					{
						switch( order ) {
							case CVT_GRAYALPHA:
							case CVT_RGBA:
							case CVT_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case CVT_GRAYALPHA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_RGBA:
							case CVT_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case CVT_RGBA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_GRAYALPHA:
								break;
							case CVT_BGRA:
								{
									while( h-- ) {
										simd->Conv_XYZAu8_to_ZYXAu8( dst, src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
						}
					}
				case CVT_BGRA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_GRAYALPHA:
								break;
							case CVT_RGBA:
								{
									while( h-- ) {
										simd->Conv_XYZAu8_to_ZYXAu8( dst, src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
						}
					}
			}
		} else if( _type == CVT_FLOAT && type == CVT_FLOAT ) {
			switch( _order ) {
				case CVT_GRAY:
					{
						switch( order ) {
							case CVT_GRAYALPHA:
							case CVT_RGBA:
							case CVT_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case CVT_GRAYALPHA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_RGBA:
							case CVT_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case CVT_RGBA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case CVT_BGRA:
								{
									while( h-- ) {
										simd->Conv_XYZAf_to_ZYXAf( ( float* ) dst, ( float* ) src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
						}
					}
				case CVT_BGRA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case CVT_RGBA:
								{
									while( h-- ) {
										simd->Conv_XYZAf_to_ZYXAf( ( float* ) dst, ( float* ) src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
						}
					}
			}
		} else if( _type == CVT_UBYTE && type == CVT_FLOAT ) {
			switch( _order ) {
				case CVT_GRAY:
					{
						switch( order ) {
							case CVT_GRAY:
								{
									while( h-- ) {
										simd->Conv_u8_to_f( ( float* ) dst, src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}

							case CVT_GRAYALPHA:
							case CVT_RGBA:
							case CVT_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case CVT_GRAYALPHA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_RGBA:
							case CVT_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case CVT_RGBA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case CVT_RGBA:
								{
									while( h-- ) {
										simd->Conv_XXXAu8_to_XXXAf( ( float* ) dst, src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
							case CVT_BGRA:
								{
									while( h-- ) {
										simd->Conv_XYZAu8_to_ZYXAf( ( float* ) dst, src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
						}
					}
				case CVT_BGRA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_GRAYALPHA:
								break;
							case CVT_BGRA:
								{
									while( h-- ) {
										simd->Conv_XXXAu8_to_XXXAf( ( float* ) dst, src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}

							case CVT_RGBA:
								{
									while( h-- ) {
										simd->Conv_XYZAu8_to_ZYXAf( ( float* ) dst, src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
						}
					}
			}
		} else if( _type == CVT_FLOAT && type == CVT_UBYTE ) {
			switch( _order ) {
				case CVT_GRAY:
					{
						switch( order ) {
							case CVT_GRAY:
								{
									while( h-- ) {
										simd->Conv_f_to_u8( dst, ( float* ) src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
							case CVT_GRAYALPHA:
							case CVT_RGBA:
							case CVT_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case CVT_GRAYALPHA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_RGBA:
							case CVT_BGRA:
								throw CVTException( "Color conversion not implemented" );
						}
					}
				case CVT_RGBA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case CVT_RGBA:
								{
									while( h-- ) {
										simd->Conv_XXXAf_to_XXXAu8( dst, ( float* ) src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
							case CVT_BGRA:
								{
									while( h-- ) {
										simd->Conv_XYZAf_to_ZYXAu8( dst, ( float* ) src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
						}
					}
				case CVT_BGRA:
					{
						switch( order ) {
							case CVT_GRAY:
							case CVT_GRAYALPHA:
								throw CVTException( "Color conversion not implemented" );
							case CVT_BGRA:
								{
									while( h-- ) {
										simd->Conv_XXXAf_to_XXXAu8( dst, ( float* ) src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}

							case CVT_RGBA:
								{
									while( h-- ) {
										simd->Conv_XYZAf_to_ZYXAu8( dst, ( float* ) src, _width );
										src += _stride;
										dst += img._stride;
									}
									return;
								}
						}
					}
			}
		} else
			throw CVTException( "Color conversion not implemented" );
	}

	void Image::fill( const Color& c )
	{
		size_t h;
		uint8_t* dst;
		SIMD* simd = SIMD::get();

		if( _type == CVT_UBYTE ) {
			switch( _order ) {
				case CVT_GRAY:
					{
						uint8_t v = ( uint8_t ) ( 255.0f * c.gray() );
						h = _height;
						dst = _data;
						while( h-- ) {
							simd->SetValueU8( dst, v, _width );
							dst += _stride;
						}
					}
					break;
				case CVT_GRAYALPHA:
					{
						uint16_t v = ( ( uint16_t ) ( 255.0f * c.alpha() ) ) << 8;
						v |= ( ( uint16_t ) ( 255.0f * c.gray() ));
						h = _height;
						dst = _data;
						while( h-- ) {
							simd->SetValueU16( ( uint16_t* ) dst, v, _width );
							dst += _stride;
						}
					}
					break;
				case CVT_RGBA:
					{
						uint32_t v = ( ( uint32_t ) ( 255.0f * c.gray() ) ) << 24;
						v |= ( ( uint32_t ) ( 255.0f * c.blue() ) ) << 16;
						v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
						v |= ( ( uint32_t ) ( 255.0f * c.red() ) );

						h = _height;
						dst = _data;
						while( h-- ) {
							simd->SetValueU32( ( uint32_t* ) dst, v, _width );
							dst += _stride;
						}
					}
					break;
				case CVT_BGRA:
					{
						uint32_t v = ( ( uint32_t ) ( 255.0f * c.gray() ) ) << 24;
						v |= ( ( uint32_t ) ( 255.0f * c.red() ) ) << 16;
						v |= ( ( uint32_t ) ( 255.0f * c.green() ) ) << 8;
						v |= ( ( uint32_t ) ( 255.0f * c.blue() ) );

						h = _height;
						dst = _data;
						while( h-- ) {
							simd->SetValueU32( ( uint32_t* ) dst, v, _width );
							dst += _stride;
						}
					}
					break;
			}
		} else if( _type == CVT_FLOAT ) {
			switch( _order ) {
				case CVT_GRAY:
					{
						float v = c.gray();

						h = _height;
						dst = _data;
						while( h-- ) {
							simd->SetValue1f( ( float* ) dst, v, _width );
							dst += _stride;
						}
					}
					break;
				case CVT_GRAYALPHA:
					{
						float v[ 2 ] = { c.gray(), c.alpha() };

						h = _height;
						dst = _data;
						while( h-- ) {
							simd->SetValue2f( ( float* ) dst, v, _width );
							dst += _stride;
						}
					}
					break;
				case CVT_RGBA:
					{
						float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };

						h = _height;
						dst = _data;
						while( h-- ) {
							simd->SetValue4f( ( float* ) dst, v, _width );
							dst += _stride;
						}
					}
					break;
				case CVT_BGRA:
					{
						float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };

						h = _height;
						dst = _data;
						while( h-- ) {
							simd->SetValue4f( ( float* ) dst, v, _width );
							dst += _stride;
						}
					}
					break;

			}
		} else
			throw CVTException("Unimplemented");
	}

	void Image::add( float alpha )
	{
		SIMD* simd = SIMD::get();
		switch( _type ) {
			case CVT_FLOAT:
				{
					uint8_t* dst = _data;

					size_t h = _height;
					while( h-- ) {
						simd->Add( ( float* ) dst, ( float* ) dst, alpha, _width * _order_channels[ _order ] );
						dst += _stride;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}


	void Image::add( const Color& c )
	{
		SIMD* simd = SIMD::get();
		switch( _type ) {
			case CVT_FLOAT:
				{
					switch( _order ) {
						case CVT_GRAY:
							{
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Add( ( float* ) dst, ( float* ) dst, c.gray(), _width );
									dst += _stride;
								}
							}
							break;
						case CVT_RGBA:
							{
								float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Add( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
									dst += _stride;
								}
							}
							break;
						case CVT_BGRA:
							{
								float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Add( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
									dst += _stride;
								}
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
		switch( _type ) {
			case CVT_FLOAT:
				{
					uint8_t* dst = _data;

					size_t h = _height;
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) dst, alpha, _width * _order_channels[ _order ] );
						dst += _stride;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::sub( const Color& c )
	{
		SIMD* simd = SIMD::get();
		switch( _type ) {
			case CVT_FLOAT:
				{
					switch( _order ) {
						case CVT_GRAY:
							{
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Sub( ( float* ) dst, ( float* ) dst, c.gray(), _width );
									dst += _stride;
								}
							}
							break;
						case CVT_RGBA:
							{
								float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Sub( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
									dst += _stride;
								}
							}
							break;
						case CVT_BGRA:
							{
								float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Sub( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
									dst += _stride;
								}
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
		switch( _type ) {
			case CVT_FLOAT:
				{
					uint8_t* dst = _data;

					size_t h = _height;
					while( h-- ) {
						simd->Mul( ( float* ) dst, ( float* ) dst, alpha, _width * _order_channels[ _order ] );
						dst += _stride;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mul( const Color& c )
	{
		SIMD* simd = SIMD::get();
		switch( _type ) {
			case CVT_FLOAT:
				{
					switch( _order ) {
						case CVT_GRAY:
							{
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Mul( ( float* ) dst, ( float* ) dst, c.gray(), _width );
									dst += _stride;
								}
							}
							break;
						case CVT_RGBA:
							{
								float v[ 4 ] = { c.red(), c.green(), c.blue(), c.alpha() };
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Mul( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
									dst += _stride;
								}
							}
							break;
						case CVT_BGRA:
							{
								float v[ 4 ] = { c.blue(), c.green(), c.red(), c.alpha() };
								uint8_t* dst = _data;
								size_t h = _height;

								while( h-- ) {
									simd->Mul( ( float* ) dst, ( float* ) dst, v, _width *_order_channels[ _order ] );
									dst += _stride;
								}
							}
							break;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::mad( const Image& i, float alpha )
	{
		if( _width != i._width || _height != i._height ||
		   _type != i._type || _order != i._order )
			throw CVTException("Image mismatch");

		SIMD* simd = SIMD::get();
		switch( _type ) {
			case CVT_FLOAT:
				{
					uint8_t* src = i._data;
					uint8_t* dst = _data;

					size_t h = _height;
					while( h-- ) {
						simd->MulAdd( ( float* ) dst, ( float* ) src, alpha, _width * _order_channels[ _order ] );
						src += i._stride;
						dst += _stride;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");

		}
	}

	void Image::ddx( Image& dx, bool forward ) const
	{
		SIMD* simd = SIMD::get();

		dx.reallocate( *this );

		switch( _type ) {
			case CVT_FLOAT:
				{
					size_t h = _height;
					uint8_t* dst = dx._data;
					float* fdst;
					uint8_t* src1 = _data;
					uint8_t* src2 = _data + sizeof( float ) * _order_channels[ _order ];
					size_t c = _order_channels[ _order ];
					size_t i;
					while( h-- ) {
						i = c;
						fdst = ( float* ) dst;
						if( !forward ) {
							while( i-- )
								*fdst++ = 0.0f;
						}
						simd->Sub( fdst, ( float* ) src1, ( float* ) src2, ( _width - 1 ) * _order_channels[ _order ] );
						if( forward ) {
							fdst += ( _width - 1 ) * _order_channels[ _order ];
							while( i-- )
								*fdst++ = 0.0f;
						}
						dst += dx._stride;
						src1 += _stride;
						src2 += _stride;
					}
				}
				break;
			default:
				throw CVTException("Unimplemented");
		}
	}

	void Image::ddy( Image& dy, bool forward ) const
	{
		SIMD* simd = SIMD::get();

		dy.reallocate( *this );

		switch( _type ) {
			case CVT_FLOAT:
				{
					size_t h = _height - 1;
					uint8_t* dst = dy._data;
					uint8_t* src1 = _data;
					uint8_t* src2 = _data + _stride;
					if( !forward ) {
						simd->SetValue1f( ( float* ) dst, 0.0f, _width * _order_channels[ dy._order ] );
						dst += dy._stride;
					}
					while( h-- ) {
						simd->Sub( ( float* ) dst, ( float* ) src1, ( float* ) src2, _width * _order_channels[ dy._order ] );
						dst += dy._stride;
						src1 += _stride;
						src2 += _stride;
					}
					if( forward )
						simd->SetValue1f( ( float* ) dst, 0.0f, _width * _order_channels[ dy._order ] );
				}
				break;
			default:
				throw CVTException("Unimplemented");
		}
	}

	void Image::convolve( Image& idst, const Image& kernel, bool normalize ) const
	{
		if( kernel._order == CVT_GRAY && kernel._type == CVT_FLOAT && _type == CVT_FLOAT )
			convolveFloat( idst, kernel, normalize );
		else {
			std::cout << "type: " << _type << std::endl;
			throw CVTException("Unimplemented");
		}
	}

	void Image::convolveFloat( Image& idst, const Image& kernel, bool normalize ) const
	{
		float* weights;
		float* pweights;
		uint8_t* src;
		uint8_t* psrc;
		uint8_t* dst;
		size_t i, k, b1, b2;
		void (SIMD::*convfunc)( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
		void (SIMD::*convaddfunc)( float* _dst, float const* _src, const size_t width, float const* weights, const size_t wn ) const;
		SIMD* simd = SIMD::get();

		if( _order_channels[ _order ] == 1 ) {
			convfunc = &SIMD::ConvolveClampSet1f;
			convaddfunc = &SIMD::ConvolveClampAdd1f;
		} else if( _order_channels[ _order ] == 2 ) {
			convfunc = &SIMD::ConvolveClampSet2f;
			convaddfunc = &SIMD::ConvolveClampAdd2f;
		} else {
			convfunc = &SIMD::ConvolveClampSet4f;
			convaddfunc = &SIMD::ConvolveClampAdd4f;
		}

		src = _data;

		/* kernel should at least fit once into the image */
		if( _width < kernel._width || _height < kernel._height )
			throw CVTException( "Image smaller than convolution kernel");

		idst.reallocate( *this );
		dst = idst.data();

		/* flip and normalize kernel image */
		weights = imageToKernel( kernel, normalize );

		b1 = ( kernel._height - ( 1 - ( kernel._height & 1 ) ) ) / 2;
		b2 = ( kernel._height + ( 1 - ( kernel._height & 1 ) ) ) / 2;

		/* upper border */
		i = b1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) src, _width, pweights, kernel._width );
			pweights += kernel._width;
			k = i;
			while( k-- ) {
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _width, pweights, kernel._width );
				pweights += kernel._width;
			}
			k = kernel._height - ( i + 1 );
			while( k-- ) {
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _width, pweights, kernel._width );
				psrc += _stride;
				pweights += kernel._width;
			}
			dst += idst._stride;
		}

		/* center */
		i = _height - kernel._height + 1;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) psrc, _width, pweights, kernel._width );
			k = kernel._height - 1;
			while( k-- ) {
				psrc += _stride;
				pweights += kernel._width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _width, pweights, kernel._width );
			}
			dst += idst._stride;
			src += _stride;
		}

		/* lower border */
		i = b2;
		while( i-- ) {
			psrc = src;
			pweights = weights;
			( simd->*convfunc )( ( float* ) dst, ( float* ) psrc, _width, pweights, kernel._width );
			k = b1 + i;
			while( k-- ) {
				psrc += _stride;
				pweights += kernel._width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _width, pweights, kernel._width );
			}
			k = b2 - i;
			while( k-- ) {
				pweights += kernel._width;
				( simd->*convaddfunc )( ( float* ) dst, ( float* ) psrc, _width, pweights, kernel._width );
			}
			dst += idst._stride;
			src += _stride;
		}

		delete[] weights;
	}

	void Image::scale( Image& idst, size_t width, size_t height, const IScaleFilter& filter ) const
	{
		if(_type == CVT_FLOAT )
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
		uint8_t* src;
		uint8_t* send;
		uint8_t* dst;
		size_t i, l;
		int32_t k;
		float** buf;
		size_t bufsize;
		size_t curbuf;
		void (SIMD::*scalex_func)( float* _dst, float const* _src, const size_t width, IConvolveAdaptivef* conva ) const;
		SIMD* simd = SIMD::get();

		if( _order_channels[ _order ] == 1 ) {
			scalex_func = &SIMD::ConvolveAdaptiveClamp1f;
		} else if( _order_channels[ _order ] == 2 ) {
			scalex_func = &SIMD::ConvolveAdaptiveClamp2f;
		} else {
			scalex_func = &SIMD::ConvolveAdaptiveClamp4f;
		}

		idst.reallocate( width, height, _order, _type );

		src = _data;
		dst = idst._data;
		send = src + _stride * _height;

		bufsize = filter.getAdaptiveConvolutionWeights( height, _height, scalery, true );
		filter.getAdaptiveConvolutionWeights( width, _width, scalerx, false );

		buf = new float*[ bufsize ];
		/* allocate and fill buffer */
		for( i = 0; i < bufsize; i++ ) {
			/* FIXME: use aligned buffer !!! */
			buf[ i ] = new float[ width * _order_channels[ _order ] ];
			( simd->*scalex_func )( ( float* ) buf[ i ], ( float* ) src, width, &scalerx );
			src += _stride;
		}
		curbuf = 0;

		pysw = scalery.size;
		pyw = scalery.weights;

		while( height-- ) {
			if( pysw->incr ) {
				for( k = 0; k < pysw->incr && src < send ; k++ ) {
					( simd->*scalex_func )( ( float* ) buf[ ( curbuf + k ) % bufsize ], ( float* ) src, width, &scalerx );
					src += _stride;
				}
				curbuf = ( curbuf + pysw->incr ) % bufsize;
			}

			l = 0;
			while( Math::abs( *pyw ) < Math::EPSILONF ) {
				l++;
				pyw++;
			}
			simd->Mul( ( float* ) dst, buf[ ( curbuf + l ) % bufsize ], *pyw++, width * _order_channels[ _order ] );
			l++;
			for( ; l < pysw->numw; l++ ) {
				if( Math::abs( *pyw ) > Math::EPSILONF )
					simd->MulAdd( ( float* ) dst, buf[ ( curbuf + l ) % bufsize ], *pyw, width * _order_channels[ _order ] );
				pyw++;
			}
			pysw++;
			dst += idst._stride;
		}

		for( i = 0; i < bufsize; i++ )
			delete[] buf[ i ];
		delete[] buf;
		delete[] scalerx.size;
		delete[] scalerx.weights;
		delete[] scalery.size;
		delete[] scalery.weights;
	}
}
