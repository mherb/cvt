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
	static void generate3dPointSet( PointSet<3, T> & set, size_t n )
	{
		Vector3<T> p;
		srandom( time( NULL ) );
		while( n-- ){
			p.x = Math::rand( ( T )-1000, ( T )1000 );
			p.y = Math::rand( ( T )-1000, ( T )1000 );
			p.z = Math::rand( ( T )-1000, ( T )1000 );
			set.add( p );
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
            //p2.x += Math::rand( ( T )-0.1, ( T )0.1 );
            //p2.y += Math::rand( ( T )-0.1, ( T )0.1 );

			p0.add( p2 );


			pp = transform * pts[ i ];
			p = K * pp;

            p2 = Vector2<T>( p );
			//p2.x += Math::rand( ( T )-0.1, ( T )0.1 );
            //p2.y += Math::rand( ( T )-0.1, ( T )0.1 );
			p1.add( p2 );
		}
	}

	template <typename T>
	static bool alignRigidTest()
	{
		bool ret = true;

		PointSet<3, T> set0, set1;

		generate3dPointSet( set0, 40 );

		srandom( time( NULL ) );

		for( size_t i = 0; i < 100; i++ ){
			set1 = set0;
			Matrix4<T> transformation;
			transformation.setRotationXYZ( Math::deg2Rad( Math::rand( (T)-40, (T)40 ) ), 
										   Math::deg2Rad( Math::rand( (T)-40, (T)40 ) ), 
										   Math::deg2Rad( Math::rand( (T)-40, (T)40 ) ) );

			transformation.setTranslation( Math::rand( (T)-100, (T)100 ),
										   Math::rand( (T)-100, (T)100 ),
										   Math::rand( (T)-100, (T)100 ) );
			set1.transform( transformation );

			Matrix4<T> estimated;
			estimated = set0.alignRigid( set1 );

			bool b =  estimated.isEqual( transformation, (T)0.001 );
			ret &= b;
			if( !b ){
				std::cout << "GT:\n" << transformation << std::endl;
				std::cout << "Estimated:\n" << estimated << std::endl;
			}
		}

		return ret;
	}

    template <typename T>
    static bool _essentialTest( T epsilon )
    {
        PointSet<2, T> p0, p1;
        std::vector<Vector3<T> > pts;

        Matrix4<T> trans;
        trans.setIdentity();

        Vector3<T> t( 10.0, 30.0, 50.0 );

        trans.setRotationXYZ( Math::deg2Rad<T>( 10 ), Math::deg2Rad<T>( 60 ), Math::deg2Rad<T>( 5 ) );
        trans.setTranslation( t.x, t.y, t.z );

        Matrix3<T> K( 500.0,   0.0, 320.0,
                      0.0,   505.0, 240.0,
                      0.0,     0.0,   1.0 );

        generate3dPoints( pts, 100 );
        fillPointSets( p0, p1, K, trans, pts );

        Matrix3<T> E;

        E = p0.essentialMatrix( p1, K );
        E *= 1.0 / E[ 2 ][ 2 ];

        // essential matrix = [t]_x R
        Matrix3<T> skew, Etrue;
        skew.setSkewSymmetric( t );

        Etrue = skew * trans.toMatrix3();
        Etrue *= 1.0 / Etrue[ 2 ][ 2 ];

        bool b = Etrue.isEqual( E, epsilon );
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
    
	b = alignRigidTest<float>();
    CVTTEST_PRINT( "alignRigid<float>(): ", b );
	result &= b;
	b = alignRigidTest<double>();
    CVTTEST_PRINT( "alignRigid<double>(): ", b );
	result &= b;

    b = _essentialTest<double>( 0.01 );
    CVTTEST_PRINT( "essentialMatrix<double>(): ", b );
	result &= b;

    b = _essentialTest<float>( 0.1f );
    CVTTEST_PRINT( "essentialMatrix<float>(): ", b );
	result &= b;

	return result;
END_CVTTEST
