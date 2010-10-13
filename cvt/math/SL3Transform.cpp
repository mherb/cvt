/*
 *  SL3Transform.cpp
 *  CVTools
 *
 *  Created by Sebastian Klose on 10.10.10.
 *  Copyright 2010. All rights reserved.
 *
 */

#include "SL3Transform.h"
#include "Math.h"

namespace cvt {
	
SL3Transform::SL3Transform():
	gMatrix( Eigen::Matrix<double, 9, 8>::Zero() ),
	transform( Eigen::Matrix3d::Identity() ),
	jacs( 8, Eigen::Matrix3d::Zero() )
{
	updateJacobians();
	
	gMatrix( 2, 0 ) = 1.0;
	gMatrix( 5, 1 ) = 1.0;
	gMatrix( 1, 2 ) = 1.0;
	gMatrix( 3, 3 ) = 1.0;
	gMatrix( 0, 4 ) = 1.0;
	gMatrix( 4, 4 ) = -1.0;
	gMatrix( 4, 5 ) = -1.0;
	gMatrix( 8, 5 ) = 1.0;
	gMatrix( 6, 6 ) = 1.0;
	gMatrix( 7, 7 ) = 1.0;
}

SL3Transform::~SL3Transform()
{	
}

/* T = T * exp( sum( delta_i * G_i ) ) */
void SL3Transform::update( const Eigen::Matrix<double, 8, 1> & delta )
{
	Eigen::Matrix3d m( Eigen::Matrix3d::Zero() );
	
	m( 0, 0 ) = delta[ 4 ];
	m( 0, 1 ) = delta[ 2 ];
	m( 0, 2 ) = delta[ 0 ];
	
	m( 1, 0 ) = delta[ 3 ];
	m( 1, 1 ) = -delta[ 4 ] - delta[5];
	m( 1, 2 ) = delta[ 1 ];
	
	m( 2, 0 ) = delta[ 6 ];		
	m( 2, 1 ) = delta[ 7 ];
	m( 2, 2 ) = delta[ 5 ];	
	
	/* m = exp( m ) */
	cvt::Math::exponential( m, m );
	
	/* update the current transformation */
	transform *= m;
	
	/* update the jacobians */
	updateJacobians();
}

const std::vector<Eigen::Matrix3d> & SL3Transform::jacobians() const
{
	return jacs;
}

void SL3Transform::jacobiansAtPoint( const Eigen::Vector3d & p, Eigen::Matrix<double, 2, 8> & J ) const
{
	Eigen::Vector3d tmp;
	double dehom;
	
	// TODO: evaluate by hand (most parts of the matrices are 0)	
	for( unsigned int i = 0; i < 8; i++ ){
		tmp = jacs[ i ] * p;
		dehom = 1.0 / tmp[ 3 ];
		
		J( 0, i ) = tmp[ 0 ] * dehom;
		J( 1, i ) = tmp[ 1 ] * dehom;
	}
}

void SL3Transform::jacobiansAtPoint( const Eigen::Vector2d & p, Eigen::Matrix<double, 2, 8> & J ) const
{
	Eigen::Vector3d tmp, pHom;
	pHom.block(0, 0, 2, 1) = p; 
	pHom[ 2 ] = 1.0;
	
	double dehom;
	
	// TODO: evaluate by hand (most parts of the matrices are 0)	
	for( unsigned int i = 0; i < 8; i++ ){
		tmp = jacs[ i ] * pHom;
		dehom = 1.0 / tmp[ 2 ];
		
		J( 0, i ) = tmp[ 0 ] * dehom;
		J( 1, i ) = tmp[ 1 ] * dehom;
	}
}
	
void SL3Transform::jacobiansAtPoint( const Eigen::Vector2d & p, Eigen::MatrixXd & J, size_t rowOffset ) const
{	
	/*
	J( 0 + rowOffset, 0 ) = transform( 0, 0 );
	J( 1 + rowOffset, 0 ) = transform( 1, 0 );
		
	J( 0 + rowOffset, 1 ) = transform( 0, 1 );
	J( 1 + rowOffset, 1 ) = transform( 1, 1 );
		
	J( 0 + rowOffset, 2 ) = J( 0, 0 );
	J( 1 + rowOffset, 2 ) = J( 1, 0 );
	
	J( 0 + rowOffset, 3 ) = J( 0, 1 );
	J( 1 + rowOffset, 3 ) = J( 1, 1 );
		
	J( 0 + rowOffset, 4 ) = ( p[0] * transform( 0, 0 ) - p[1] * transform( 0, 1 ) );
	J( 1 + rowOffset, 4 ) = ( p[0] * transform( 1, 0 ) - p[1] * transform( 1, 1 ) );
		
	J( 0 + rowOffset, 5 ) = ( -p[1] * transform( 0, 1 ) + transform( 0, 2 ) );
	J( 1 + rowOffset, 5 ) = ( -p[1] * transform( 1, 1 ) + transform( 1, 2 ) );
	
	J( 0 + rowOffset, 6 ) = transform( 0, 2 );
	J( 1 + rowOffset, 6 ) = transform( 1, 2 );		
		
	J( 0 + rowOffset, 7 ) = J( 0, 6 );
	J( 1 + rowOffset, 7 ) = J( 1, 6 );*/
	
	J( 0 + rowOffset, 0 ) = 1.0;
	J( 1 + rowOffset, 0 ) = 0.0;
	
	J( 0 + rowOffset, 1 ) = 0.0;
	J( 1 + rowOffset, 1 ) = 1.0;
	
	J( 0 + rowOffset, 2 ) = p[ 1 ];
	J( 1 + rowOffset, 2 ) = 0.0;
	
	J( 0 + rowOffset, 3 ) = 0.0;
	J( 1 + rowOffset, 3 ) = p[ 0 ];
	
	J( 0 + rowOffset, 4 ) = p[ 0 ];
	J( 1 + rowOffset, 4 ) = -p[ 1 ];
	
	J( 0 + rowOffset, 5 ) = -p[ 0 ];
	J( 1 + rowOffset, 5 ) = -2 * p[ 1 ];
	
	J( 0 + rowOffset, 6 ) = -p[ 0 ] * p[ 0 ];
	J( 1 + rowOffset, 6 ) = -p[ 0 ] * p[ 1 ];		
	
	J( 0 + rowOffset, 7 ) = -p[ 0 ] * p[ 1 ];
	J( 1 + rowOffset, 7 ) = -p[ 1 ] * p[ 1 ];
}
	
/* J_i = T * G_i */
void SL3Transform::updateJacobians()
{
	jacs[ 0 ]( 0, 2 ) = transform( 0, 0 );	
	jacs[ 0 ]( 1, 2 ) = transform( 1, 0 );
	jacs[ 0 ]( 2, 2 ) = transform( 2, 0 );
	
	jacs[ 1 ]( 0, 2 ) = transform( 0, 1 );	
	jacs[ 1 ]( 1, 2 ) = transform( 1, 1 );
	jacs[ 1 ]( 2, 2 ) = transform( 2, 1 );
	
	jacs[ 2 ]( 0, 1 ) = transform( 0, 0 );	
	jacs[ 2 ]( 1, 1 ) = transform( 1, 0 );
	jacs[ 2 ]( 2, 1 ) = transform( 2, 0 );
	
	jacs[ 3 ]( 0, 0 ) = transform( 0, 1 );	
	jacs[ 3 ]( 1, 0 ) = transform( 1, 1 );
	jacs[ 3 ]( 2, 0 ) = transform( 2, 1 );
	
	jacs[ 4 ]( 0, 0 ) = transform( 0, 0 );	
	jacs[ 4 ]( 1, 0 ) = transform( 1, 0 );
	jacs[ 4 ]( 2, 0 ) = transform( 2, 0 );
	jacs[ 4 ]( 0, 1 ) = -transform( 0, 1 );	
	jacs[ 4 ]( 1, 1 ) = -transform( 1, 1 );
	jacs[ 4 ]( 2, 1 ) = -transform( 2, 1 );
	
	jacs[ 5 ]( 0, 1 ) = -transform( 0, 1 );	
	jacs[ 5 ]( 1, 1 ) = -transform( 1, 1 );
	jacs[ 5 ]( 2, 1 ) = -transform( 2, 1 );
	jacs[ 5 ]( 0, 2 ) = transform( 0, 2 );	
	jacs[ 5 ]( 1, 2 ) = transform( 1, 2 );
	jacs[ 5 ]( 2, 2 ) = transform( 2, 2 );
	
	jacs[ 6 ]( 0, 0 ) = transform( 0, 2 );	
	jacs[ 6 ]( 1, 0 ) = transform( 1, 2 );
	jacs[ 6 ]( 2, 0 ) = transform( 2, 2 );
	
	jacs[ 7 ]( 0, 1 ) = transform( 0, 2 );	
	jacs[ 7 ]( 1, 1 ) = transform( 1, 2 );
	jacs[ 7 ]( 2, 1 ) = transform( 2, 2 );
}
	
	
const Eigen::Matrix<double, 9, 8> & SL3Transform::generatorMatrix()
{
	return gMatrix;
}
	
}
