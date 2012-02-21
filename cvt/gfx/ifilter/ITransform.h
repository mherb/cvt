/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
			static void apply( Image& dst, const Image& src, const Matrix3f& transform, size_t width = 0, size_t height = 0 );
			static void apply( Image& dst, const Image& src, const Matrix3f& transform, const Matrix3f& itransform, size_t width = 0, size_t height = 0 );

		private:
			static void applyFC1( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv );
			static void applyFC4( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv );
			static void applyU8C1( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv );
			static void applyU8C4( Image& dst, const Image& src, const Matrix3f& T, const Matrix3f& Tinv );

			ITransform( const ITransform& t );
	};
}

#endif
