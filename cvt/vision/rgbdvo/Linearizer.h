#ifndef CVT_LINEARIZER_H
#define CVT_LINEARIZER_H

#include <vector>
#include <cvt/vision/ImagePyramid.h>

namespace cvt {

    // inverse compositional linearizer
    template <class AlignData>
    class InvCompLinearizer
    {
            typedef typename AlignData::JacobianVec JacobianVecType;
        public:
            InvCompLinearizer( const IKernel&, const IKernel&, size_t /*octaves*/, float /*scale*/ ){}

            void recomputeJacobians( JacobianVecType& jacobians,
                                     std::vector<float>& residuals,
                                     const std::vector<Vector2f>& /*warpedPts*/,
                                     const std::vector<float>& interpolated,
                                     const AlignData& data,
                                     size_t /*octave*/ ) const
            {
                // sort out bad pixels (out of image)
                const JacobianVecType& refJacs = data.jacobians();
                size_t savePos = 0;
                for( size_t i = 0; i < data.size(); ++i ){
                    if( interpolated[ i ] >= 0.0f ){
                        jacobians[ savePos ] = refJacs[ i ];
                        residuals[ savePos ] = residuals[ i ];
                        ++savePos;
                    }
                }
                residuals.erase( residuals.begin() + savePos, residuals.end() );
                jacobians.erase( jacobians.begin() + savePos, jacobians.end() );
            }

            void updateOnlineData( const ImagePyramid&, const Image& ) {}
    };

    template <class AlignData>
    class FwdCompLinearizer {
            typedef typename AlignData::JacobianVec     JacobianVecType;
            typedef typename AlignData::ScreenJacVec    ScreenJacVecType;
            typedef typename AlignData::GradientType    GradientType;
            typedef typename AlignData::WarpType        WarpType;

        public:
            FwdCompLinearizer( const IKernel& kdx, const IKernel& kdy, size_t octaves, float scale ) :
                _onlineGradientsX( octaves, scale ),
                _onlineGradientsY( octaves, scale ),
                _kx( kdx ), _ky( kdy )
            {}

            void recomputeJacobians( JacobianVecType& jacobians,
                                     std::vector<float>& residuals,
                                     const std::vector<Vector2f>& warpedPts,
                                     const std::vector<float>& interpolated,
                                     const AlignData& data,
                                     size_t octave ) const
            {
                std::vector<float> intGradX;
                std::vector<float> intGradY;
                size_t n = data.size();
                intGradX.resize( n );
                intGradY.resize( n );
                // evaluate the gradients at the warped positions
                SIMD* simd = SIMD::instance();
                AlignData::interpolateGradients( intGradX, _onlineGradientsX[ octave ], warpedPts, simd );
                AlignData::interpolateGradients( intGradY, _onlineGradientsY[ octave ], warpedPts, simd );

                // sort out bad pixels (out of image)
                const ScreenJacVecType& sj = data.screenJacobians();
                GradientType grad;
                size_t savePos = 0;

                for( size_t i = 0; i < n; ++i ){
                    if( interpolated[ i ] >= 0.0f ){
                        grad.coeffRef( 0, 0 ) = intGradX[ i ];
                        grad.coeffRef( 0, 1 ) = intGradY[ i ];
                        // compute the Fwd jacobians
                        WarpType::computeJacobian( jacobians[ savePos ], sj[ i ], grad, interpolated[ i ] );
                        residuals[ savePos ] = residuals[ i ];
                        ++savePos;
                    }
                }
                residuals.erase( residuals.begin() + savePos, residuals.end() );
                jacobians.erase( jacobians.begin() + savePos, jacobians.end() );
            }

            void updateOnlineData( const ImagePyramid& pyrf, const Image& /*depth*/ )
            {
                // compute the gradients of the input
                pyrf.convolve( _onlineGradientsX, this->_kx );
                pyrf.convolve( _onlineGradientsY, this->_ky );
            }

        protected:
            ImagePyramid    _onlineGradientsX;
            ImagePyramid    _onlineGradientsY;
            IKernel         _kx;
            IKernel         _ky;
    };

    template <class AlignData>
    class ESMLinearizer : public FwdCompLinearizer<AlignData>
    {
            typedef typename AlignData::JacobianType    JacobianType;
            typedef typename AlignData::JacobianVec     JacobianVecType;
            typedef typename AlignData::ScreenJacVec    ScreenJacVecType;
            typedef typename AlignData::GradientType    GradientType;
            typedef typename AlignData::WarpType        WarpType;

        public:
            ESMLinearizer( const IKernel& kdx, const IKernel& kdy, size_t octaves, float scale ) :
                FwdCompLinearizer<AlignData>( kdx, kdy, octaves, scale )
            {
            }

            void recomputeJacobians( JacobianVecType& jacobians,
                                     std::vector<float>& residuals,
                                     const std::vector<Vector2f>& warpedPts,
                                     const std::vector<float>& interpolated,
                                     const AlignData& data,
                                     size_t octave ) const
            {
                std::vector<float> intGradX, intGradY;
                size_t n = data.size();
                intGradX.resize( n );
                intGradY.resize( n );

                // evaluate the gradients at the warped positions
                SIMD* simd = SIMD::instance();
                AlignData::interpolateGradients( intGradX, this->_onlineGradientsX[ octave ], warpedPts, simd );
                AlignData::interpolateGradients( intGradY, this->_onlineGradientsY[ octave ], warpedPts, simd );

                // sort out bad pixels (out of image)
                const JacobianVecType& refJacs = data.jacobians();
                const ScreenJacVecType& sj = data.screenJacobians();
                GradientType grad;
                size_t savePos = 0;

                JacobianType jCur;
                for( size_t i = 0; i < n; ++i ){
                    if( interpolated[ i ] >= 0.0f ){
                        grad.coeffRef( 0, 0 ) = intGradX[ i ];
                        grad.coeffRef( 0, 1 ) = intGradY[ i ];
                        // compute the ESM jacobians
                        WarpType::computeJacobian( jCur, sj[ i ], grad, interpolated[ i ] );
                        jacobians[ savePos ] = 0.5f * ( refJacs[ i ] + jCur );
                        residuals[ savePos ] = residuals[ i ];
                        ++savePos;
                    }
                }
                residuals.erase( residuals.begin() + savePos, residuals.end() );
                jacobians.erase( jacobians.begin() + savePos, jacobians.end() );
            }
    };


}

#endif // LINEARIZER_H
