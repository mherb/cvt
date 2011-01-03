#include <cvt/math/Matrix.h>
#include <cvt/util/CVTTest.h>

#include <iostream>

namespace cvt {

	template<>
	bool Matrix2<float>::inverseSelf()
	{
		float D, invD, tmp;

		D = mat[ 0 ][ 0 ] * mat[ 1 ][ 1 ] - mat[ 1 ][ 0 ] * mat[ 0 ][ 1 ];

		if ( Math::abs( D ) < Math::EPSILONF )
			return false;

		invD = 1.0f / D;

		tmp = mat[ 0 ][ 0 ];
		mat[ 0 ][ 0 ] =   mat[ 1 ][ 1 ] * invD;
		mat[ 0 ][ 1 ] = - mat[ 0 ][ 1 ] * invD;
		mat[ 1 ][ 0 ] = - mat[ 1 ][ 0 ] * invD;
		mat[ 1 ][ 1 ] =   tmp * invD;

		return true;
	}

	template<>
	bool Matrix2<double>::inverseSelf()
	{
		double D, invD, tmp;

		D = mat[ 0 ][ 0 ] * mat[ 1 ][ 1 ] - mat[ 1 ][ 0 ] * mat[ 0 ][ 1 ];

		if ( Math::abs( D ) < Math::EPSILOND )
			return false;

		invD = 1.0 / D;

		tmp = mat[ 0 ][ 0 ];
		mat[ 0 ][ 0 ] =   mat[ 1 ][ 1 ] * invD;
		mat[ 0 ][ 1 ] = - mat[ 0 ][ 1 ] * invD;
		mat[ 1 ][ 0 ] = - mat[ 1 ][ 0 ] * invD;
		mat[ 1 ][ 1 ] =   tmp * invD;

		return true;
	}

	template<>
	bool Matrix3<float>::inverseSelf()
	{
		Matrix3<float> cofactor;
		float D, invD;

		cofactor[ 0 ][ 0 ] = mat[ 1 ][ 1 ] * mat[ 2 ][ 2 ] - mat[ 2 ][ 1 ] * mat[ 1 ][ 2 ];
		cofactor[ 1 ][ 0 ] = mat[ 2 ][ 0 ] * mat[ 1 ][ 2 ] - mat[ 1 ][ 0 ] * mat[ 2 ][ 2 ];
		cofactor[ 2 ][ 0 ] = mat[ 1 ][ 0 ] * mat[ 2 ][ 1 ] - mat[ 2 ][ 0 ] * mat[ 1 ][ 1 ];

		D = mat[ 0 ][ 0 ] * cofactor[ 0 ][ 0 ] + mat[ 0 ][ 1 ] * cofactor[ 1 ][ 0 ] + mat[ 0 ][ 2 ] * cofactor[ 2 ][ 0 ];

		if ( Math::abs( D ) < Math::EPSILONF )
			return false;

		invD = 1.0f / D;

		cofactor[ 0 ][ 1 ] = mat[ 2 ][ 1 ] * mat[ 0 ][ 2 ] - mat[ 0 ][ 1 ] * mat[ 2 ][ 2 ];
		cofactor[ 0 ][ 2 ] = mat[ 0 ][ 1 ] * mat[ 1 ][ 2 ] - mat[ 1 ][ 1 ] * mat[ 0 ][ 2 ];

		cofactor[ 1 ][ 1 ] = mat[ 0 ][ 0 ] * mat[ 2 ][ 2 ] - mat[ 2 ][ 0 ] * mat[ 0 ][ 2 ];
		cofactor[ 1 ][ 2 ] = mat[ 1 ][ 0 ] * mat[ 0 ][ 2 ] - mat[ 0 ][ 0 ] * mat[ 1 ][ 2 ];

		cofactor[ 2 ][ 1 ] = mat[ 2 ][ 0 ] * mat[ 0 ][ 1 ] - mat[ 0 ][ 0 ] * mat[ 2 ][ 1 ];
		cofactor[ 2 ][ 2 ] = mat[ 0 ][ 0 ] * mat[ 1 ][ 1 ] - mat[ 1 ][ 0 ] * mat[ 0 ][ 1 ];

		mat[ 0 ][ 0 ] =  cofactor[ 0 ][ 0 ] * invD;
		mat[ 0 ][ 1 ] =  cofactor[ 0 ][ 1 ] * invD;
		mat[ 0 ][ 2 ] =  cofactor[ 0 ][ 2 ] * invD;

		mat[ 1 ][ 0 ] =  cofactor[ 1 ][ 0 ] * invD;
		mat[ 1 ][ 1 ] =  cofactor[ 1 ][ 1 ] * invD;
		mat[ 1 ][ 2 ] =  cofactor[ 1 ][ 2 ] * invD;

		mat[ 2 ][ 0 ] =  cofactor[ 2 ][ 0 ] * invD;
		mat[ 2 ][ 1 ] =  cofactor[ 2 ][ 1 ] * invD;
		mat[ 2 ][ 2 ] =  cofactor[ 2 ][ 2 ] * invD;

		return true;
	}

