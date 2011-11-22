#include <cvt/gfx/ifilter/ITransform.h>
#include <cvt/gfx/Clipping.h>

namespace cvt {

	static ParamInfoTyped<Image*>   _pinput( "Input" );
	static ParamInfoTyped<Matrix3f> _ptransform( "Transformation" );
	static ParamInfoTyped<uint32_t>	_pwidth( "Width", 0, true, 1, 0 );
	static ParamInfoTyped<uint32_t>	_pheight( "Height", 0, true, 1, 0 );
	static ParamInfoTyped<Image*>	_poutput( "Output", NULL, false );

	static ParamInfo* _itransform_params[] =
	{
		&_pinput,
		&_ptransform,
		&_pwidth,
		&_pheight,
		&_poutput
	};

	ITransform::ITransform() : IFilter( "ImageTransform", _itransform_params, 5, IFILTER_CPU )
	{
	}

	ITransform::~ITransform()
	{
	}

	void ITransform::apply( Image& dst, const Image& src, const Matrix3f& T, size_t width, size_t height )
	{
		if( !width )
			width = src.width();
		if( !height )
			height = src.height();
		dst.reallocate( width, height, src.format() );

		Matrix3f Tinv( T );
		if( !Tinv.inverseSelf() )
			return;

		switch( src.format().formatID ) {
			case IFORMAT_GRAY_FLOAT: return applyFC1( dst, src, T, Tinv );
			case IFORMAT_GRAY_UINT8: return applyU8C1( dst, src, T, Tinv );
			case IFORMAT_RGBA_FLOAT:
			case IFORMAT_BGRA_FLOAT: return applyFC4( dst, src, T, Tinv );
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_BGRA_UINT8: return applyU8C4( dst, src, T, Tinv );
			default: throw CVTException( "Unsupported image format!" );
		}
	}

	void ITransform::apply( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv, size_t width, size_t height )
	{
		if( !width )
			width = src.width();
		if( !height )
			height = src.height();
		dst.reallocate( width, height, src.format() );

		switch( src.format().formatID ) {
			case IFORMAT_GRAY_FLOAT: return applyFC1( dst, src, T, Tinv );
			case IFORMAT_GRAY_UINT8: return applyU8C1( dst, src, T, Tinv );
			case IFORMAT_RGBA_FLOAT:
			case IFORMAT_BGRA_FLOAT: return applyFC4( dst, src, T, Tinv );
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_BGRA_UINT8: return applyU8C4( dst, src, T, Tinv );
			default: throw CVTException( "Unsupported image format!" );
		}
	}

	void ITransform::apply( const ParamSet* attribs, IFilterType iftype ) const
	{
		if( !(getIFilterType() & iftype ) )
			throw CVTException("Invalid filter type (CPU/GPU)!");

	/*	Image* dst;
		Image* src;
		Matrix3f H;
		Color c;

		src = set->arg<Image*>( 0 );
		dst = set->arg<Image*>( 1 );
		H = set->arg<Matrix3f>( 2 );
		c = set->arg<Color>( 3 );*/
	}


