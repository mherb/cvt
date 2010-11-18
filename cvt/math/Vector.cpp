#include <cvt/math/Vector.h>
#include <cvt/util/CVTTest.h>

namespace cvt {

	BEGIN_CVTTEST(Vector2)
		Vector2f vec;
		Vector2f zero( 0.0f, 0.0f );
		bool b = true;

		vec.zero();
		b = b && vec == zero;
		CVTTEST_PRINT( "zero()", b );

		vec.set( 0.5f, 2.0f );
		b = b && vec == Vector2f( 0.5f, 2.0f );
		CVTTEST_PRINT( "set()", b );

		vec[ 0 ] = 10.0f;
		vec[ 1 ] = 20.0f;
		b = b && vec == Vector2f( 10.0f, 20.0f );
		CVTTEST_PRINT( "operator[]", b );

		b = b && Math::abs( vec[ 0 ] - 10.0f ) < Math::EPSILONF && Math::abs( vec[ 1 ] - 20.0f ) < Math::EPSILONF;
		CVTTEST_PRINT( "const operator[]", b );

		vec.zero();
		vec += 1.0f;
		b = b && vec == Vector2f( 1.0f, 1.0f );
		CVTTEST_PRINT( "operator+=", b );

		vec.zero();
		vec -= 1.0f;
		b = b && vec == Vector2f( -1.0f, -1.0f );
		CVTTEST_PRINT( "operator-=", b );

		vec *= -1.0f;
		b = b && vec == Vector2f( 1.0f, 1.0f );
		CVTTEST_PRINT( "operator*=", b );

		vec /= 2.0f;
		b = b && vec == Vector2f( 0.5f, 0.5f );
		CVTTEST_PRINT( "operator/=", b );

		b = b && Math::abs( vec.lengthSqr() - vec * vec ) < Math::EPSILONF;
		CVTTEST_PRINT( "lengthSqr", b );

		b = b && Math::abs( vec.length() - Math::sqrt( vec * vec ) ) < Math::EPSILONF;
		CVTTEST_PRINT( "length", b );

		return b;
	END_CVTTEST

	BEGIN_CVTTEST(Vector3)
		Vector3f vec;
		Vector3f zero( 0.0f, 0.0f, 0.0f );
		bool b = true;

		vec.zero();
		b = b && vec == zero;
		CVTTEST_PRINT( "zero()", b );

		vec.set( 0.5f, 2.0f, -5.0f );
		b = b && vec == Vector3f( 0.5f, 2.0f, -5.0f );
		CVTTEST_PRINT( "set()", b );

		vec[ 0 ] = 10.0f;
		vec[ 1 ] = 20.0f;
		vec[ 2 ] = 30.0f;
		b = b && vec == Vector3f( 10.0f, 20.0f, 30.0f );
		CVTTEST_PRINT( "operator[]", b );

		b = b && Math::abs( vec[ 0 ] - 10.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 1 ] - 20.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 2 ] - 30.0f ) < Math::EPSILONF;
		CVTTEST_PRINT( "const operator[]", b );

		vec.zero();
		vec += 1.0f;
		b = b && vec == Vector3f( 1.0f, 1.0f, 1.0f );
		CVTTEST_PRINT( "operator+=", b );

		vec.zero();
		vec -= 1.0f;
		b = b && vec == Vector3f( -1.0f, -1.0f, -1.0f );
		CVTTEST_PRINT( "operator-=", b );

		vec *= -1.0f;
		b = b && vec == Vector3f( 1.0f, 1.0f, 1.0f );
		CVTTEST_PRINT( "operator*=", b );

		vec /= 2.0f;
		b = b && vec == Vector3f( 0.5f, 0.5f, 0.5f );
		CVTTEST_PRINT( "operator/=", b );

		b = b && Math::abs( vec.lengthSqr() - vec * vec ) < Math::EPSILONF;
		CVTTEST_PRINT( "lengthSqr", b );

		b = b && Math::abs( vec.length() - Math::sqrt( vec * vec ) ) < Math::EPSILONF;
		CVTTEST_PRINT( "length", b );

		return b;
	END_CVTTEST

	BEGIN_CVTTEST(Vector4)
		Vector4f vec;
		Vector4f zero( 0.0f, 0.0f, 0.0f, 0.0f );
		bool b = true;

		vec.zero();
		b = b && vec == zero;
		CVTTEST_PRINT( "zero()", b );

		vec.set( 0.5f, 2.0f, -5.0f, 8.0f );
		b = b && vec == Vector4f( 0.5f, 2.0f, -5.0f, 8.0f );
		CVTTEST_PRINT( "set()", b );

		vec[ 0 ] = 10.0f;
		vec[ 1 ] = 20.0f;
		vec[ 2 ] = 30.0f;
		vec[ 3 ] = 40.0f;
		b = b && vec == Vector4f( 10.0f, 20.0f, 30.0f, 40.0f );
		CVTTEST_PRINT( "operator[]", b );

		b = b && Math::abs( vec[ 0 ] - 10.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 1 ] - 20.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 2 ] - 30.0f ) < Math::EPSILONF
			  && Math::abs( vec[ 3 ] - 40.0f ) < Math::EPSILONF;
		CVTTEST_PRINT( "const operator[]", b );

		vec.zero();
		vec += 1.0f;
		b = b && vec == Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
		CVTTEST_PRINT( "operator+=", b );

		vec.zero();
		vec -= 1.0f;
		b = b && vec == Vector4f( -1.0f, -1.0f, -1.0f, -1.0f );
		CVTTEST_PRINT( "operator-=", b );

		vec *= -1.0f;
		b = b && vec == Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
		CVTTEST_PRINT( "operator*=", b );

		vec /= 2.0f;
		b = b && vec == Vector4f( 0.5f, 0.5f, 0.5f, 0.5f );
		CVTTEST_PRINT( "operator/=", b );

		b = b && Math::abs( vec.lengthSqr() - vec * vec ) < Math::EPSILONF;
		CVTTEST_PRINT( "lengthSqr", b );

		b = b && Math::abs( vec.length() - Math::sqrt( vec * vec ) ) < Math::EPSILONF;
		CVTTEST_PRINT( "length", b );

		return b;
	END_CVTTEST


}
