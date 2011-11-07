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
        if( Math::abs( a[ 0 ] - b[ 0 ] ) > epsilon )
            return false;
        if( Math::abs( a[ 1 ] - b[ 1 ] ) > epsilon )
            return false;
        if( Math::abs( a[ 2 ] - b[ 2 ] ) > epsilon )
            return false;
        return true;
    }

    template <typename T>
    static bool _compareVectorsNormalized( const Vector3<T> & a, const Vector3<T> & b, T epsilon )
    {
        Vector3<T> a0 = a;
        a0.normalize();

        Vector3<T> b0 = b;
        b0.normalize();

        return _compareVectors( a0, b0, epsilon );
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

            Matrix3<T> R0, R1, RR;
            Vector3<T> t0, t1, tt;
            Vision::decomposeEssential( R0, R1, t0, t1, E );

            bool b = true;

            b = _compare( R, R0, (T)0.0001 ) || _compare( R, R1, (T)0.0001 );
            result &= b;
            if( !b ){
                std::cout << "Ground Truth Rotation: \n" << R << std::endl;
                std::cout << "Decomposed R0: \n"            << R0 << std::endl;
                std::cout << "Decomposed R1: \n"            << R1 << std::endl;
            }

            b = _compareVectorsNormalized( t, t0, (T)0.0001 ) || _compareVectorsNormalized( t, t1, (T)0.0001 );
            result &= b;

            if( !b ){
                std::cout << "Ground Truth Translation: \n" << t << std::endl;
                std::cout << "Decomposed t0: \n" << t0 << std::endl;
                std::cout << "Decomposed t1: \n" << t1 << std::endl;
            }
        }

        return result;
    }

    template <typename T>
    static bool _triangulate()
    {
        bool ret = true;

        Matrix3<T> K( 230.0,   0.0, 320.0,
                        0.0, 235.0, 240.0,
                        0.0,   0.0,   1.0 );

		Matrix4<T> T0;
		T0.setIdentity();
		Matrix3<T> fund;

        for( size_t i = 0; i < 100; i++ ){
            Matrix4<T> T1;
            T1.setRotationXYZ( Math::rand( ( T )-Math::PI/6.0, ( T )Math::PI/6.0 ),
                                Math::rand( ( T )-Math::PI/6.0, ( T )Math::PI/6.0 ),
                                Math::rand( ( T )-Math::PI/6.0, ( T )Math::PI/6.0 ));
            T1.setTranslation( Math::rand( ( T )-10, ( T )10 ),
                                Math::rand( ( T )-10, ( T )10 ),
                                Math::rand( ( T )-10, ( T )10 ) );
            T1[ 3 ][ 3 ] = ( T )1;


			Vision::composeFundamental( fund, K, T0, K, T1 );

            Vector4<T> truePoint( Math::rand( ( T )-100, ( T )200 ),
                                  Math::rand( ( T )-100, ( T )200 ),
                                  Math::rand( ( T )-100, ( T )200 ),
                                  ( T )1 );

            Matrix3<T> R = T1.toMatrix3();
            Matrix4<T> P0( K ), P1( K*R );
            Vector3<T> t( T1[ 0 ][ 3 ], T1[ 1 ][ 3 ], T1[ 2 ][ 3 ] );

            t = K * t;
            P1[ 0][  3 ] = t[ 0 ];
            P1[ 1][  3 ] = t[ 1 ];
            P1[ 2][  3 ] = t[ 2 ];
            P1[ 3 ][ 3 ] = ( T )1;
            P0[ 3 ][ 3 ] = ( T )1;


            Vector4<T> tmp;
            Vector2<T> proj0, proj1;

            tmp = P0 * truePoint;
            proj0[ 0 ] = tmp[ 0 ] / tmp[ 2 ];
            proj0[ 1 ] = tmp[ 1 ] / tmp[ 2 ];
            tmp = P1 * truePoint;
            proj1[ 0 ] = tmp[ 0 ] / tmp[ 2 ];
            proj1[ 1 ] = tmp[ 1 ] / tmp[ 2 ];

			proj0[ 0 ] += Math::rand( ( T )-1, ( T )1 );
			proj0[ 1 ] += Math::rand( ( T )-1, ( T )1 );
			proj1[ 0 ] += Math::rand( ( T )-1, ( T )1 );
			proj1[ 1 ] += Math::rand( ( T )-1, ( T )1 );

			Vision::correctCorrespondencesSampson( proj0, proj1, fund );
            Vision::triangulate( tmp, P0, P1, proj0, proj1 );

            // normalize
            tmp *= ( T )1 / tmp[ 3 ];

            bool b = ( ( tmp - truePoint ).length() < 5 );
            ret &= b;

            if( !b ){
                std::cout << "Ground Truth point:\t\t" << truePoint << std::endl;
                std::cout << "Estimated \t\t: " << tmp << std::endl;
            }
        }

        return ret;
    }

BEGIN_CVTTEST( Vision )
    bool testResult = true;
    bool b;


    b = _essentialTest<float>();
    CVTTEST_PRINT( "decomposeEssential<float>()\t", b );
    testResult &= b;

    b = _essentialTest<double>();
    CVTTEST_PRINT( "decomposeEssential<double>()\t", b );
    testResult &= b;

    b = _triangulate<float>();
    CVTTEST_PRINT( "triangulate<float>()\t", b );
    testResult &= b;
    b = _triangulate<double>();
    CVTTEST_PRINT( "triangulate<double>()\t", b );
    testResult &= b;

    return testResult;
END_CVTTEST

}
