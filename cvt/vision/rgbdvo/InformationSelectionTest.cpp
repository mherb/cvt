/*
            CVT - Computer Vision Tools Library
            
     Copyright (c) 2012, Philipp Heise, Sebastian Klose
     
    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#include <cvt/util/CVTTest.h>

#include <cvt/vision/rgbdvo/InformationSelection.h>
#include <Eigen/Core>

#include <vector>

namespace cvt
{
    bool testJ6()
    {
        typedef Eigen::Matrix<float, 6, 1> JacType;

        std::vector<JacType> all( 100 );

        for( size_t x = 0; x < all.size(); x++ ){
            all[ x ].fill( x );
        }

        size_t n = 30;
        InformationSelection<JacType> selector( n );

        std::vector<size_t> expectedIds;
        for( size_t i = 0; i < n; i++ ){
            expectedIds.push_back( all.size() - n + i );
        }

        const std::set<size_t>& ids = selector.selectInformation( &all[ 0 ], all.size() );

        std::set<size_t>::const_iterator it = ids.begin();
        const std::set<size_t>::const_iterator itEnd = ids.end();
        int i = 0;

        while( it != itEnd ){
            if( expectedIds[ i++ ] != *it ) return false;
            it++;
        }

        return true;
    }

BEGIN_CVTTEST( RGBDInfoSelection )

bool result = true;
bool b;

b = testJ6();
CVTTEST_PRINT( "incremental data", b );
result &= b;

return result;

END_CVTTEST

}