	void ITransform::applyFC1( Image& idst, const Image& isrc, const Matrix3f& T, const Matrix3f& Tinv )
	{
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		ssize_t w, h;
		ssize_t sw, sh;
		float* pdst;
		Matrix3f Ttrans;
		Vector2f pt1( 0, 0 ), pt2( 0, 0 );
		SIMD* simd =SIMD::instance();

		dst = idst.map( &dstride );
		dst2 = dst;
		src = isrc.map( &sstride );
		w = idst.width();
		h = idst.height();
		sw = ( ssize_t ) isrc.width();
		sh = ( ssize_t ) isrc.height();

		Ttrans = T.transpose();

		Rectf r( 0 - 1.0f, 0 - 1.0f, sw + 2, sh + 2);
		Vector3f nx = Tinv * Vector3f( 1.0f, 0.0f, 0.0f );
		ssize_t ystart = 0, yend = h;

		Vector2f p = Tinv * Vector2f( 0.0f, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( sw, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( 0.0f, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( sw, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		for( ssize_t y = ystart; y < yend; y++  ) {
			Line2Df l( Vector3f( 0, 1, -y ) );
			Line2Df l2( Ttrans * Vector3f( 0, 1, -y ) );

			pdst = ( float* ) dst2;
			if( Clipping::clip( r, l2, pt1, pt2 ) ) {
				Vector2f px1, px2;
				px1 = T * pt1;
				px2 = T * pt2;

				if( px1.x > px2.x ) {
					Vector2f tmp = px1;
					px1 = px2;
					px2 = tmp;
				}

				ssize_t x1 =  Math::clamp<ssize_t>( px1.x, 0, w );
				ssize_t len =  Math::clamp<ssize_t>( px2.x + 1, 0, w ) -x1;
				Vector3f p = Tinv * Vector3f( x1, y, 1.0f );
				simd->warpLinePerspectiveBilinear1f( pdst + x1, ( const float* ) src, sstride, sw, sh,
													  p.ptr(), nx.ptr(), len );
			}
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}


	void ITransform::applyFC4( Image& idst, const Image& isrc, const Matrix3f& T, const Matrix3f& Tinv )
	{
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		ssize_t w, h;
		ssize_t sw, sh;
		Matrix3f Ttrans;
		float* pdst;
		Vector2f pt1( 0, 0 ), pt2( 0, 0 );
		SIMD* simd =SIMD::instance();

		dst = idst.map( &dstride );
		dst2 = dst;
		src = isrc.map( &sstride );
		w = idst.width();
		h = idst.height();
		sw = ( ssize_t ) isrc.width();
		sh = ( ssize_t ) isrc.height();

		Ttrans = T.transpose();

		Rectf r( 0 - 1.0f, 0 - 1.0f, sw + 2, sh + 2);
		Vector3f nx = Tinv * Vector3f( 1.0f, 0.0f, 0.0f );
		ssize_t ystart = 0, yend = h;

		Vector2f p = Tinv * Vector2f( 0.0f, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( sw, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( 0.0f, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( sw, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		for( ssize_t y = ystart; y < yend; y++  ) {
			Line2Df l( Vector3f( 0, 1, -y ) );
			Line2Df l2( Ttrans * Vector3f( 0, 1, -y ) );

			pdst = ( float* ) dst2;
			if( Clipping::clip( r, l2, pt1, pt2 ) ) {
				Vector2f px1, px2;
				px1 = T * pt1;
				px2 = T * pt2;

				if( px1.x > px2.x ) {
					Vector2f tmp = px1;
					px1 = px2;
					px2 = tmp;
				}

				ssize_t x1 =  Math::clamp<ssize_t>( px1.x, 0, w );
				ssize_t len =  Math::clamp<ssize_t>( px2.x + 1, 0, w ) -x1;
				Vector3f p = Tinv * Vector3f( x1, y, 1.0f );
				simd->warpLinePerspectiveBilinear4f( pdst + 4 * x1, ( const float* ) src, sstride, sw, sh,
													p.ptr(), nx.ptr(), len );
			}
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}

	void ITransform::applyU8C1( Image& idst, const Image& isrc, const Matrix3f& T, const Matrix3f& Tinv )
	{
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		ssize_t w, h;
		ssize_t sw, sh;
		Matrix3f Ttrans;
		Vector2f pt1( 0, 0 ), pt2( 0, 0 );
		SIMD* simd =SIMD::instance();

		dst = idst.map( &dstride );
		dst2 = dst;
		src = isrc.map( &sstride );
		w = idst.width();
		h = idst.height();
		sw = ( ssize_t ) isrc.width();
		sh = ( ssize_t ) isrc.height();

		Ttrans = T.transpose();

		Rectf r( 0 - 1.0f, 0 - 1.0f, sw + 2, sh + 2);
		Vector3f nx = Tinv * Vector3f( 1.0f, 0.0f, 0.0f );
		ssize_t ystart = 0, yend = h;

		Vector2f p = Tinv * Vector2f( 0.0f, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( sw, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( 0.0f, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( sw, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		for( ssize_t y = ystart; y < yend; y++  ) {
			Line2Df l( Vector3f( 0, 1, -y ) );
			Line2Df l2( Ttrans * Vector3f( 0, 1, -y ) );

			if( Clipping::clip( r, l2, pt1, pt2 ) ) {
				Vector2f px1, px2;
				px1 = T * pt1;
				px2 = T * pt2;

				if( px1.x > px2.x ) {
					Vector2f tmp = px1;
					px1 = px2;
					px2 = tmp;
				}

				ssize_t x1 =  Math::clamp<ssize_t>( px1.x, 0, w );
				ssize_t len =  Math::clamp<ssize_t>( px2.x + 1, 0, w ) -x1;
				Vector3f p = Tinv * Vector3f( x1, y, 1.0f );
				simd->warpLinePerspectiveBilinear1u8( dst2 + x1, src, sstride, sw, sh,
													  p.ptr(), nx.ptr(), len );
			}
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}

	void ITransform::applyU8C4( Image& idst, const Image& isrc, const Matrix3f& T, const Matrix3f& Tinv )
	{
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		ssize_t w, h;
		ssize_t sw, sh;
		Matrix3f Ttrans;
		Vector2f pt1( 0, 0 ), pt2( 0, 0 );
		SIMD* simd =SIMD::instance();

		dst = idst.map( &dstride );
		dst2 = dst;
		src = isrc.map( &sstride );
		w = idst.width();
		h = idst.height();
		sw = ( ssize_t ) isrc.width();
		sh = ( ssize_t ) isrc.height();

		Ttrans = T.transpose();

		Rectf r( 0 - 1.0f, 0 - 1.0f, sw + 2, sh + 2);
		Vector3f nx = Tinv * Vector3f( 1.0f, 0.0f, 0.0f );
		ssize_t ystart = 0, yend = h;

		Vector2f p = Tinv * Vector2f( 0.0f, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( sw, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( 0.0f, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		p = Tinv * Vector2f( sw, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( ssize_t ) p.y );
		yend = Math::max( yend, ( ssize_t ) p.y );

		for( ssize_t y = ystart; y < yend; y++  ) {
			Line2Df l( Vector3f( 0, 1, -y ) );
			Line2Df l2( Ttrans * Vector3f( 0, 1, -y ) );

			if( Clipping::clip( r, l2, pt1, pt2 ) ) {
				Vector2f px1, px2;
				px1 = T * pt1;
				px2 = T * pt2;

				if( px1.x > px2.x ) {
					Vector2f tmp = px1;
					px1 = px2;
					px2 = tmp;
				}

				ssize_t x1 =  Math::clamp<ssize_t>( px1.x, 0, w );
				ssize_t len =  Math::clamp<ssize_t>( px2.x + 1, 0, w ) -x1;
				Vector3f p = Tinv * Vector3f( x1, y, 1.0f );
				simd->warpLinePerspectiveBilinear4u8( dst2 + sizeof( uint32_t ) * x1, src, sstride, sw, sh,
													p.ptr(), nx.ptr(), len );
			}
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}


}
