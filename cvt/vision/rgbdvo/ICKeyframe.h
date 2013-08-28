/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_ICKEYFRAME_H
#define CVT_ICKEYFRAME_H

#include <cvt/vision/rgbdvo/RGBDKeyframe.h>

namespace cvt
{
	template <class AlignData>
	class ICKeyframe : public RGBDKeyframe<AlignData> {
        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			typedef RGBDKeyframe<AlignData>             Base;
            typedef float                               T;
            typedef typename Base::JacobianType         JacobianType;
            typedef typename Base::ScreenJacobianType   ScreenJacobianType;
            typedef typename Base::JacobianVec          JacobianVec;            
            typedef typename Base::GradientType         GradientType;

            ICKeyframe( const Matrix3f &K, size_t octaves, float scale );
            ~ICKeyframe();

            void recompute( std::vector<float>& residuals,
                            JacobianVec& jacobians,
							const typename Base::WarpType& warp,
                            const IMapScoped<const float>& gray,
                            size_t octave );
        private:
            InvCompLinearizer<AlignData> _linearizer;
    };

	template <class AlignData>
	inline ICKeyframe<AlignData>::ICKeyframe( const Matrix3f &K, size_t octaves, float scale ) :
		RGBDKeyframe<AlignData>( K, octaves, scale ),
		_linearizer( this->_kx, this->_ky, octaves, scale )
    {
    }

	template <class AlignData>
	inline ICKeyframe<AlignData>::~ICKeyframe()
    {
    }

	template <class AlignData>
	inline void ICKeyframe<AlignData>::recompute( std::vector<float>& residuals,
												  JacobianVec& jacobians,
												  const typename Base::WarpType& warp,
												  const IMapScoped<const float>& gray,
												  size_t octave )
    {
        SIMD* simd = SIMD::instance();
        const size_t width = gray.width();
        const size_t height = gray.height();
        std::vector<Vector2f> warpedPts;
        std::vector<float> interpolatedPixels;

		const AlignData& data = this->dataForScale( octave );
        size_t n = data.size();

        // construct the projection matrix
        Matrix4f projMat( data.intrinsics() );
        projMat *= warp.pose();

        // resize the data storage
        warpedPts.resize( n );
        interpolatedPixels.resize( n );
        residuals.resize( n );
        jacobians.resize( n );

        // project the points:
        simd->projectPoints( &warpedPts[ 0 ], projMat, &data.points()[ 0 ], n );

        // interpolate the pixel values
        simd->warpBilinear1f( &interpolatedPixels[ 0 ], &warpedPts[ 0 ].x, gray.ptr(), gray.stride(), width, height, -10.0f, n );

        // compute the residuals
        warp.computeResiduals( &residuals[ 0 ], &data.pixels()[ 0 ], &interpolatedPixels[ 0 ], n );

        // compute the jacobians
        _linearizer.recomputeJacobians( jacobians, residuals, warpedPts, interpolatedPixels, data, octave );
    }
}


#endif // ICKEYFRAME_H
