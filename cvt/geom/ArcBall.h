#ifndef CVT_ARCBALL_H
#define CVT_ARCBALL_H

#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Quaternion.h>

namespace cvt {
	class ArcBall
	{
		public:
			ArcBall( size_t width = 1, size_t height = 1, float radius = 1.0f );
			~ArcBall();
			void setViewportSize( size_t width, size_t height );
			void getRotation( Matrix4f& mat, int x1, int y1, int x2, int y2 );

		private:
			ArcBall( const ArcBall& );
			void mapToSphere( Vector3f& vec, int x, int y );

			size_t _width;
			size_t _height;
			float  _radius;
	};

	inline ArcBall::ArcBall( size_t width, size_t height, float radius ) : _width( width ), _height( height ), _radius( radius )
	{
	}

	inline ArcBall::~ArcBall()
	{
	}

	inline void ArcBall::setViewportSize( size_t width, size_t height )
	{
		_width = width;
		_height = height;
	}

	inline void ArcBall::mapToSphere( Vector3f& vec, int _x, int _y )
	{
		vec[ 0 ] = -( ( ( float ) _x / ( float ) _width ) - 0.5f );
		vec[ 1 ] = ( ( float ) _y / ( float ) _height ) - 0.5f;

		float len = Math::sqr( vec[ 0 ] ) + Math::sqr( vec[ 1 ] );
		if( len < Math::sqr( _radius ) * 0.5f ) {
			vec[ 2 ]  = Math::sqrt( Math::sqr( _radius ) - len );
		} else {
			vec[ 2 ] = Math::sqr( _radius ) / ( 2.0f * Math::sqrt( len ) );
		}
	}

	inline void ArcBall::getRotation( Matrix4f& mat, int x1, int y1, int x2, int y2 )
	{
		Vector3f v1, v2, c;

		mapToSphere( v1, x1, y1 );
		mapToSphere( v2, x2, y2 );

		c.cross( v1, v2 );
		float lsqrt = c.lengthSqr();
		if( lsqrt > Math::EPSILONF ) {
			Quaternionf q( c[ 0 ], c[ 1 ], c[ 2 ], v1 * v2 );
			mat = q.toMatrix4();
		} else {
			mat.setIdentity();
		}
	}
}

#endif
