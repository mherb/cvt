//
//  SL3Test.cpp
//  CVTools
//
//  Created by Sebastian Klose on 08.02.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SL3Test.h"

#include <cvt/math/SL3.h>
#include <cvt/math/Math.h>
#include <cvt/util/CVTTest.h>

#include <Eigen/Core>

namespace cvt {

	static bool testJacobian()
	{
		Eigen::Matrix<double, 8, 1> delta = Eigen::Matrix<double, 8, 1>::Zero();
		Eigen::Matrix<double, 3, 8> jNumeric, jAnalytic, jDiff;
		Eigen::Matrix<double, 2, 8> sJN, sJA, sJDiff;

		SL3<double> pose;
		pose.set( Math::deg2Rad( 10 ), Math::deg2Rad( -25 ), 0.76, 0.89, 10, -10, 0.0001, 0.0002 );

		Eigen::Matrix<double, 3, 1> point;
		Eigen::Matrix<double, 3, 1> p, pp;
		Eigen::Matrix<double, 2, 1> sp, spp;
		point[ 0 ] = 13; point[ 1 ] = 8; point[ 2 ] = 12;;

		pose.transform( p, point );
		pose.project( sp, point );

		double h = 0.000000001;
		for( size_t i = 0; i < 8; i++ ){
			delta[ i ] = h;

			pose.apply( delta );

			pose.transform( pp, point );
			jNumeric.col( i ) = ( pp - p ) / h;

			pose.project( spp, point );
			sJN.col( i ) = ( spp - sp ) / h;

			delta[ i ] = 0;

			pose.set( Math::deg2Rad( 10 ), Math::deg2Rad( -25 ), 0.76, 0.89, 10, -10, 0.0001, 0.0002 );
		}

		pose.jacobian( jAnalytic, point );
		pose.project( spp, sJA, point );

		bool b, ret = true;
		jDiff = jAnalytic - jNumeric;
		b = ( jDiff.array().abs().sum() / 24.0 ) < 0.00001;
		CVTTEST_PRINT( "Pose Jacobian", b );
		if( !b ){
			std::cout << "Analytic:\n" << jAnalytic << std::endl;
			std::cout << "Numeric:\n" << jNumeric << std::endl;
			std::cout << "Difference:\n" << jDiff << std::endl;
		}
		ret &= b;

		sJDiff = sJA - sJN;
		b = ( sJDiff.array().abs().sum() / 16.0 ) < 0.0001;
		CVTTEST_PRINT( "Screen Jacobian", b );
		if( !b ){
			std::cout << "Analytic:\n" << sJA << std::endl;
			std::cout << "Numeric:\n" << sJN << std::endl;
			std::cout << "Difference:\n" << sJDiff << std::endl;
			std::cout << "Avg. Diff: " << sJDiff.array().abs().sum() / 16.0 << std::endl;
		}
		ret &= b;

		return ret;
	}

BEGIN_CVTTEST( SL3 )
	bool ret = true;

	SL3<double> pose;
	bool b = true;
	b = ( pose.transformation() == Eigen::Matrix<double, 3, 3>::Identity() );

	CVTTEST_PRINT( "Initialization", b );
	ret &= b;
	ret &= testJacobian();

	return ret;
END_CVTTEST

}
