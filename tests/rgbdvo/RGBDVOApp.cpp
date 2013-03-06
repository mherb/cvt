#include <RGBDVOApp.h>

namespace cvt {
    static void demeanNormalize( Image& pp, const Image& input )
    {
        // compute the two values
        float x, xx;
        x = xx = 0.0f;
        {
            IMapScoped<const float> inMap( input );

            size_t h = input.height();
            while( h-- ){
                const float* ptr = inMap.ptr();
                for( size_t i = 0; i < input.width(); i++ ){
                    x  += ptr[ i ];
                    xx += Math::sqr( ptr[ i ] );
                }
                inMap.nextLine();
            }
            x  /= ( input.width() * input.height() ); // mean
            xx /= ( input.width() * input.height() );
        }

        float stdDev = Math::sqrt( xx - Math::sqr( x ) );

        pp = input;
        pp.sub( x );
        pp.mul( 1.0f / stdDev );

        /*
        x = xx = 0.0f;
        {
            IMapScoped<const float> inMap( pp );

            size_t h = pp.height();
            while( h-- ){
                const float* ptr = inMap.ptr();
                for( size_t i = 0; i < pp.width(); i++ ){
                    x  += ptr[ i ];
                    xx += Math::sqr( ptr[ i ] );
                }
                inMap.nextLine();
            }
            x  /= ( input.width() * input.height() ); // mean
            xx /= ( input.width() * input.height() );
        }

        stdDev = Math::sqrt( xx - Math::sqr( x ) );
        std::cout << "Mean: " << x << " stdDev: " << stdDev << std::endl;
        */
    }
}
