/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */

#ifndef CVT_TSDFVOLUME_H
#define CVT_TSDFVOLUME_H

#include <cvt/cl/CLKernel.h>
#include <cvt/math/Matrix.h>
#include <cvt/util/String.h>

namespace cvt
{
	class TSDFVolume
	{
		public:
			TSDFVolume( const Matrix4f& gridtoworld, size_t width, size_t height, size_t depth, float truncation = 5.0f );

			void clear();
			void addDepthMap( const Matrix4f& proj, const Image& depthmap, float scale );
			void addDepthMap( const Matrix3f& intrinsics, const Matrix4f& extrinsics, const Image& depthmap, float scale );

			size_t width() const { return _width; }
			size_t height() const { return _height; }
			size_t depth() const { return _depth; }
			/*
			   o save or map-data
			   o to SceneMesh / GLMesh using MC
			   o addDepthNormalMap
			   o getDepthMap for intrinsics/extrinsics ...
			   o add loadRaw with width, height, depth
			 */

			void saveRaw( const String& path ) const;

		private:
			size_t	 _width;
			size_t	 _height;
			size_t	 _depth;
			float	 _trunc;
			Matrix4f _g2w;
			CLBuffer _clvolume;
			CLBuffer _clproj;
			CLKernel _clvolclear;
			CLKernel _clvoladd;
	};

}

#endif