	template<>
	bool Matrix3<double>::inverseSelf()
	{
		Matrix3<double> cofactor;
		double D, invD;

		cofactor[ 0 ][ 0 ] = mat[ 1 ][ 1 ] * mat[ 2 ][ 2 ] - mat[ 2 ][ 1 ] * mat[ 1 ][ 2 ];
		cofactor[ 1 ][ 0 ] = mat[ 2 ][ 0 ] * mat[ 1 ][ 2 ] - mat[ 1 ][ 0 ] * mat[ 2 ][ 2 ];
		cofactor[ 2 ][ 0 ] = mat[ 1 ][ 0 ] * mat[ 2 ][ 1 ] - mat[ 2 ][ 0 ] * mat[ 1 ][ 1 ];

		D = mat[ 0 ][ 0 ] * cofactor[ 0 ][ 0 ] + mat[ 0 ][ 1 ] * cofactor[ 1 ][ 0 ] + mat[ 0 ][ 2 ] * cofactor[ 2 ][ 0 ];

		if ( Math::abs( D ) < Math::EPSILOND )
			return false;

		invD = 1.0 / D;

		cofactor[ 0 ][ 1 ] = mat[ 2 ][ 1 ] * mat[ 0 ][ 2 ] - mat[ 0 ][ 1 ] * mat[ 2 ][ 2 ];
		cofactor[ 0 ][ 2 ] = mat[ 0 ][ 1 ] * mat[ 1 ][ 2 ] - mat[ 1 ][ 1 ] * mat[ 0 ][ 2 ];

		cofactor[ 1 ][ 1 ] = mat[ 0 ][ 0 ] * mat[ 2 ][ 2 ] - mat[ 2 ][ 0 ] * mat[ 0 ][ 2 ];
		cofactor[ 1 ][ 2 ] = mat[ 1 ][ 0 ] * mat[ 0 ][ 2 ] - mat[ 0 ][ 0 ] * mat[ 1 ][ 2 ];

		cofactor[ 2 ][ 1 ] = mat[ 2 ][ 0 ] * mat[ 0 ][ 1 ] - mat[ 0 ][ 0 ] * mat[ 2 ][ 1 ];
		cofactor[ 2 ][ 2 ] = mat[ 0 ][ 0 ] * mat[ 1 ][ 1 ] - mat[ 1 ][ 0 ] * mat[ 0 ][ 1 ];

		mat[ 0 ][ 0 ] =  cofactor[ 0 ][ 0 ] * invD;
		mat[ 0 ][ 1 ] =  cofactor[ 0 ][ 1 ] * invD;
		mat[ 0 ][ 2 ] =  cofactor[ 0 ][ 2 ] * invD;

		mat[ 1 ][ 0 ] =  cofactor[ 1 ][ 0 ] * invD;
		mat[ 1 ][ 1 ] =  cofactor[ 1 ][ 1 ] * invD;
		mat[ 1 ][ 2 ] =  cofactor[ 1 ][ 2 ] * invD;

		mat[ 2 ][ 0 ] =  cofactor[ 2 ][ 0 ] * invD;
		mat[ 2 ][ 1 ] =  cofactor[ 2 ][ 1 ] * invD;
		mat[ 2 ][ 2 ] =  cofactor[ 2 ][ 2 ] * invD;

		return true;
	}

