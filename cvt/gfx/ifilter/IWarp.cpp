#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/Clipping.h>

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

		dst = idst.map( &stride );

		idst.unmap( dst );

	}

	void IWarp::applyFC1( Image& idst, const Image& isrc, const Image& iwarp )
	{
		const uint8_t* src;
		uint8_t* dst;
		const uint8_t* wrp;
		uint8_t* pdst;
		const uint8_t* pwrp;
		size_t sstride, dstride, wstride, w, h;

		pdst = dst = idst.map( &dstride );
		pwrp = wrp = iwarp.map( &wstride );
		src = isrc.map( &sstride );

		SIMD* simd = SIMD::instance();

		w = iwarp.width();
		h = iwarp.height();
		while( h-- ) {
			simd->warpBilinear1f( ( float* ) pdst, ( const float* ) pwrp, ( const float* ) src, sstride, w );
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
		size_t sstride, dstride, wstride, w, h;

		pdst = dst = idst.map( &dstride );
		pwrp = wrp = iwarp.map( &wstride );
		src = isrc.map( &sstride );

		SIMD* simd = SIMD::instance();

		w = iwarp.width();
		h = iwarp.height();
		while( h-- ) {
			simd->warpBilinear4f( ( float* ) pdst, ( const float* ) pwrp, ( const float* ) src, sstride, w );
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
		size_t sstride, dstride, wstride, w, h;

		pdst = dst = idst.map( &dstride );
		pwrp = wrp = iwarp.map( &wstride );
		src = isrc.map( &sstride );

		SIMD* simd = SIMD::instance();

		w = iwarp.width();
		h = iwarp.height();
		while( h-- ) {
			simd->warpBilinear1u8( pdst, ( const float* ) pwrp, src, sstride, w );
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
		size_t sstride, dstride, wstride, w, h;

		pdst = dst = idst.map( &dstride );
		pwrp = wrp = iwarp.map( &wstride );
		src = isrc.map( &sstride );

		SIMD* simd = SIMD::instance();

		w = iwarp.width();
		h = iwarp.height();
		while( h-- ) {
			simd->warpBilinear4u8( pdst, ( const float* ) pwrp, src, sstride, w );
			pdst += dstride;
			pwrp += wstride;
		}

		idst.unmap( dst );
		isrc.unmap( src );
		iwarp.unmap( wrp );
	}

}
