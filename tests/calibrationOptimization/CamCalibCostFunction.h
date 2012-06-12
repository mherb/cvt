#ifndef CAMCALIBCOSTFUNCTION_H
#define CAMCALIBCOSTFUNCTION_H

#include <cvt/math/Vector.h>
#include <cvt/math/SE3.h>
#include <cvt/util/EigenBridge.h>

#include <Eigen/Dense>

namespace cvt
{
    class DistortionFunction
    {
        public:
            DistortionFunction()
            {
                _radial.setZero();
                _tangential.setZero();
            }

            Vector2f undistort( const Vector2f& distortedPoint, const Vector2f& camCenter )
            {
                Vector2f pDiff = distortedPoint - camCenter;
                float r2 = pDiff.lengthSqr();
                float r4 = Math::sqr( r2 );
                float r6 = r4 * r2;

                Vector2f ret = distortedPoint;

                // radial part:
                ret += pDiff * ( _radial.x * r2 + _radial.y * r4 + _radial.z * r6 );

                // tangential part:
                ret.x += _tangential.x * ( r2 + 2.0f * Math::sqr( pDiff.x ) );
                ret.x += 2.0f * _tangential.y * ( pDiff.x * pDiff.y );

                ret.y += _tangential.y * ( r2 + 2.0f * Math::sqr( pDiff.y ) );
                ret.y += 2.0f * _tangential.x * ( pDiff.x * pDiff.y );

                return ret;
            }

            /**
             * \brief get the jacobian of the distortion function for a given point
             *
             */
            void jacobian( Eigen::Matrix<float, 2, 7>& jac, const Vector2f& distortedPoint, const Vector2f& camCenter )
            {
                Vector2f pdiff = distortedPoint - camCenter;
                float r2 = pdiff.lengthSqr();
                float r4 = Math::sqr( r2 );
                float r6 = r4 * r2;

                float dx2 = Math::sqr( pdiff.x );
                float dy2 = Math::sqr( pdiff.y );
                float dxdy = pdiff.x * pdiff.y;

                // kappa1
                jac( 0, 0 ) = pdiff.x * r2;
                jac( 1, 0 ) = pdiff.y * r2;

                // kappa2
                jac( 0, 1 ) = pdiff.x * r4;
                jac( 1, 1 ) = pdiff.y * r4;

                // kappa3
                jac( 0, 2 ) = pdiff.x * r6;
                jac( 1, 2 ) = pdiff.y * r6;

                // tao1
                jac( 0, 3 ) = r2 + 2.0f * dx2;
                jac( 1, 3 ) = 2.0f * dxdy;

                // tao2
                jac( 0, 4 ) = 2.0f * dxdy;
                jac( 1, 4 ) = r2 + 2.0f * dy2;

                float radCoeff0 = -_radial.x * r2 - _radial.y * r4 - _radial.z * r6;
                float radCoeff1 = -2.0f * _radial.x - 4.0f * _radial.y * r2 - 6.0f * _radial.z * r4;

                // cx
                jac( 0, 5 ) = radCoeff0 + dx2 * radCoeff1 - 6.0f * _tangential.x * pdiff.x - 2.0f * _tangential.y * pdiff.y;
                jac( 1, 5 ) = dxdy * radCoeff1 - 2.0f * _tangential.x * pdiff.y - 2.0f * _tangential.y * pdiff.x;

                // cy
                jac( 0, 6 ) = jac( 1, 5 );
                jac( 1, 6 ) = radCoeff0 + dy2 * radCoeff1 - 6.0f * _tangential.y * pdiff.y - 2.0f * _tangential.x * pdiff.x;
            }

            const Vector3f& radial()     const       { return _radial; }
            const Vector2f& tangential() const       { return _tangential; }
            void  setRadial( const Vector3f& v )     { _radial = v; }
            void  setTangential( const Vector2f& v ) { _tangential = v; }

        private:
            Vector3f    _radial;
            Vector2f    _tangential;
    };

    class CamCalibCostFunction
    {
        public:
            CamCalibCostFunction( const std::vector<Vector3f>& boardpts );

            /**
             *  Add another pattern view!
             *  \param imgPts   Vector of the measured 2D positions
             *  \param pose     estimated pattern pose
             */
            void add( const std::vector<Vector2f>& imgPts, const Matrix4f& pose );

            void setInitialK( const Matrix3f& K ){ _K = K; }
            void setInitialDistortions( const Vector3f& rad, const Vector2f& tang );

