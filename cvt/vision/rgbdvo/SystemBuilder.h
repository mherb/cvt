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
            size_t build( HessType& H,
                          JType& b,
                          const JType* jacobians,
                          const float* residuals,
                          const std::vector<size_t>& indices,
                          float & ssd ) const
            {
                // standard: robust lossfunc
                ssd = 0;
                size_t numPixels = 0;
                JType jtmp;
                b.setZero();
                H.setZero();

                for( size_t i = 0; i < indices.size(); i++ ){
                    // compute the delta
                    size_t idx = indices[ i ];
                    ssd += Math::sqr( residuals[ idx ] );
                    numPixels++;

                    float weight = _lossFunc.weight( residuals[ idx ] );
                    jtmp = weight * jacobians[ idx ];

                    /*
                    if( hasNaN( jtmp ) ){
                        std::cout << "jtmp has nan value(s)" << std::endl;
                        std::cout << "idx: " << idx << std::endl;
                        std::cout << "i: " << i << std::endl;
                        std::cout << "weight: " << weight << std::endl;
                        std::cout << "residual: " << residuals[idx] << std::endl;
                        getchar();
                    }
                    if( hasNaN( jacobians[ idx ] ) ){
                        std::cout << "jacobian has nan value(s)" << std::endl;
                        std::cout << jacobians[ idx ] << std::endl;
                        getchar();
                    }*/

                    H.noalias() += jtmp.transpose() * jacobians[ idx ];
                    b.noalias() += jtmp * residuals[ idx ];

                    /*
                    if( hasNaN( H ) ){
                        std::cout << "hessian has nan value(s)" << std::endl;
                        std::cout << "Before: \n" << bkp << std::endl;
                        std::cout << "Now: \n" << H << std::endl;
                        std::cout << "jac: \n" << jacobians[ idx ] << std::endl;
                        std::cout << "jtmp: \n" << jtmp << std::endl;
                        std::cout << "idx: " << idx << std::endl;
                        std::cout << "i: " << i << std::endl;
                        std::cout << "numindices: " << indices.size() << std::endl;
                        std::cout << "weight: " << weight << std::endl;
                        std::cout << "residual: " << residuals[idx] << std::endl;
                        getchar();
                    }*/

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