	template<>
	bool Matrix4<float>::inverseSelf()
	{
		float D, invD;
		float det2_23_01, det2_23_02, det2_23_03,
			  det2_23_12, det2_23_13, det2_23_23;
		float det3_123_123, det3_123_023, det3_123_013, det3_123_012;

		det2_23_01 = mat[ 2 ][ 0 ] * mat[ 3 ][ 1 ] - mat[ 3 ][ 0 ] * mat[ 2 ][ 1 ];
		det2_23_02 = mat[ 2 ][ 0 ] * mat[ 3 ][ 2 ] - mat[ 3 ][ 0 ] * mat[ 2 ][ 2 ];
		det2_23_03 = mat[ 2 ][ 0 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 0 ] * mat[ 2 ][ 3 ];
		det2_23_12 = mat[ 2 ][ 1 ] * mat[ 3 ][ 2 ] - mat[ 3 ][ 1 ] * mat[ 2 ][ 2 ];
		det2_23_13 = mat[ 2 ][ 1 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 1 ] * mat[ 2 ][ 3 ];
		det2_23_23 = mat[ 2 ][ 2 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 2 ] * mat[ 2 ][ 3 ];

		det3_123_123  =  mat[ 1 ][ 1 ] * det2_23_23 - mat[ 1 ][ 2 ] * det2_23_13 + mat[ 1 ][ 3 ] * det2_23_12;
		det3_123_023  =  mat[ 1 ][ 0 ] * det2_23_23 - mat[ 1 ][ 2 ] * det2_23_03 + mat[ 1 ][ 3 ] * det2_23_02;
		det3_123_013  =  mat[ 1 ][ 0 ] * det2_23_13 - mat[ 1 ][ 1 ] * det2_23_03 + mat[ 1 ][ 3 ] * det2_23_01;
		det3_123_012  =  mat[ 1 ][ 0 ] * det2_23_12 - mat[ 1 ][ 1 ] * det2_23_02 + mat[ 1 ][ 2 ] * det2_23_01;

		D = mat[ 0 ][ 0 ] * det3_123_123 - mat[ 0 ][ 1 ] * det3_123_023 + mat[ 0 ][ 2 ] * det3_123_013 - mat[ 0 ][ 3 ] * det3_123_012;

		if ( Math::abs( D ) < Math::EPSILONF )
			return false;

		invD = 1.0f / D;

		float det3_023_123, det3_023_023, det3_023_013, det3_023_012;

		det3_023_123  =  mat[ 0 ][ 1 ] * det2_23_23 - mat[ 0 ][ 2 ] * det2_23_13 + mat[ 0 ][ 3 ] * det2_23_12;
		det3_023_023  =  mat[ 0 ][ 0 ] * det2_23_23 - mat[ 0 ][ 2 ] * det2_23_03 + mat[ 0 ][ 3 ] * det2_23_02;
		det3_023_013  =  mat[ 0 ][ 0 ] * det2_23_13 - mat[ 0 ][ 1 ] * det2_23_03 + mat[ 0 ][ 3 ] * det2_23_01;
		det3_023_012  =  mat[ 0 ][ 0 ] * det2_23_12 - mat[ 0 ][ 1 ] * det2_23_02 + mat[ 0 ][ 2 ] * det2_23_01;

		float det2_13_01, det2_13_02, det2_13_03,
			  det2_13_12, det2_13_13, det2_13_23;
		float det3_013_123, det3_013_023, det3_013_013, det3_013_012;

		det2_13_01 = mat[ 1 ][ 0 ] * mat[ 3 ][ 1 ] - mat[ 3 ][ 0 ] * mat[ 1 ][ 1 ];
		det2_13_02 = mat[ 1 ][ 0 ] * mat[ 3 ][ 2 ] - mat[ 3 ][ 0 ] * mat[ 1 ][ 2 ];
		det2_13_03 = mat[ 1 ][ 0 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 0 ] * mat[ 1 ][ 3 ];
		det2_13_12 = mat[ 1 ][ 1 ] * mat[ 3 ][ 2 ] - mat[ 3 ][ 1 ] * mat[ 1 ][ 2 ];
		det2_13_13 = mat[ 1 ][ 1 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 1 ] * mat[ 1 ][ 3 ];
		det2_13_23 = mat[ 1 ][ 2 ] * mat[ 3 ][ 3 ] - mat[ 3 ][ 2 ] * mat[ 1 ][ 3 ];

		det3_013_123  =  mat[ 0 ][ 1 ] * det2_13_23 - mat[ 0 ][ 2 ] * det2_13_13 + mat[ 0 ][ 3 ] * det2_13_12;
		det3_013_023  =  mat[ 0 ][ 0 ] * det2_13_23 - mat[ 0 ][ 2 ] * det2_13_03 + mat[ 0 ][ 3 ] * det2_13_02;
		det3_013_013  =  mat[ 0 ][ 0 ] * det2_13_13 - mat[ 0 ][ 1 ] * det2_13_03 + mat[ 0 ][ 3 ] * det2_13_01;
		det3_013_012  =  mat[ 0 ][ 0 ] * det2_13_12 - mat[ 0 ][ 1 ] * det2_13_02 + mat[ 0 ][ 2 ] * det2_13_01;

		float det2_12_01, det2_12_02, det2_12_03,
			  det2_12_12, det2_12_13, det2_12_23;
		float det3_012_123, det3_012_023, det3_012_013, det3_012_012;

		det2_12_01 = mat[ 1 ][ 0 ] * mat[ 2 ][ 1 ] - mat[ 2 ][ 0 ] * mat[ 1 ][ 1 ];
		det2_12_02 = mat[ 1 ][ 0 ] * mat[ 2 ][ 2 ] - mat[ 2 ][ 0 ] * mat[ 1 ][ 2 ];
		det2_12_03 = mat[ 1 ][ 0 ] * mat[ 2 ][ 3 ] - mat[ 2 ][ 0 ] * mat[ 1 ][ 3 ];
		det2_12_12 = mat[ 1 ][ 1 ] * mat[ 2 ][ 2 ] - mat[ 2 ][ 1 ] * mat[ 1 ][ 2 ];
		det2_12_13 = mat[ 1 ][ 1 ] * mat[ 2 ][ 3 ] - mat[ 2 ][ 1 ] * mat[ 1 ][ 3 ];
		det2_12_23 = mat[ 1 ][ 2 ] * mat[ 2 ][ 3 ] - mat[ 2 ][ 2 ] * mat[ 1 ][ 3 ];

		det3_012_123  =  mat[ 0 ][ 1 ] * det2_12_23 - mat[ 0 ][ 2 ] * det2_12_13 + mat[ 0 ][ 3 ] * det2_12_12;
		det3_012_023  =  mat[ 0 ][ 0 ] * det2_12_23 - mat[ 0 ][ 2 ] * det2_12_03 + mat[ 0 ][ 3 ] * det2_12_02;
		det3_012_013  =  mat[ 0 ][ 0 ] * det2_12_13 - mat[ 0 ][ 1 ] * det2_12_03 + mat[ 0 ][ 3 ] * det2_12_01;
		det3_012_012  =  mat[ 0 ][ 0 ] * det2_12_12 - mat[ 0 ][ 1 ] * det2_12_02 + mat[ 0 ][ 2 ] * det2_12_01;

		mat[ 0 ][ 0 ] =   det3_123_123 * invD;
		mat[ 0 ][ 1 ] = - det3_023_123 * invD;
		mat[ 0 ][ 2 ] =   det3_013_123 * invD;
		mat[ 0 ][ 3 ] = - det3_012_123 * invD;

		mat[ 1 ][ 0 ] = - det3_123_023 * invD;
		mat[ 1 ][ 1 ] =   det3_023_023 * invD;
		mat[ 1 ][ 2 ] = - det3_013_023 * invD;
		mat[ 1 ][ 3 ] =   det3_012_023 * invD;

		mat[ 2 ][ 0 ] =   det3_123_013 * invD;
		mat[ 2 ][ 1 ] = - det3_023_013 * invD;
		mat[ 2 ][ 2 ] =   det3_013_013 * invD;
		mat[ 2 ][ 3 ] = - det3_012_013 * invD;

		mat[ 3 ][ 0 ] = - det3_123_012 * invD;
		mat[ 3 ][ 1 ] =   det3_023_012 * invD;
		mat[ 3 ][ 2 ] = - det3_013_012 * invD;
		mat[ 3 ][ 3 ] =   det3_012_012 * invD;

		return true;
	}


