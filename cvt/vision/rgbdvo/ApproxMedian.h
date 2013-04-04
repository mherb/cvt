#ifndef CVT_APPROXMEDIAN_H
#define CVT_APPROXMEDIAN_H

#include <vector>

namespace cvt {

    class ApproxMedian
    {
        public:
            ApproxMedian( float min, float max, float resolution ) :
                _min( min ),
                _max( max ),
                _range( _max - _min ),
                _resolution( resolution )
            {
                size_t nBins = _range / resolution;
                _hist.resize( nBins, 0 );
            }

            void add( float value )
            {
                value = Math::clamp( value, _min, _max );
                float fidx =  value / ( _range ) * ( float ) ( _hist.size() - 1 ) + 0.5f;
                int idx = ( int ) fidx;

//                if( idx < 0 || idx >= _hist.size() )
//                    std::cout << "value: " << value << " idx = " << idx << std::endl;
                _hist[ idx ] += 1;
            }

            // approximate the nth value
            float approximateNth( size_t nth )
            {
                size_t bin = 0;
                size_t num = _hist[ bin++ ];

                while( num < nth ){
                    num += _hist[ bin++ ];
                }
                bin--;
                size_t nPrev = num - _hist[ bin ];

                if( bin )
                    bin--;

                // previous is smaller:
                float frac = ( nth - nPrev ) / ( float )( num - nPrev );

                return ( bin + frac ) * _resolution;
            }

            void clearHistogram()
            {
                for( size_t i = 0; i < _hist.size(); i++ ){
                    _hist[ i ] = 0;
                }
            }

        private:
            float               _min;
            float               _max;
            float               _range;
            float               _resolution;
            std::vector<size_t> _hist;
    };

}

#endif // APPROXMEDIAN_H
