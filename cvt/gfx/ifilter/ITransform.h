#ifndef CVT_ITRANSFORM_H
#define CVT_ITRANSFORM_H

#include <cvt/gfx/IFilter.h>
#include <cvt/util/Plugin.h>
#include <cvt/util/PluginManager.h>

namespace cvt {
	class ITransform : public IFilter {
		public:
			ITransform();
			~ITransform();
			void apply( const ParamSet* attribs, IFilterType iftype ) const;

			/**
			 *	@param	dst			the destination image
			 *	@param	src			the source image
			 *	@param	transform	transformation from src to dst!
			 */
			static void apply( Image& dst, const Image& src, const Matrix3f& transform );
			static void apply( Image& dst, const Image& src, const Matrix3f& transform, const Matrix3f& itransform );

		private:
			static void applyFC1( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv );
			static void applyFC4( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv );
			static void applyU8C1( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv );
			static void applyU8C4( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv );

			ITransform( const ITransform& t );
	};
}

#endif