	BEGIN_CVTTEST(Matrix2)
		Matrix2f mat, m2;
		bool b = true;

		mat.identity();
		mat *= 2.0f;
		m2 = mat.inverse();
		mat *= m2;

		b = b && mat.isIdentity();
		CVTTEST_PRINT( "inverse()", b );

		return true;
	END_CVTTEST

	BEGIN_CVTTEST(Matrix3)
		Matrix3f mat, m2;
		bool b = true;

		mat.identity();
		mat *= 2.0f;
		m2 = mat.inverse();
		mat *= m2;

		b = b && mat.isIdentity();
		CVTTEST_PRINT( "inverse()", b );


		mat.identity();
		mat *= 2.0f;
		mat.inverseSelf();
		b = b && mat == Matrix3f(  0.5f, 0.0f,  0.0f,
								   0.0f, 0.5f,  0.0f,
								   0.0f, 0.0f,  0.5f );
		CVTTEST_PRINT( "inverse()", b );

		mat[ 0 ][ 0 ] =  1.0f;
		mat[ 0 ][ 1 ] =  0.0f;
		mat[ 0 ][ 2 ] =  4.0f;

		mat[ 1 ][ 0 ] =  1.0f;
		mat[ 1 ][ 1 ] =  1.0f;
		mat[ 1 ][ 2 ] =  6.0f;

