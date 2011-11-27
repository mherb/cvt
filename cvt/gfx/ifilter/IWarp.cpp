#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/math/Vector.h>

namespace cvt {

	static ParamInfoTyped<Image*>   _pinput( "Input" );
	static ParamInfoTyped<Image*>   _pwarp( "Warp" );
	static ParamInfoTyped<Image*>	_poutput( "Output", NULL, false );

	static ParamInfo* _itransform_params[] =
	{
		&_pinput,
		&_pwarp,
		&_poutput
	};

	IWarp::IWarp() : IFilter( "ImageWarp", _itransform_params, 3, IFILTER_CPU )
	{
	}

	IWarp::~IWarp()
	{
	}

	void IWarp::apply( Image& dst, const Image& src, const Image& warp )
	{
		if( warp.format() != IFormat::GRAYALPHA_FLOAT )
			throw CVTException( "Unsupported warp image type" );

		dst.reallocate( warp.width(), warp.height(), src.format() );

		switch( src.format().formatID ) {
			case IFORMAT_GRAY_FLOAT: return applyFC1( dst, src, warp );
			case IFORMAT_GRAY_UINT8: return applyU8C1( dst, src, warp );
			case IFORMAT_RGBA_FLOAT:
			case IFORMAT_BGRA_FLOAT: return applyFC4( dst, src, warp );
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_BGRA_UINT8: return applyU8C4( dst, src, warp );
			default: throw CVTException( "Unsupported image format!" );
		}
	}


	void IWarp::apply( const ParamSet* attribs, IFilterType iftype ) const
	{
	}


	void IWarp::warpTunnel( Image& idst, float radius, float cx, float cy )
	{
		if( idst.format() != IFormat::GRAYALPHA_FLOAT )
			throw CVTException( "Unsupported warp image type" );

		uint8_t* dst;
		float* pdst;
		size_t stride;
		size_t w, h;

		dst = idst.map( &stride );
		w = idst.width();
		h = idst.height();

		Vector2f c( cx, cy );
		for( size_t y = 0; y < h; y++ ) {
			pdst = ( float* ) ( dst + y * stride );
			for( size_t x = 0; x < w; x++ ) {
				Vector2f p( x, y );
				p -= c;
				float r = p.length();
				r = Math::min( r, radius );
				float angle = Math::atan2( p.y, p.x );
				*pdst++ = Math::clamp<float>( r * Math::cos( angle ) + c.x, 0, w - 1 );
				*pdst++ = Math::clamp<float>( r * Math::sin( angle ) + c.y, 0, h - 1 );
			}
		}
		idst.unmap( dst );
	}

	void IWarp::warpFishEye( Image& idst, float strength, float cx, float cy )
	{
		if( idst.format() != IFormat::GRAYALPHA_FLOAT )
			throw CVTException( "Unsupported warp image type" );

		uint8_t* dst;
		float* pdst;
		size_t stride;
		size_t w, h;

		dst = idst.map( &stride );
		w = idst.width();
		h = idst.height();

		float R = Vector2f( Math::max( cx, w - 1 - cx ), Math::max( cy, h - 1 - cy ) ).length();

		Vector2f c( cx, cy );
		for( size_t y = 0; y < h; y++ ) {
			pdst = ( float* ) ( dst + y * stride );
			for( size_t x = 0; x < w; x++ ) {
				Vector2f p( x, y );
				p -= c;
				float r = p.length();
				r = R * Math::pow( r / R, strength * 0.5f );
				float angle = Math::atan2( p.y, p.x );
				*pdst++ = Math::clamp<float>( r * Math::cos( angle ) + c.x, 0, w - 1 );
				*pdst++ = Math::clamp<float>( r * Math::sin( angle ) + c.y, 0, h - 1 );
			}
		}
		idst.unmap( dst );
	}

	void IWarp::warpUndistort( Image& idst, float k1, float k2, float cx, float cy, float fx, float fy, size_t srcWidth, size_t srcHeight, float k3, float p1, float p2 )
	{
		if( idst.format() != IFormat::GRAYALPHA_FLOAT )
			throw CVTException( "Unsupported warp image type" );

		uint8_t* dst;
		float* pdst;
		size_t stride;
		size_t w, h;
		float invfx = 1.0f / fx;
		float invfy = 1.0f / fy;

		dst = idst.map( &stride );
		w = idst.width();
		h = idst.height();

		Vector2f c1( cx +  ( ( float ) w - ( float ) srcWidth ) / 2.0f, cy + ( ( float ) h - ( float ) srcHeight ) / 2.0f );
		Vector2f c2( cx, cy );
		for( size_t y = 0; y < h; y++ ) {
			pdst = ( float* ) ( dst + y * stride );
			for( size_t x = 0; x < w; x++ ) {
				Vector2f p( x, y );
				p -= c1;
				p.x *= invfx;
				p.y *= invfy;
				float r2 = p.lengthSqr();
				float r4 = Math::sqr( r2 );
				float r6 = r2 * r4;
				float poly = ( 1.0f + k1 * r2 + k2 * r4 + k3 * r6 );
				float xy2 = 2.0f * p.x * p.y;
				*pdst++ = fx * ( p.x * poly + xy2 * p1 + p2 * ( r2 + 2.0f * p.x * p.x ) ) + c2.x;
				*pdst++ = fy * ( p.y * poly + xy2 * p2 + p1 * ( r2 + 2.0f * p.y * p.y ) ) + c2.y;
			}
		}
		idst.unmap( dst );
	}

