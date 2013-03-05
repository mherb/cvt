#ifndef CVT_BATCHEVALUATION_H
#define CVT_BATCHEVALUATION_H

#include <cvt/math/Quaternion.h>
#include <cvt/math/Matrix.h>

namespace cvt {

    class BatchEvaluation {
        public:
            BatchEvaluation(){}

            void add( double time, const Matrix4f& computed, const Matrix4f& gt )
            {
                _data.push_back( PosePair( time, computed, gt ) );
            }

            void writeComputedFile( const String& filename ) const
            {
                std::ofstream file;
                file.open( filename.c_str() );

                std::vector<PosePair>::const_iterator it = _data.begin();
                const std::vector<PosePair>::const_iterator itEnd = _data.end();

                file.precision( 15 );
                while( it != itEnd ){
                    file << std::fixed
                         << it->timeStamp << " "
                         << it->tEstimated.x << " "
                         << it->tEstimated.y << " "
                         << it->tEstimated.z << " "
                         << it->qEstimated.x << " "
                         << it->qEstimated.y << " "
                         << it->qEstimated.z << " "
                         << it->qEstimated.w << "\n";
                    ++it;
                }
                file.close();
            }

            void writeEstimatedFile( const String& filename ) const
            {
                std::ofstream file;
                file.open( filename.c_str() );

                std::vector<PosePair>::const_iterator it = _data.begin();
                const std::vector<PosePair>::const_iterator itEnd = _data.end();

                file << "# <time_stamp> "
                     << " <tx_estimated> "
                     << " <ty_estimated> "
                     << " <tz_estimated> "
                     << " <q_x_estimated> "
                     << " <q_y_estimated> "
                     << " <q_z_estimated> "
                     << " <q_w_estimated> "
                     << " <tx_gt> "
                     << " <ty_gt> "
                     << " <tz_gt> "
                     << " <q_x_gt> "
                     << " <q_y_gt> "
                     << " <q_z_gt> "
                     << " <q_w_gt> "
                     << " <delta_rot_euler_x> "
                     << " <delta_rot_euler_y> "
                     << " <delta_rot_euler_z> "
                     << " <delta_t_x> "
                     << " <delta_t_y> "
                     << " <delta_t_z> "
                     << std::endl;

                file.precision( 15 );
                while( it != itEnd ){
                    const PosePair& d = *it;
                    Quaternionf qerr( d.errorMat.toMatrix3() );
                    Vector3f euler = qerr.toEuler();
                    Vector3f dt = d.errorMat.col( 3 );
                    file << std::fixed
                         << d.timeStamp << " "
                         << d.tEstimated.x << " "
                         << d.tEstimated.y << " "
                         << d.tEstimated.z << " "
                         << d.qEstimated.x << " "
                         << d.qEstimated.y << " "
                         << d.qEstimated.z << " "
                         << d.qEstimated.w << " "
                         << d.tGroundTruth.x << " "
                         << d.tGroundTruth.y << " "
                         << d.tGroundTruth.z << " "
                         << d.qGroundTruth.x << " "
                         << d.qGroundTruth.y << " "
                         << d.qGroundTruth.z << " "
                         << d.qGroundTruth.w << " "                            
                         << euler.x << " "
                         << euler.y << " "
                         << euler.z << " "
                         << dt.x << " "
                         << dt.y << " "
                         << dt.z << " "
                         << "\n";
                    ++it;
                }
                file.close();
            }

            void evalRPEPerSecond()
            {
                Matrix4f deltaError;
                float meanT = 0.0f;
                float meanR = 0.0f;
                size_t n = 0;
                for( size_t i = 0; i < _data.size(); i++ ){
                    PosePair& d0 = _data[ i ];
                    size_t second = findClosestIndex( d0.timeStamp + 1.0, i );
                    const PosePair& d1 = _data[ second ];

                    double deltaT = d1.timeStamp - d0.timeStamp;
                    if( i == second ){
                        d0.transDriftPerSec = 0.0f;
                        d0.rotDriftPerSec = 0.0f;
                    } else {
                        // compute the error change
                        // std::cout << "DeltaT = " << deltaT << std::endl;
                        deltaError = d0.errorMat.inverse() * d1.errorMat;
                        d0.rotDriftPerSec = computeAngleError( deltaError ) / deltaT;
                        d0.transDriftPerSec = Math::sqrt( deltaError.col( 3 ).lengthSqr() - 1 ) / deltaT;

                        meanR += d0.rotDriftPerSec;
                        meanT += Math::abs( d0.transDriftPerSec );

                        n++;
                    }
                }

                meanT /= n;
                meanR /= n;
                std::cout << "Mean Translation drift (m/s):\t" << meanT << std::endl;
                std::cout << "Mean Rotation drift (deg/s):\t" << Math::rad2Deg( meanR ) << std::endl;
            }

        private:
            struct PosePair {
                PosePair( double t,
                          const Matrix4f& poseComp,
                          const Matrix4f& poseGT ) :
                    qEstimated( poseComp.toMatrix3() ),
                    qGroundTruth( poseGT.toMatrix3() ),
                    tEstimated( poseComp.col( 3 ) ),
                    tGroundTruth( poseGT.col( 3 ) ),
                    timeStamp( t ),
                    rotDriftPerSec( -1.0f ),
                    transDriftPerSec( -1.0f )
                {
                    // compute the error Matrix
                    errorMat = poseGT.inverse() * poseComp;
                }

                Quaternionf qEstimated;
                Quaternionf qGroundTruth;
                Vector3f    tEstimated;
                Vector3f    tGroundTruth;
                double      timeStamp;

                Matrix4f    errorMat;
                float       rotDriftPerSec;
                float       transDriftPerSec;
            };

            std::vector<PosePair>   _data;

            size_t findClosestIndex( double stamp, size_t start = 0 ) const
            {
                double best = Math::abs( stamp - _data[ start ].timeStamp );
                size_t bestIdx = start;
                start++;
                while( start < _data.size() ){
                    double curr = Math::abs( _data[ start ].timeStamp - stamp );
                    if( curr < best ){
                        best = curr;
                        bestIdx = start;
                    } else {
                        return bestIdx;
                    }
                    start++;
                }
                return bestIdx;
            }

            float computeAngleError( const Matrix4f& m )
            {
                return Math::acos( Math::clamp( 0.5f * ( m.toMatrix3().trace() - 1.0f ), -1.0f, 1.0f ) );
            }

    };

}

#endif // CVT_BATCHEVALUATION_H