		mat[ 2 ][ 0 ] = -3.0f;
		mat[ 2 ][ 1 ] =  0.0f;
		mat[ 2 ][ 2 ] = -10.0f;

		mat.inverseSelf();
		b = b && mat == Matrix3f( -5.0f, 0.0f, -2.0f,
								  -4.0f, 1.0f, -1.0f,
								   1.5f, 0.0f,  0.5f );
		CVTTEST_PRINT( "inverse()", b );


		mat[ 0 ][ 0 ] = -2.0f;
		mat[ 0 ][ 1 ] =  2.0f;
		mat[ 0 ][ 2 ] = -3.0f;

		mat[ 1 ][ 0 ] = -1.0f;
		mat[ 1 ][ 1 ] =  1.0f;
		mat[ 1 ][ 2 ] =  3.0f;

		mat[ 2 ][ 0 ] =  2.0f;
		mat[ 2 ][ 1 ] =  0.0f;
		mat[ 2 ][ 2 ] = -1.0f;

		b = b && mat.determinant() == 18.0f;
		CVTTEST_PRINT( "determinant()", b );

		return true;
	END_CVTTEST

	BEGIN_CVTTEST(Matrix4)
		Matrix4f mat, m2;
		bool b = true;

		mat.identity();
		mat *= 2.0f;
		m2 = mat.inverse();
		mat *= m2;

		b = b && mat.isIdentity();
		CVTTEST_PRINT( "inverse()", b );


		mat.identity();
		mat *= 2.0f;
		mat.inverseSelf();
		b = b && mat == Matrix4f( 0.5f, 0.0f,  0.0f, 0.0f,
								  0.0f, 0.5f,  0.0f, 0.0f,
								  0.0f, 0.0f,  0.5f, 0.0f,
								  0.0f, 0.0f,  0.0f, 0.5f );

		CVTTEST_PRINT( "inverse()", b );



		mat[ 0 ][ 0 ] =  3.0f;
		mat[ 0 ][ 1 ] =  2.0f;
		mat[ 0 ][ 2 ] =  0.0f;
		mat[ 0 ][ 3 ] =  1.0f;

		mat[ 1 ][ 0 ] =  4.0f;
		mat[ 1 ][ 1 ] =  0.0f;
		mat[ 1 ][ 2 ] =  1.0f;
		mat[ 1 ][ 3 ] =  2.0f;

		mat[ 2 ][ 0 ] =  3.0f;
		mat[ 2 ][ 1 ] =  0.0f;
		mat[ 2 ][ 2 ] =  2.0f;
		mat[ 2 ][ 3 ] =  1.0f;

		mat[ 3 ][ 0 ] =  9.0f;
		mat[ 3 ][ 1 ] =  3.0f;
		mat[ 3 ][ 2 ] =  2.0f;
		mat[ 3 ][ 3 ] =  1.0f;

		b = b && mat.determinant() == 24.0f;
		CVTTEST_PRINT( "determinant()", b );

		mat[ 0 ][ 0 ] =  2.0f;
		mat[ 0 ][ 1 ] =  5.0f;
		mat[ 0 ][ 2 ] = -2.0f;
		mat[ 0 ][ 3 ] = -1.0f;

		mat[ 1 ][ 0 ] =  3.0f;
		mat[ 1 ][ 1 ] =  4.0f;
		mat[ 1 ][ 2 ] =  1.0f;
		mat[ 1 ][ 3 ] =  2.0f;

		mat[ 2 ][ 0 ] = -1.0f;
		mat[ 2 ][ 1 ] =  9.0f;
		mat[ 2 ][ 2 ] =  6.0f;
		mat[ 2 ][ 3 ] =  7.0f;

		mat[ 3 ][ 0 ] =  8.0f;
		mat[ 3 ][ 1 ] =  1.0f;
		mat[ 3 ][ 2 ] =  3.0f;
		mat[ 3 ][ 3 ] =  2.0f;

		b = b && mat.determinant() == 301.0f;
		CVTTEST_PRINT( "determinant()", b );

		return true;
	END_CVTTEST

}
