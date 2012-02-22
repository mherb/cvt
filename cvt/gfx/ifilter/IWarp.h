/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_IWARP_H
#define CVT_IWARP_H

#include <cvt/gfx/IFilter.h>
#include <cvt/util/Plugin.h>
#include <cvt/util/PluginManager.h>
#include <cvt/vision/CameraCalibration.h>

namespace cvt {
	class IWarp : public IFilter {
		public:
			IWarp();
			~IWarp();
			void apply( const ParamSet* attribs, IFilterType iftype ) const;

			static void apply( Image& dst, const Image& src, const Image& warp );

			static void warpTunnel( Image& dst, float radius, float cx, float cy );
			static void warpFishEye( Image& idst, float strength, float cx, float cy );
			static void warpUndistort( Image& idst, float k1, float k2, float cx, float cy, float fx, float fy, size_t srcWidth, size_t srcHeight, float k3 = 0, float p1 = 0, float p2 = 0 );


			/**
			  @brief Calculate the warp for each pixel to undistort an image aquired by a calibrated camera.
			  @param idst the warp image
			  @param Knew the new camera matrix for the undistorted image
			  @param srcWidth the width of the original image
			  @param srcHeight the height of the original image
			  @param alpha value between 0 ... 1 to get only valid pixels or all original pixels
			  @param beta value between 0 ... 1 to get only valids pixels or all pixels of the selected rectangle
			 */
			static void warpUndistort( Image& idst, Matrix3f& Knew, const CameraCalibration& calib, size_t srcWidth, size_t srcHeight, float alpha = 0.0f, float beta = 0.0f );

		private:
			static void applyFC1( Image& dst, const Image& src, const Image& warp );
			static void applyFC4( Image& dst, const Image& src, const Image& warp );
			static void applyU8C1( Image& dst, const Image& src, const Image& warp );
			static void applyU8C4( Image& dst, const Image& src, const Image& warp );

			IWarp( const IWarp& t );
	};
}

#endif
