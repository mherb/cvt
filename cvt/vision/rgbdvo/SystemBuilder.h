#ifndef CVT_SYSTEMBUILDER_H
#define CVT_SYSTEMBUILDER_H

#include <cvt/vision/rgbdvo/RobustWeighting.h>
#include <cvt/math/Math.h>

namespace cvt {
    template <class EigenMat>
    static bool hasNaN( const EigenMat& mat )
    {
        for( int i = 0; i < mat.rows(); i++ ){
            for( int k = 0; k < mat.cols(); k++ ){
                if( Math::isNaN( mat( i, k ) ) ||
                    Math::isInf( mat( i, k ) ) ){
                    return true;
                }
            }
        }
        return false;
    }

    template <class LossFunc>
    class SystemBuilder
    {
        public:
            SystemBuilder( const LossFunc& lf ) : _lossFunc( lf )
            {
            }

            template <class HessType, class JType>
            float build( HessType& H,
                          JType& b,
                          const JType* jacobians,
                          const float* residuals,
                          size_t n ) const
            {
                // standard: robust lossfunc
                JType jtmp;
                b.setZero();
                H.setZero();

                float ssd = 0.0f;
                for( size_t i = 0; i < n; ++i ){
                    // compute the delta
                    ssd += Math::sqr( residuals[ i ] );

                    float weight = _lossFunc.weight( residuals[ i ] );
                    jtmp = weight * jacobians[ i ];
                    H.noalias() += jtmp.transpose() * jacobians[ i ];
                    b.noalias() += jtmp * residuals[ i ];

                }
                return ssd;
            }

        private:
            const LossFunc& _lossFunc;
    };

    // specialized implementation for non robust lossfunc
    template <>
    template <class HessType, class JType>
    inline float SystemBuilder<NoWeighting<float> >::build( HessType& H, JType& b,
                                                             const JType* jacobians,
                                                             const float* residuals,
                                                             size_t n ) const
    {
        // standard: robust lossfunc
        float ssd = 0.0f;
        b.setZero();
        H.setZero();
        for( size_t i = 0; i < n; ++i ){
            // compute the delta
            ssd += Math::sqr( residuals[ i ] );

            const JType& jtmp = jacobians[ i ];
            H.noalias() += jtmp.transpose() * jtmp;
            b.noalias() += jtmp * residuals[ i ];

        }
        return ssd;
    }

}

#endif // SYSTEMBUILDER_H
