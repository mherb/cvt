#include "ITransform.h"
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

	void ITransform::apply( Image& dst, const Image& src, const Matrix3f& transform )
	{
		if( src.format() != dst.format() )
			throw CVTException( "Image formats do not match!" );

		switch( src.format().formatID ) {
			case IFORMAT_GRAY_FLOAT: return applyFC1( dst, src, transform );
			case IFORMAT_GRAY_UINT8: return applyU8C1( dst, src, transform );
			case IFORMAT_RGBA_FLOAT:
			case IFORMAT_BGRA_FLOAT: return applyFC4( dst, src, transform );
			case IFORMAT_RGBA_UINT8:
			case IFORMAT_BGRA_UINT8: return applyU8C4( dst, src, transform );
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


	void ITransform::applyFC1( Image& idst, const Image& isrc, const Matrix3f& transform )
	{
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		size_t w, h;
		size_t sw, sh;
		Matrix3f invtrans, invtt;
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

		invtrans = transform.inverse();
		invtt = invtrans.transpose();

		Rectf r( 0 - 1.0f, 0 - 1.0f, sw + 2, sh + 2);
		Vector3f nx = transform * Vector3f( 1.0f, 0.0f, 0.0f );
		size_t ystart = 0, yend = h;
		Vector2f p = invtrans * Vector2f( 0.0f, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( sw, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( 0.0f, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( sw, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );

		for( size_t y = ystart; y < yend; y++  ) {
			Line2Df l( 0, y, w, y );
			Line2Df l2( invtt * l.vector() );

			pdst = ( float* ) dst2;
			if( Clipping::clip( r, l2, pt1, pt2 ) ) {
				Vector2f px1, px2;
				px1 = invtrans * pt1;
				px2 = invtrans * pt2;

				if( px1.x > px2.x ) {
					Vector2f tmp( px1 );
					px1 = px2;
					px2 = tmp;
				}

				Vector3f p = transform * Vector3f( ( float ) Math::clamp<size_t>( ( px1.x ), 0, w ), y, 1.0f );
				simd->warpLinePerspectiveBilinear1f( ( float* ) ( pdst +  Math::clamp<size_t>( px1.x, 0, w ) ), ( const float* ) src, sstride, sw, sh,
													  p.ptr(), nx.ptr(), Math::clamp<ssize_t>(  px2.x + 1, 0, w ) - Math::clamp<ssize_t>( ( px1.x ), 0, w ) );
			}
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}


	void ITransform::applyFC4( Image& idst, const Image& isrc, const Matrix3f& transform )
	{
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		size_t w, h;
		size_t sw, sh;
		Matrix3f invtrans, invtt;
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

		invtrans = transform.inverse();
		invtt = invtrans.transpose();

		Rectf r( 0 - 1.0f, 0 - 1.0f, sw + 2, sh + 2);
		Vector3f nx = transform * Vector3f( 1.0f, 0.0f, 0.0f );
		size_t ystart = 0, yend = h;
		Vector2f p = invtrans * Vector2f( 0.0f, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( sw, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( 0.0f, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( sw, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );

		for( size_t y = ystart; y < yend; y++  ) {
			Line2Df l( 0, y, w, y );
			Line2Df l2( invtt * l.vector() );

			pdst = ( float* ) dst2;
			if( Clipping::clip( r, l2, pt1, pt2 ) ) {
				Vector2f px1, px2;
				px1 = invtrans * pt1;
				px2 = invtrans * pt2;

				if( px1.x > px2.x ) {
					Vector2f tmp( px1 );
					px1 = px2;
					px2 = tmp;
				}

				Vector3f p = transform * Vector3f( ( float ) Math::clamp<size_t>( ( px1.x ), 0, w ), y, 1.0f );
				simd->warpLinePerspectiveBilinear4f( ( float* ) ( pdst +  4 * Math::clamp<size_t>( px1.x, 0, w ) ), ( const float* ) src, sstride, sw, sh,
													p.ptr(), nx.ptr(), Math::clamp<ssize_t>(  px2.x + 1, 0, w ) - Math::clamp<ssize_t>( ( px1.x ), 0, w ) );
			}
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}

	void ITransform::applyU8C1( Image& idst, const Image& isrc, const Matrix3f& transform )
	{
		const uint8_t* src;
		uint8_t* dst;
		uint8_t* dst2;
		size_t sstride;
		size_t dstride;
		size_t w, h;
		size_t sw, sh;
		Matrix3f invtrans, invtt;
		Vector2f pt1( 0, 0 ), pt2( 0, 0 );

		SIMD* simd =SIMD::instance();

		dst = idst.map( &dstride );
		dst2 = dst;
		src = isrc.map( &sstride );
		w = idst.width();
		h = idst.height();
		sw = ( ssize_t ) isrc.width();
		sh = ( ssize_t ) isrc.height();

		invtrans = transform.inverse();
		invtt = invtrans.transpose();

		Rectf r( 0 - 1.0f, 0 - 1.0f, sw + 2, sh + 2);
		Vector3f nx = transform * Vector3f( 1.0f, 0.0f, 0.0f );
		size_t ystart = 0, yend = h;
		Vector2f p = invtrans * Vector2f( 0.0f, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( sw, 0.0f );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( 0.0f, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );
		p = invtrans * Vector2f( sw, sh );
		p.y = Math::clamp<float>( p.y, 0, h );
		ystart = Math::min( ystart, ( size_t ) p.y );
		yend = Math::max( yend, ( size_t ) p.y );

		for( size_t y = ystart; y < yend; y++  ) {
			Line2Df l( 0, y, w, y );
			Line2Df l2( invtt * l.vector() );

			if( Clipping::clip( r, l2, pt1, pt2 ) ) {
				Vector2f px1, px2;
				px1 = invtrans * pt1;
				px2 = invtrans * pt2;

				if( px1.x > px2.x ) {
					Vector2f tmp( px1 );
					px1 = px2;
					px2 = tmp;
				}

				Vector3f p = transform * Vector3f( ( float ) Math::clamp<size_t>( ( px1.x ), 0, w ), y, 1.0f );
				simd->warpLinePerspectiveBilinear1u8(  ( dst2 +  Math::clamp<size_t>( px1.x, 0, w ) ), src, sstride, sw, sh,
													p.ptr(), nx.ptr(), Math::clamp<ssize_t>(  px2.x + 1, 0, w ) - Math::clamp<ssize_t>( ( px1.x ), 0, w ) );
			}
			dst2 += dstride;
		}
		idst.unmap( dst );
		isrc.unmap( src );
	}

	void ITransform::applyU8C4( Image& dst, const Image& src, const Matrix3f& transform )
	{
	}


}
