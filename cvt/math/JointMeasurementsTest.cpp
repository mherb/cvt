/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/util/CVTTest.h>
#include <cvt/math/JointMeasurements.h>
#include <cvt/math/Math.h>

namespace cvt {

	static bool checkSingleConstraintSingleMeasurement( size_t c0, size_t c1, size_t m, const JointMeasurements & jm )
	{
		// MeasIter for c0 should contain a single set for c1 with single meas m
		JointMeasurements::ConstMapIterType iter = jm.secondEntityIteratorBegin( c0 );
		JointMeasurements::ConstMapIterType end  = jm.secondEntityIteratorEnd( c0 );
		bool b = true;
		if( iter->first != c1 ){
			b = false;	
		} else {
			std::set<size_t>::const_iterator it = iter->second.begin();

			// the one element should be m
			if( *it != m )
				b = false;

			// should be only one element in set
			it++;
			if( it != iter->second.end() )
				b = false;

			// there should be only one constraint
			iter++;
			if( iter != end )
				b = false;
		}
		return b;
	}

	static bool stressTest()
	{
		try {
			JointMeasurements jm;
			jm.resize( 7 );

			size_t numElems = 10000;

			while( numElems-- ){
				size_t c0 = Math::rand( 0, 7 );
				size_t c1 = Math::rand( 0, 7 );
				size_t m = Math::rand( 0, 100 );

				if( c0 > c1 ){
					jm.addMeasurementForEntity( c1, c0, m );
				} else {
					jm.addMeasurementForEntity( c0, c1, m );
				}
			}

		} catch( const Exception & e )
		{
			return false;
		}
		return true;
	}

BEGIN_CVTTEST( JointMeasurements )
	bool ret = true;

	JointMeasurements jm;

	bool b;

	b = (jm.size() == 0 );
	CVTTEST_PRINT( "size()", b );
	ret &= b;

	jm.resize( 10 );
	b = ( jm.size() == 10 );
	jm.resize( 5 );
	b &= ( jm.size() == 5 );
	CVTTEST_PRINT( "resize()", b );
	ret &= b;

	// add measurements:
	size_t c0, c1, m;
	c0 = c1 = m = 0;
	c1 = 1;
	
	// adding two times the same should result in single contraint with single measurement
	jm.addMeasurementForEntity( c0, c1, m );
	b = checkSingleConstraintSingleMeasurement( c0, c1, m, jm );
	jm.addMeasurementForEntity( c0, c1, m );
	b = checkSingleConstraintSingleMeasurement( c0, c1, m, jm );

	CVTTEST_PRINT( "addMeasurementForEntity()", b );
	ret &= b;

	b = stressTest();	
	CVTTEST_PRINT( "stressTest:", b );
	ret &= b;


	return ret;
END_CVTTEST

}
