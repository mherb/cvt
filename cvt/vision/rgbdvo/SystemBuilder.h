#ifndef CVT_SYSTEMBUILDER_H
#define CVT_SYSTEMBUILDER_H

#include <cvt/vision/rgbdvo/RobustWeighting.h>

namespace cvt {

    template <class LossFunc>
    class SystemBuilder
    {
        public:
            SystemBuilder( const LossFunc& lf ) : _lossFunc( lf )
            {
            }

            template <class HessType, class JType>
            size_t build( HessType& H, JType& b,
                          const JType* jacobians,
                          const float* residuals,
                          float & ssd,
                          size_t n ) const
            {
                // standard: assume robust lossfunc
                ssd = 0;
                size_t numPixels = 0;
                JType jtmp;
                b.setZero();
                H.setZero();
                for( size_t i = 0; i < n; i++ ){
                    // compute the delta

                    ssd += Math::sqr( residuals[ i ] );
                    numPixels++;

                    float weight = _lossFunc.weight( residuals[ i ] );
                    jtmp = weight * jacobians[ i ];

                    H.noalias() += jtmp.transpose() * jacobians[ i ];
                    b.noalias() += jtmp * residuals[ i ];

                }
                return numPixels;
            }

        private:
            const LossFunc& _lossFunc;
    };

    // specialized implementation for non robust lossfunc
    template <>
    template <class HessType, class JType>
    inline size_t SystemBuilder<NoWeighting<float> >::build( HessType&, JType& b,
                                                             const JType* jacobians,
                                                             const float* residuals,
                                                             float & ssd,
                                                             size_t n ) const
    {
        ssd = 0;
        size_t numPixels = 0;
        b.setZero();
        for( size_t i = 0; i < n; i++ ){
            // compute the delta
            ssd += Math::sqr( residuals[ i ] );
            numPixels++;
            b.noalias() += jacobians[ i ] * residuals[ i ];

        }
        return numPixels;
    }

}

#endif // SYSTEMBUILDER_H
