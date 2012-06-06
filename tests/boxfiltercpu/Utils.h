#ifndef UTILS_H
#define UTILS_H

#include <cvt/math/Vector.h>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>

template<class T>
inline T* linePtr( const T *base, const size_t stride, const int row )
{
	return ( T * ) ( ( ( uint8_t *) base ) + stride * row );
}

template<class T>
inline T& getPixel( const T *base, const size_t stride, const int row, const int col )
{
	return linePtr( base, stride, row )[col];
}

template<class T>
inline void setPixel( const T &value, const T *base, const size_t stride, const int row, const int col )
{
	linePtr( base, stride, row )[col] = value;
}

template<class T>
inline void getPixel( cvt::Vector4f &value, const T *base, const size_t stride, const int row, const int col )
{
	value.x = getPixel( base, stride, row, col * 4 + 0 );
	value.y = getPixel( base, stride, row, col * 4 + 1 );
	value.z = getPixel( base, stride, row, col * 4 + 2 );
	value.w = getPixel( base, stride, row, col * 4 + 3 );
}

template<class T>
inline void setPixel( const cvt::Vector4f &value, const T *base, const size_t stride, const int row, const int col )
{
	setPixel<uint8_t>( (uint8_t) value.x, base, stride, row, col * 4 + 0 );
	setPixel<uint8_t>( (uint8_t) value.y, base, stride, row, col * 4 + 1 );
	setPixel<uint8_t>( (uint8_t) value.z, base, stride, row, col * 4 + 2 );
	setPixel<uint8_t>( (uint8_t) value.w, base, stride, row, col * 4 + 3 );
}

template<class T>
inline T clamp( T value, T a, T b )
{
	return ( value ) < a ? a : ( ( value ) > b ? b : ( value ) );
}

#define for_all( x ) \
	for ( size_t i = 0, __size = x.size(); i < __size; ++i )

#define for_count( x ) \
	for ( size_t i = 0; i < x; ++i )

#define for_all_iterator( c, x ) \
	for ( c::iterator it = x.begin(); it != x.end(); ++it )

#define SAFE_DELETE( x ) if ( x ) { delete x; x = 0; }

#define DEBUG_IF( x ) if ( x ) \

#define debug_out( x ) std::cout << x << std::endl;

struct EllipseParams_t
{
	cvt::Vector2f Center;
	cvt::Vector2f AxesDirection[2];
	cvt::Vector2f AxesLength;

	float Orientation;
	float FitError;
};