            void optimize( size_t maxiter );

            Vector2f reprojectionError() const;
            size_t numMeasurements() const { return _samples.size() * _worldPoints.size(); }

        private:
            struct MeasurementInfo
            {
                std::vector<Vector2f> points; /* the measured image positions */
                SE3<float>            patternPose /* the estimated pose of the board */;
            };

            std::vector<Vector3f>           _worldPoints;
            std::vector<MeasurementInfo>    _samples;
            Matrix3f                        _K;

            DistortionFunction              _distortionFunc;

            void evalIntrinsicJac( Eigen::Matrix<float, 2, 4>& jac, const Vector3f& pcam ) const;

            void applyDelta( const Eigen::VectorXf& delta );

    };

    inline CamCalibCostFunction::CamCalibCostFunction( const std::vector<Vector3f>& boardpts ) :
        _worldPoints( boardpts )
    {
    }

    inline void CamCalibCostFunction::add( const std::vector<Vector2f>& imgPts, const Matrix4f& pose )
    {
        _samples.push_back( MeasurementInfo() );
        _samples.back().points = imgPts;
		Eigen::Matrix4f pe;
		EigenBridge::toEigen( pe, pose );
        _samples.back().patternPose.set( pe );
    }

    inline void CamCalibCostFunction::setInitialDistortions( const Vector3f& rad, const Vector2f& tang )
    {
        _distortionFunc.setRadial( rad );
        _distortionFunc.setTangential( tang );
    }

    inline void CamCalibCostFunction::optimize( size_t maxiter )
    {
        // the jacobian for a single board
        Eigen::Matrix<float, 2, 15>  jSingle;
        Eigen::Matrix<float, 2, 6>   jSinglePose;
        Eigen::Matrix<float, 2, 4>   jSingleIntrinsic;
        Eigen::Matrix<float, 2, 7>   jSingleDistortion;

        Eigen::Matrix<float, 15, 15> jtjSum;
        Eigen::Matrix<float, 15, 1>  jtResSum;

        //Vector2f lastError = reprojectionError();

        static const size_t numBoardPts = _worldPoints.size();
        Matrix4f pose;
        Vector3f pointInCam;
        Vector2f undistorted;

        Eigen::Matrix3f intrinsics;
        Eigen::Vector3f currentPoint;
        Vector2f center;

        Eigen::Vector2f residual;

        size_t nParams = _samples.size() * 6 + 9;
        Eigen::MatrixXf jtjFull( nParams, nParams );
        Eigen::VectorXf jtResFull( nParams, 1 );
        Eigen::VectorXf delta( nParams, 1 );

        size_t iter = 0;

        while( iter++ < maxiter ){
            // get the current intrinsics into eigen
            EigenBridge::toEigen( intrinsics, _K );
            center.x = _K[ 0 ][ 2 ];
            center.y = _K[ 1 ][ 2 ];

            jtjFull.setZero();
            jtResFull.setZero();
            delta.setZero();

            // for each board
            for( size_t i = 0; i < _samples.size(); i++ ){
                jtjSum.setZero();
                jtResSum.setZero();

                MeasurementInfo & meas = _samples[ i ];
                // get the current Transformation:
                EigenBridge::toCVT( pose, meas.patternPose.transformation() );
                for( size_t p = 0; p < numBoardPts; p++ ){
                    Vector2f& distorted  = meas.points[ p ];
                    Vector3f& worldPoint = _worldPoints[ p ];
                    pointInCam  = pose * worldPoint;
                    undistorted = _distortionFunc.undistort( distorted, center );                    

                    EigenBridge::toEigen( currentPoint, pointInCam );

                    Vector2f rep( _K * pointInCam );
                    EigenBridge::toEigen( residual, ( rep - undistorted ) );

                    // evaluate the jacobians for this measurement:
                    meas.patternPose.screenJacobian( jSinglePose, currentPoint, intrinsics );
                    evalIntrinsicJac( jSingleIntrinsic, pointInCam );
                    _distortionFunc.jacobian( jSingleDistortion, distorted, center );

                    // fill the complete jacobian
                    jSingle.block( 0, 0, 2, 4 ) = jSingleIntrinsic;
                    // cx
                    jSingle( 0, 2 ) -= jSingleDistortion( 0, 5 );
                    jSingle( 0, 3 ) -= jSingleDistortion( 0, 6 );
                    jSingle( 1, 2 ) -= jSingleDistortion( 1, 5 );
                    jSingle( 1, 3 ) -= jSingleDistortion( 1, 6 );
                    jSingle.block( 0, 4, 2, 5 ) = -jSingleDistortion.block( 0, 0, 2, 5 );
                    jSingle.block( 0, 9, 2, 6 ) = jSinglePose;

                    jtjSum += jSingle.transpose() * jSingle;
                    jtResSum += jSingle.transpose() * residual;
                }

                // copy stuff to the big matrix:
                // intrinsic and distortion part
                size_t idx = 9 + i * 6;
                jtjFull.block( 0, 0, 9, 9 )     += jtjSum.block( 0, 0, 9, 9 );
                jtjFull.block( idx, 0, 6, 9 )   += jtjSum.block( 9, 0, 6, 9 );
                jtjFull.block( 0, idx, 9, 6 )   += jtjSum.block( 0, 9, 9, 6 );
                jtjFull.block( idx, idx, 6, 6 ) += jtjSum.block( 9, 9, 6, 6 );

                jtResFull.block( 0, 0, 9, 1 )   += jtResSum.block( 0, 0, 9, 1 );
                jtResFull.block( idx, 0, 6, 1 ) += jtResSum.block( 9, 0, 6, 1 );
            }

            //std::cout << jtjFull << std::endl;
            //std::cout << "\n\n" << jtResFull << std::endl;
            delta = jtjFull.ldlt().solve( -jtResFull );

            std::cout << delta << std::endl;
            getchar();

            // apply the parameters and go on
            applyDelta( delta );
        }
    }

