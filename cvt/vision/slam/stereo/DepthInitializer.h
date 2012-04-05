#ifndef CVT_DEPTHINITIALIZER_H
#define CVT_DEPTHINITIALIZER_H

namespace cvt
{
    class DepthInitializer
    {
        public:
            DepthInitializer( const CameraCalibration& c0, const CameraCalibration& c1 );

            /**
             *	\brief triangulate features from two (undistorted) views
             */
            virtual void triangulateFeatures( std::vector<Vector3d> & triangulated,
                                              const std::vector<Vector2f> & avoidPositionsImg0,
                                              const Image& view0, const Image& view1 ) = 0;
        protected:
            const CameraCalibration & _calib0;
            const CameraCalibration & _calib1;
    };

    inline DepthInitializer::DepthInitializer( const CameraCalibration& c0, const CameraCalibration& c1 ) :
        _calib0( c0 ),
        _calib1( c1 )
    {
    }
}

#endif
