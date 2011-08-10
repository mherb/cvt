#include <cvt/vision/Vision.h>

#include <cvt/util/CVTTest.h>
#include <cvt/math/Matrix.h>

namespace cvt {

    template <typename T>
    static bool _compare( const Matrix3<T> & a, const Matrix3<T> & b, T epsilon )
    {
        for( size_t i = 0; i < 3; i++ ){
            for( size_t k = 0; k < 3; k++ ){
                if( Math::abs( a[ i ][ k ] - b[ i ][ k ] ) > epsilon )
                    return false;
            }
        }

        return true;
    }

    template <typename T>
    static bool _compareVectors( const Vector3<T> & a, const Vector3<T> & b, T epsilon )
    {
        Vector3<T> a0 = a;
        a0.normalize();

        Vector3<T> b0 = b;
        b0.normalize();

        if( Math::abs( a0[ 0 ] - b0[ 0 ] ) > epsilon )
            return false;
        if( Math::abs( a0[ 1 ] - b0[ 1 ] ) > epsilon )
            return false;
        if( Math::abs( a0[ 2 ] - b0[ 2 ] ) > epsilon )
            return false;
        return true;
    }

    template <typename T>
    static bool _essentialTest()
    {
        bool result = true;

        Matrix3<T> K( 230.0,   0.0, 320.0,
                        0.0, 235.0, 240.0,
                        0.0,   0.0,   1.0 );

        Matrix3<T> R, tSkew;

        srandom( time( NULL ) );

        for( size_t i = 0; i < 100; i++ ){
            R.setRotationXYZ( Math::rand( ( T )-Math::PI/6.0, ( T )Math::PI/6.0 ),
                              Math::rand( ( T )-Math::PI/6.0, ( T )Math::PI/6.0 ),
                              Math::rand( ( T )-Math::PI/6.0, ( T )Math::PI/6.0 ));

            Vector3<T> t( Math::rand( ( T )-1000, ( T )1000 ),
                          Math::rand( ( T )-1000, ( T )1000 ),
                          Math::rand( ( T )-1000, ( T )1000 ) );

            tSkew.setSkewSymmetric( t );


            Matrix3<T> E = tSkew * R;
            E *= 1.0 / E[ 2 ][ 2 ];

            Matrix3<T> R0, R1;
            Vector3<T> t0, t1;
            Vision::decomposeEssential( R0, R1, t0, t1, E );


            bool b = true;

            b = _compare( R, R0, (T)0.0001 ) || _compare( R, R1, (T)0.0001 );
            result &= b;
            if( !b ){
                std::cout << "Ground Truth Rotation: \n" << R << std::endl;
                std::cout << "Decomposed R0: \n"            << R0 << std::endl;
                std::cout << "Decomposed R1: \n"            << R1 << std::endl;
            }

            b = _compareVectors( t, t0, (T)0.0001 ) || _compareVectors( t, t1, (T)0.0001 );
            result &= b;

            if( !b ){
                std::cout << "Ground Truth Translation: \n" << t << std::endl;
                std::cout << "Decomposed t0: \n" << t0 << std::endl;
                std::cout << "Decomposed t1: \n" << t1 << std::endl;
            }
        }

        return result;
    }

BEGIN_CVTTEST( Vision )
    bool testResult = true;
    bool b;


    b = _essentialTest<float>();
    CVTTEST_PRINT( "float: decomposeEssential()\t", b );
    testResult &= b;

    b = _essentialTest<double>();
    CVTTEST_PRINT( "double: decomposeEssential()\t", b );
    testResult &= b;



    return testResult;
END_CVTTEST

}