template<class T>
void fitEllipse( EllipseParams_t &params, const T *begin, const T *end )
{
	typedef Eigen::Matrix<float, 6, 6> EigenMatrix6f;
	typedef Eigen::Matrix<float, 3, 3> EigenMatrix3f;
	typedef Eigen::Matrix<float, 3, 1> EigenVector3f;
	typedef Eigen::Matrix<float, 6, 1> EigenVector6f;

	EigenMatrix6f Scatter;
	float x2, y2, xy, x, y;
	float sx, sy, mx = 0, my = 0;

	const T *points = begin;
	size_t count = end - begin;

	float maxX = points[0].x, maxY = points[0].y;
	float minX = points[0].x, minY = points[0].y;

	for_count( count )
	{
		x 	= points[i].x;
		y 	= points[i].y;

		mx += x;
		my += y;

		minX = std::min( minX, x );
		maxX = std::max( maxX, x );

		minY = std::min( minY, y );
		maxY = std::max( maxY, y );
	}

	mx = mx / count;
	my = my / count;

	sx = (maxX-minX)/2.0f;
	sy = (maxY-minY)/2.0f;

	// compute upper scatter matrix
	Scatter << 0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0;

	for_count( count )
	{
		x 	= (points[i].x - mx) / sx;
		y 	= (points[i].y - my) / sy;

		x2	= x * x;
		y2	= y * y;
		xy 	= x * y;

		Scatter( 0 , 0 ) += x2 * x2;
		Scatter( 0 , 1 ) += x2 * xy;
		Scatter( 0 , 2 ) += x2 * y2;
		Scatter( 0 , 3 ) += x2 * x;
		Scatter( 0 , 4 ) += x2 * y;
		Scatter( 0 , 5 ) += x2 * 1.0f;

		Scatter( 1 , 1 ) += xy * xy;
		Scatter( 1 , 2 ) += xy * y2;
		Scatter( 1 , 3 ) += xy * x;
		Scatter( 1 , 4 ) += xy * y;
		Scatter( 1 , 5 ) += xy * 1.0f;

		Scatter( 2 , 2 ) += y2 * y2;
		Scatter( 2 , 3 ) += y2 * x;
		Scatter( 2 , 4 ) += y2 * y;
		Scatter( 2 , 5 ) += y2 * 1.0f;

		Scatter( 3 , 3 ) += x * x;
		Scatter( 3 , 4 ) += x * y;
		Scatter( 3 , 5 ) += x * 1.0f;

		Scatter( 4 , 4 ) += y * y;
		Scatter( 4 , 5 ) += y * 1.0f;

		Scatter( 5 , 5 ) += 1.0f * 1.0f;
	}

	// make matrix symmetric

	Scatter( 1 , 0 ) = Scatter( 0 , 1 );
	Scatter( 2 , 0 ) = Scatter( 0 , 2 );
	Scatter( 3 , 0 ) = Scatter( 0 , 3 );
	Scatter( 4 , 0 ) = Scatter( 0 , 4 );
	Scatter( 5 , 0 ) = Scatter( 0 , 5 );

	Scatter( 2 , 1 ) = Scatter( 1 , 2 );
	Scatter( 3 , 1 ) = Scatter( 1 , 3 );
	Scatter( 4 , 1 ) = Scatter( 1 , 4 );
	Scatter( 5 , 1 ) = Scatter( 1 , 5 );

	Scatter( 3 , 2 ) = Scatter( 2 , 3 );
	Scatter( 4 , 2 ) = Scatter( 2 , 4 );
	Scatter( 5 , 2 ) = Scatter( 2 , 5 );

	Scatter( 4 , 3 ) = Scatter( 3 , 4 );
	Scatter( 5 , 3 ) = Scatter( 3 , 5 );

	Scatter( 5 , 4 ) = Scatter( 4 , 5 );

	// build up constraint matrix
	EigenMatrix6f C;

	C << 0.0f, 0.0f, -2.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 -2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f;

	EigenMatrix3f tmpA;

	tmpA << Scatter(0,0), Scatter(0,1), Scatter(0,2),
			Scatter(1,0), Scatter(1,1), Scatter(1,2),
			Scatter(2,0), Scatter(2,1), Scatter(2,2);

	EigenMatrix3f tmpB;

	tmpB << Scatter(0,3), Scatter(0,4), Scatter(0,5),
			Scatter(1,3), Scatter(1,4), Scatter(1,5),
			Scatter(2,3), Scatter(2,4), Scatter(2,5);


	EigenMatrix3f tmpC;

	tmpC << Scatter(3,3), Scatter(3,4), Scatter(3,5),
			Scatter(4,3), Scatter(4,4), Scatter(4,5),
			Scatter(5,3), Scatter(5,4), Scatter(5,5);

	EigenMatrix3f tmpD;

	tmpD << C(0,0), C(0,1), C(0,2),
			C(1,0), C(1,1), C(1,2),
			C(2,0), C(2,1), C(2,2);

	EigenMatrix3f tmpE = tmpC.inverse() * tmpB.transpose();
	EigenMatrix3f A = tmpD.inverse() * ( tmpA - tmpB * tmpE );

	Eigen::SelfAdjointEigenSolver<EigenMatrix3f> solver( A );
	Eigen::SelfAdjointEigenSolver<EigenMatrix3f>::MatrixType eigenVectors = solver.eigenvectors();

	EigenVector3f top( eigenVectors( 0, 0 ), eigenVectors( 1, 0 ), eigenVectors( 2, 0 ) );
	EigenVector3f bottom = -tmpE * top;

	EigenVector6f parNorm;

	parNorm << top(0), top(1), top(2),
			bottom(0), bottom(1), bottom(2);

	// unnormalize system parameters
	EigenVector6f par;

	par(0) = parNorm(0)*sy*sy;
	par(1) = parNorm(1)*sx*sy;
	par(2) = parNorm(2)*sx*sx;
	par(3) = -2.0f*parNorm(0)*sy*sy*mx - parNorm(1)*sx*sy*my + parNorm(3)*sx*sy*sy;
	par(4) = -parNorm(1)*sx*sy*mx - 2.0f*top(2)*sx*sx*my + parNorm(4)*sx*sx*sy;
	par(5) = parNorm(0)*sy*sy*mx*mx + parNorm(1)*sx*sy*mx*my + parNorm(2)*sx*sx*my*my
			- parNorm(3)*sx*sy*sy*mx - parNorm(4)*sx*sx*sy*my
			+ parNorm(5)*sx*sx*sy*sy;

	// compute ellipse parameters

	float thetarad = 0.5*atan2(double(par(1)), double(par(0) - par(2)));
	float cost = cos(thetarad);
	float sint = sin(thetarad);
	float sin_squared = sint*sint;
	float cos_squared = cost*cost;
	float cos_sin = sint * cost;

	float Ao =  par(5);
	float Au =  par(3) * cost + par(4) * sint;
	float Av = -par(3) * sint + par(4) * cost;
	float Auu = par(0) * cos_squared + par(2) * sin_squared + par(1) * cos_sin;
	float Avv = par(0) * sin_squared + par(2) * cos_squared - par(1) * cos_sin;

	float tuCentre = - Au/(2*Auu);
	float tvCentre = - Av/(2*Avv);
	float wCentre = Ao - Auu*tuCentre*tuCentre - Avv*tvCentre*tvCentre;

	params.Center.x = tuCentre * cost - tvCentre * sint;
	params.Center.y = tuCentre * sint + tvCentre * cost;

	float Ru = -wCentre/Auu;
	float Rv = -wCentre/Avv;

	params.AxesLength.x = sqrt(abs(Ru));
	params.AxesLength.y = sqrt(abs(Rv));

	params.Orientation = thetarad;

	params.AxesDirection[0].x = cost;
	params.AxesDirection[0].y = sint;

	params.AxesDirection[1].x = -sint;
	params.AxesDirection[1].y = cost;

	// compute fit error

	params.FitError = parNorm.transpose() * Scatter * parNorm;
	params.FitError /= count;
}

#endif // UTILS_H