	void IWarp::applyFC1( Image& idst, const Image& isrc, const Image& iwarp )
	{
		const uint8_t* src;
		uint8_t* dst;
		const uint8_t* wrp;
		uint8_t* pdst;
		const uint8_t* pwrp;
		size_t sstride, dstride, wstride, w, h, sw, sh;

		pdst = dst = idst.map( &dstride );
		pwrp = wrp = iwarp.map( &wstride );
		src = isrc.map( &sstride );

		SIMD* simd = SIMD::instance();

		sw = isrc.width();
		sh = isrc.height();
		w = iwarp.width();
		h = iwarp.height();
		while( h-- ) {
			simd->warpBilinear1f( ( float* ) pdst, ( const float* ) pwrp, ( const float* ) src, sstride, sw, sh, 0.0f, w );
			pdst += dstride;
			pwrp += wstride;
		}

		idst.unmap( dst );
		isrc.unmap( src );
		iwarp.unmap( wrp );
	}

	void IWarp::applyFC4( Image& idst, const Image& isrc, const Image& iwarp )
	{
		const uint8_t* src;
		uint8_t* dst;
		const uint8_t* wrp;
		uint8_t* pdst;
		const uint8_t* pwrp;
		size_t sstride, dstride, wstride, w, h, sw, sh;
		float black[ ] = { 0.0f, 0.0f, 0.0f, 1.0f };

		pdst = dst = idst.map( &dstride );
		pwrp = wrp = iwarp.map( &wstride );
		src = isrc.map( &sstride );

		SIMD* simd = SIMD::instance();

		sw = isrc.width();
		sh = isrc.height();
		w = iwarp.width();
		h = iwarp.height();
		while( h-- ) {
			simd->warpBilinear4f( ( float* ) pdst, ( const float* ) pwrp, ( const float* ) src, sstride, sw, sh, black, w );
			pdst += dstride;
			pwrp += wstride;
		}

		idst.unmap( dst );
		isrc.unmap( src );
		iwarp.unmap( wrp );
	}

	void IWarp::applyU8C1( Image& idst, const Image& isrc, const Image& iwarp )
	{
		const uint8_t* src;
		uint8_t* dst;
		const uint8_t* wrp;
		uint8_t* pdst;
		const uint8_t* pwrp;
		size_t sstride, dstride, wstride, w, h, sw, sh;

		pdst = dst = idst.map( &dstride );
		pwrp = wrp = iwarp.map( &wstride );
		src = isrc.map( &sstride );

		SIMD* simd = SIMD::instance();

		sw = isrc.width();
		sh = isrc.height();
		w = iwarp.width();
		h = iwarp.height();
		while( h-- ) {
			simd->warpBilinear1u8( pdst, ( const float* ) pwrp, src, sstride, sw, sh, 0, w );
			pdst += dstride;
			pwrp += wstride;
		}

		idst.unmap( dst );
		isrc.unmap( src );
		iwarp.unmap( wrp );
	}

	void IWarp::applyU8C4( Image& idst, const Image& isrc, const Image& iwarp )
	{
		const uint8_t* src;
		uint8_t* dst;
		const uint8_t* wrp;
		uint8_t* pdst;
		const uint8_t* pwrp;
		size_t sstride, dstride, wstride, w, h, sw, sh;
		uint32_t black = 0xff000000;

		pdst = dst = idst.map( &dstride );
		pwrp = wrp = iwarp.map( &wstride );
		src = isrc.map( &sstride );

		SIMD* simd = SIMD::instance();

		sw = isrc.width();
		sh = isrc.height();
		w = iwarp.width();
		h = iwarp.height();
		while( h-- ) {
			simd->warpBilinear4u8( pdst, ( const float* ) pwrp, src, sstride, sw, sh, black, w );
			pdst += dstride;
			pwrp += wstride;
		}

		idst.unmap( dst );
		isrc.unmap( src );
		iwarp.unmap( wrp );
	}

}