    inline Vector2f CamCalibCostFunction::reprojectionError() const
    {
        Vector2f err( 0.0f, 0.0f );
        Vector3f tmp;
        Vector2f rep;
        Matrix4f pose;
        size_t npts = _worldPoints.size();
        for( size_t i = 0; i < _samples.size(); i++ ){
            const MeasurementInfo & meas = _samples[ i ];
            EigenBridge::toCVT( pose, meas.patternPose.transformation() );
            for( size_t p = 0; p < npts; p++ ){
                tmp = pose * _worldPoints[ p ];
                tmp = _K * tmp;
                rep.x = tmp.x / tmp.z;
                rep.y = tmp.y / tmp.z;

                rep -= meas.points[ p ];

                err.x += Math::abs( rep.x );
                err.y += Math::abs( rep.y );
            }
        }

        return err;
    }

    inline void CamCalibCostFunction::evalIntrinsicJac( Eigen::Matrix<float, 2, 4>& jac, const Vector3f& pcam ) const
    {
        jac( 0, 0 ) = pcam.x / pcam.z; jac( 0, 1 ) = 0; jac( 0, 2 ) = 1; jac( 0, 3 ) = 0;
        jac( 0, 0 ) = 0; jac( 1, 1 ) = pcam.y / pcam.z; jac( 1, 2 ) = 0; jac( 1, 3 ) = 1;
    }

    inline void CamCalibCostFunction::applyDelta( const Eigen::VectorXf& delta )
    {
        // update the intrinsics from first four params
        size_t idx = 0;
        _K[ 0 ][ 0 ] += delta[ idx++ ];
        _K[ 1 ][ 1 ] += delta[ idx++ ];
        _K[ 0 ][ 2 ] += delta[ idx++ ];
        _K[ 1 ][ 2 ] += delta[ idx++ ];

        // update the distortion parameters:
        Vector3f kappa = _distortionFunc.radial();
        Vector2f tao   = _distortionFunc.tangential();
        kappa[ 0 ] += delta[ idx++ ];
        kappa[ 1 ] += delta[ idx++ ];
        kappa[ 2 ] += delta[ idx++ ];
        tao[ 0 ] += delta[ idx++ ];
        tao[ 1 ] += delta[ idx++ ];

        _distortionFunc.setRadial( kappa );
        _distortionFunc.setTangential( tao );

        SE3<float>::ParameterVectorType poseUpdate;
        for( size_t i = 0; i < _samples.size(); i++ ){
            MeasurementInfo& measInfo = _samples[ i ];
            poseUpdate = delta.block( idx, 0, 6, 1 );
            idx += 6;
            measInfo.patternPose.apply( poseUpdate );
        }
    }
}

#endif // CAMCALIBCOSTFUNCTION_H
