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
                          const std::vector<size_t>& indices,
                          float & ssd ) const
            {
                // standard: assume robust lossfunc
                ssd = 0;
                size_t numPixels = 0;
                JType jtmp;
                b.setZero();
                H.setZero();

                for( size_t i = 0; i < indices.size(); i++ ){
                    // compute the delta
                    ssd += Math::sqr( residuals[ indices[ i ] ] );
                    numPixels++;

                    float weight = _lossFunc.weight( residuals[ indices[ i ] ] );
                    jtmp = weight * jacobians[ indices[ i ] ];

                    H.noalias() += jtmp.transpose() * jacobians[ indices[ i ] ];
                    b.noalias() += jtmp * residuals[ indices[ i ] ];

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
                                                             const std::vector<size_t>& indices,
                                                             float & ssd ) const
    {
        ssd = 0;
        size_t numPixels = 0;
        b.setZero();
        for( size_t i = 0; i < indices.size(); i++ ){
            // compute the delta
            ssd += Math::sqr( residuals[ indices[ i ] ] );
            numPixels++;
            b.noalias() += jacobians[ indices[ i ] ] * residuals[ indices[ i ] ];

        }
        return numPixels;
    }

}

#endif // SYSTEMBUILDER_H
