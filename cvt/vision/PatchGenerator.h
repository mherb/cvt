/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_PATCH_GENERATOR
#define CVT_PATCH_GENERATOR

#include <cvt/gfx/Image.h>
#include <cvt/math/Math.h>
#include <cvt/util/Range.h>
#include <cvt/util/RNG.h>
#include <Eigen/Core>


namespace cvt
{
	class PatchGenerator
	{
		public:
			PatchGenerator( const Rangef & angleRange, const Rangef & scaleRange, uint32_t patchSize = 32, double whiteNoiseSigma = 5.0 );
			~PatchGenerator();
			
			/* generate the next patch */
			void next( Image & outputPatch, const cvt::Image & inputImage, const Vector2f & patchCenter );			
			
		private:
			uint32_t			_patchSize;
			Rangef				_angleRange;
			Rangef				_scaleRange;
			double				_whiteNoiseSigma;			
			Eigen::Matrix2f		_affine;
			RNG					_rng;
			size_t				_inHandle, _outHandle;
			
			void randomizeAffine();		
	};	
}

#endif
