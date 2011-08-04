#include <cvt/geom/PointSet.h>

#include <cvt/util/CVTTest.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Math.h>

namespace cvt
{
    template <typename T>
	static void generate3dPoints( std::vector<Vector3<T> > & pts, size_t n )
	{
		Vector3<T> p;
		srandom( time( NULL ) );
		while( n-- ){
			p.x = Math::rand( ( T )-1000, ( T )1000 );
			p.y = Math::rand( ( T )-1000, ( T )1000 );
			p.z = Math::rand( ( T )-1000, ( T )1000 );
			pts.push_back( p );
		}
	}

    template <typename T>
	static void fillPointSets( PointSet<2, T> & p0, PointSet<2, T> & p1, const Matrix3<T> & K, const Matrix4<T> & transform, std::vector<Vector3<T> > & pts )
	{
		Vector3<T> p;
		Vector3<T> pp;
        Vector2<T> p2;


		for( size_t i = 0; i < pts.size(); i++ ){
			p = K * pts[ i ];

            p2 = Vector2<T>( p );
            p2.x += Math::rand( ( T )-0.1, ( T )0.1 );
            p2.y += Math::rand( ( T )-0.1, ( T )0.1 );

			p0.add( p2 );


			pp = transform * pts[ i ];
			p = K * pp;

            p2 = Vector2<T>( p );
            p2.x += Math::rand( ( T )-0.1, ( T )0.1 );
            p2.y += Math::rand( ( T )-0.1, ( T )0.1 );
			p1.add( p2 );
		}
	}

    template <typename T>
    static bool compareMatrices( const Matrix3<T> & m1, const Matrix3<T> & m2, T epsilon )
    {
        for( size_t i = 0; i < 3; i++ )
            for( size_t k = 0; k < 3; k++ )
                if( Math::abs( m1[ i ][ k ] - m2[ i ][ k ] ) > epsilon )
                    return false;

        return true;
    }

    template <typename T>
    bool essentialTest()
    {
        PointSet<2, T> p0, p1;
        std::vector<Vector3<T> > pts;

        Matrix4<T> trans;
        trans.setIdentity();

        Vector3<T> t( 10.0, 30.0, 50.0 );

        trans.setRotationXYZ( Math::deg2Rad<T>( 20 ), Math::deg2Rad<T>( 10 ), Math::deg2Rad<T>( 30 ) );
        trans.setTranslation( t.x, t.y, t.z );

        Matrix3<T> K( 500.0,   0.0, 320.0,
                      0.0,   505.0, 240.0,
                      0.0,     0.0,   1.0 );

        generate3dPoints( pts, 8 );
        fillPointSets( p0, p1, K, trans, pts );

        Matrix3<T> E;

        E = p0.essentialMatrix( p1, K );
        E *= 1.0 / E[ 2 ][ 2 ];

        // essential matrix = [t]_x R
        Matrix3<T> skew, Etrue;
        skew.setSkewSymmetric( t );

        Etrue = skew * trans.toMatrix3();
        Etrue *= 1.0 / Etrue[ 2 ][ 2 ];

        bool b = compareMatrices( Etrue, E, (T)0.00001 );
        if( !b ){
            std::cout << "True:\n" << Etrue << std::endl;
            std::cout << "Estimated:\n" << E << std::endl;
        }
        return b;
    }

}

BEGIN_CVTTEST( PointSet )

	bool result = true;

	using namespace cvt;

    bool b;

    b = essentialTest<double>();
    CVTTEST_PRINT( "essentialMatrix<double>(): ", b );
	result &= b;

    b = essentialTest<float>();
    CVTTEST_PRINT( "essentialMatrix<float>(): ", b );
	result &= b;

	return result;
END_CVTTEST
