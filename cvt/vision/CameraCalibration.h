/*
 * File:   CameraCalibration.h
 * Author: sebi
 *
 * Created on August 8, 2011, 11:09 AM
 */

#ifndef CVT_CAMERACALIBRATION_H
#define	CVT_CAMERACALIBRATION_H

#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>
#include <cvt/util/Flags.h>
#include <cvt/geom/Rect.h>
#include <cvt/math/Polynomial.h>
#include <cvt/io/xml/XMLElement.h>
#include <cvt/io/xml/XMLText.h>
#include <cvt/io/xml/XMLSerializable.h>

namespace cvt
{

    enum CamCalibFlagTypes {
        INTRINSICS = ( 1 << 0 ),
        EXTRINSICS = ( 1 << 1 ),
        DISTORTION = ( 1 << 2 )
	};

	CVT_ENUM_TO_FLAGS( CamCalibFlagTypes, CameraCalibrationFlags )


    class CameraCalibration : public XMLSerializable
    {
      public:
        CameraCalibration();
        CameraCalibration( const CameraCalibration & other );

        const Matrix3f & intrinsics()           const { return _intrinsics; }
        const Matrix4f & extrinsics()           const { return _extrinsics; }
        const Vector3f & radialDistortion()     const { return _radial; }
        const Vector2f & tangentialDistortion() const { return _tangential; }
        const Matrix4f & projectionMatrix()     const { return _projection; }
        const CameraCalibrationFlags & flags()  const { return _flags; }

        /**
         * set the extrinsics of the camera
         * @param extr Transformation from Camera to World!
         */
        void setExtrinsics( const Matrix4f & extr );

        void setIntrinsics( const Matrix3f & intr );
        void setIntrinsics( float fx, float fy, float cx, float cy, float alpha = 0.0f );

        void setDistortion( const Vector3f & radial, const Vector2f & tangential );
		Vector2f undistortPoint( const Vector2f& in ) const;
		void calcUndistortRects( Rectf& minvalid, Rectf& max, const Rectf& input ) const;

        bool hasExtrinsics() const { return ( _flags & EXTRINSICS ); }
        bool hasIntrinsics() const { return ( _flags & INTRINSICS ); }
        bool hasDistortion() const { return ( _flags & DISTORTION ); }

        // de-/serialization interface
        void	 deserialize( XMLNode* node );
        XMLNode* serialize() const;

      private:
        Matrix3f                _intrinsics;
        Matrix4f                _extrinsics;
        Matrix4f                _projection;

        // Distortion parameters
        Vector3f                _radial;
        Vector2f                _tangential;

        CameraCalibrationFlags  _flags;

        void updateProjectionMatrix();

    };

    inline CameraCalibration::CameraCalibration() :
		XMLSerializable()
    {
        _intrinsics.setIdentity();
        _extrinsics.setIdentity();
        _projection.setIdentity();
        _radial.setZero();
        _tangential.setZero();
    }

    inline CameraCalibration::CameraCalibration( const CameraCalibration & other ) :
		XMLSerializable(),
        _intrinsics( other._intrinsics ),
        _extrinsics( other._extrinsics ),
        _projection( other._projection ),
        _radial( other._radial ),
        _tangential( other._tangential ),
        _flags( other._flags )
    {
    }

    inline XMLNode* CameraCalibration::serialize() const
    {
        XMLNode * root;

        root = new XMLElement( "CameraCalibration" );

        if( hasExtrinsics() ){
            XMLElement * element = new XMLElement( "Extrinsics" );
            element->addChild( new XMLText( _extrinsics.toString() ) );
            root->addChild( element );
        }

        if( hasIntrinsics() ){
            XMLElement * element = new XMLElement( "Intrinsics" );
            element->addChild( new XMLText( _intrinsics.toString() ) );
            root->addChild( element );
        }

        if( hasDistortion() ){
            XMLElement * element = new XMLElement( "Distortion" );
            XMLElement * rDist = new XMLElement( "Radial" );
            rDist->addChild( new XMLText( _radial.toString() ) );
            element->addChild( rDist );

            XMLElement * tDist = new XMLElement( "Tangential" );
            tDist->addChild( new XMLText( _tangential.toString() ) );
            element->addChild( tDist );

            root->addChild( element );
        }

        return root;
    }

    inline void CameraCalibration::deserialize( XMLNode* node )
    {
        if( node->name() != "CameraCalibration" )
            throw CVTException( "this is not a camera calibration node" );
        XMLNode * n;
        n = node->childByName( "Extrinsics" );
        if( n != NULL ){
            Matrix4f mat = Matrix4f::fromString( n->child( 0 )->value() );
            setExtrinsics( mat );
        }

        n = node->childByName( "Intrinsics" );
        if( n != NULL ){
            Matrix3f intr = Matrix3f::fromString( n->child( 0 )->value() );
            setIntrinsics( intr );
        }

        n = node->childByName( "Distortion" );
        if( n != NULL ){
            XMLNode * child;
            Vector3f radial;
            Vector2f tangential;

			/* to make the compiler happy */
			radial.setZero();
			tangential.setZero();

            for( size_t i = 0; i < n->childSize(); i++ ){
                child = n->child( i );

                if( child->name() == "Radial"){
                    radial = Vector3f::fromString( child->child( 0 )->value() );
                } else if( child->name() == "Tangential" ){
                    tangential = Vector2f::fromString( child->child( 0 )->value() );
                } else {
                    String message( "Unkown child type: " );
                    message += child->name();
                    throw CVTException( message.c_str() );
                }
            }

            setDistortion( radial, tangential );
        }
    }

    inline void CameraCalibration::setExtrinsics( const Matrix4f & extr )
    {
        _flags |= EXTRINSICS;
        _extrinsics = extr;
        updateProjectionMatrix();
    }

    inline void CameraCalibration::setIntrinsics( const Matrix3f & intr )
    {
        _flags |= INTRINSICS;
        _intrinsics = intr;
        updateProjectionMatrix();
    }

    inline void CameraCalibration::setIntrinsics( float fx, float fy, float cx, float cy, float alpha )
    {
        _flags |= INTRINSICS;
        _intrinsics[ 0 ][ 0 ] = fx;
        _intrinsics[ 0 ][ 1 ] = alpha;
        _intrinsics[ 0 ][ 2 ] = cx;
        _intrinsics[ 1 ][ 0 ] = 0.0f;
        _intrinsics[ 1 ][ 1 ] = fy;
        _intrinsics[ 1 ][ 2 ] = cy;
        _intrinsics[ 2 ][ 0 ] = 0.0f;
        _intrinsics[ 2 ][ 1 ] = 0.0f;
        _intrinsics[ 2 ][ 2 ] = 1.0f;
        updateProjectionMatrix();
    }


    inline void CameraCalibration::setDistortion( const Vector3f & radial, const Vector2f & tangential )
    {
        _flags |= DISTORTION;
        _radial = radial;
        _tangential = tangential;
    }

	inline Vector2f CameraCalibration::undistortPoint( const Vector2f& in ) const
	{
		Vector2f out;
		Vector2f p = in;
		Vector2f c = Vector2f( _intrinsics[ 0 ][ 2 ], _intrinsics[ 1 ][ 2 ] );
		Vector2f f = Vector2f( _intrinsics[ 0 ][ 0 ], _intrinsics[ 1 ][ 1 ] );
		p -= c;
		p /= f;
		float r2 = p.lengthSqr();
		float r4 = Math::sqr( r2 );
		float r6 = r2 * r4;
		float poly = ( 1.0f + _radial[ 0 ] * r2 + _radial[ 1 ] * r4 + _radial[ 2 ] * r6 );
		float xy2 = 2.0f * _tangential[ 0 ] * _tangential[ 1 ];
		out.x = f.x * ( p.x * poly + xy2 * _tangential[ 0 ] + _tangential[ 1 ] * ( r2 + 2.0f * p.x ) ) + c.x;
		out.y = f.y * ( p.y * poly + xy2 * _tangential[ 1 ] + _tangential[ 0 ] * ( r2 + 2.0f * p.y ) ) + c.y;
		return out;
	}

	inline void CameraCalibration::calcUndistortRects( Rectf& min, Rectf& max, const Rectf& input ) const
	{
		Vector2f pt;
		float y ,x;
		float x1min, x2min, y1min, y2min;
		float x1max, x2max, y1max, y2max;

		x1min = x1max = input.x;
		x2min = x2max = input.x + input.width;
		y1min = y1max = input.y;
		y2min = y2max = input.y + input.height;

		pt = undistortPoint( Vector2f( input.x, input.y ) );
		x1min = Math::max( x1min, pt.x );
		y1min = Math::max( y1min, pt.y );
		x1max = Math::min( x1max, pt.x );
		y1max = Math::min( y1max, pt.y );

		pt = undistortPoint( Vector2f( input.x + input.width, input.y ) );
		x2min = Math::min( x2min, pt.x );
		y1min = Math::max( y1min, pt.y );
		x2max = Math::max( x2max, pt.x );
		y1max = Math::min( y1max, pt.y );

		pt = undistortPoint( Vector2f( input.x + input.width, input.y + input.height ) );
		x2min = Math::min( x2min, pt.x );
		y2min = Math::min( y2min, pt.y );
		x2max = Math::max( x2max, pt.x );
		y2max = Math::max( y2max, pt.y );

		pt = undistortPoint( Vector2f( input.x, input.y + input.height ) );
		x1min = Math::max( x1min, pt.x );
		y2min = Math::min( y2min, pt.y );
		x1max = Math::min( x1max, pt.x );
		y2max = Math::max( y2max, pt.y );

		return;

		Vector2f c = Vector2f( _intrinsics[ 0 ][ 2 ], _intrinsics[ 1 ][ 2 ] );

		std::cout << c << std::endl;

		Vector2f f = Vector2f( _intrinsics[ 0 ][ 0 ], _intrinsics[ 1 ][ 1 ] );
		c /= f;

		std::vector<Complexf> roots;
		Polynomialf deriv;
		// get roots of x derivative-polynomial for y equal to input.y and input.y + input.height
		Polynomialf rx2, rx4, rx6, polyrx, allx;

		y = ( input.y ) / f.y;
		rx2 = Polynomialf( 1.0f, -2.0f * c.x, y * y - 2.0f * c.y * y + c.y * c.y + c.x * c.x );
		rx4 = rx2 * rx2;
		rx6 = rx4 * rx2;
		polyrx = _radial[ 0 ] * rx2 + _radial[ 1 ] * rx4 + _radial[ 2 ] * rx6;
		polyrx[ 0 ] += 1.0f;
		allx = f.y * ( ( y - c.y ) * polyrx + Polynomialf( 2.0f * y * _tangential[ 1 ], 0.0f ) + _tangential[ 0 ] * rx2 /* + tangential[ 0 ] * 2.0f * ( y - c.y ) */ );
		allx[ 0 ] += _intrinsics[ 1 ][ 2 ];
		deriv = allx.derivative();
		deriv.roots( roots );
		for( int i = 0; i < roots.size(); i++ ) {
			if( roots[ i ].im == 0.0f ) {
				pt.x = roots[ i ].re * f.x;
				pt.y = input.y;
				pt = undistortPoint( pt );
				y1min = Math::max( y1min, pt.y );
				y1max = Math::min( y1max, pt.y );
				//std::cout << roots[ i ].re * f.x << std::endl;
			}
		}

		y = ( input.y + input.height ) / f.y;
		rx2 = Polynomialf( 1.0f, -2.0f * c.x, y * y - 2.0f * c.y * y + c.y * c.y + c.x * c.x );
		rx4 = rx2 * rx2;
		rx6 = rx4 * rx2;
		polyrx = _radial[ 0 ] * rx2 + _radial[ 1 ] * rx4 + _radial[ 2 ] * rx6;
		polyrx[ 0 ] += 1.0f;
		allx = f.y * ( ( y - c.y ) * polyrx + Polynomialf( 2.0f * y * _tangential[ 1 ], 0.0f ) + _tangential[ 0 ] * rx2 /* + tangential[ 0 ] * 2.0f * ( y - c.y ) */ );
		allx[ 0 ] += _intrinsics[ 1 ][ 2 ];
		deriv = allx.derivative();
		deriv.roots( roots );
		for( int i = 0; i < roots.size(); i++ ) {
			if( roots[ i ].im == 0.0f ) {
				pt.x = roots[ i ].re * f.x;
				pt.y = input.y + input.height;
				pt = undistortPoint( pt );
				y2min = Math::min( y2min, pt.y );
				y2max = Math::max( y2max, pt.y );
				//std::cout << roots[ i ].re * f.x << std::endl;
			}
		}


		// get roots of y derivative-polynomial for x input.x and input.x + input.width
		Polynomialf ry2, ry4, ry6, polyry, ally;
		x = ( input.x ) / f.x;
		ry2 = Polynomialf( 1.0f, -2.0f * c.y, x * x - 2.0f * c.x * x + c.x * c.x + c.y * c.y );
		ry4 = ry2 * ry2;
		ry6 = ry4 * ry2;
		polyry = _radial[ 0 ] * ry2 + _radial[ 1 ] * ry4 + _radial[ 2 ] * ry6;
		polyry[ 0 ] += 1.0f;
		ally = f.x * ( ( x - c.x ) * polyry + Polynomialf( 2.0f * x * _tangential[ 0 ], 0.0f ) + _tangential[ 1 ] * ry2 /* + tangential[ 1 ] * 2.0f * ( x - c.x ) */ );
		ally[ 0 ] += _intrinsics[ 0 ][ 2 ];
		deriv = ally.derivative();
		deriv.roots( roots );
		for( int i = 0; i < roots.size(); i++ ) {
			if( roots[ i ].im == 0.0f ) {
				pt.x = input.x;
				pt.y = roots[ i ].re * f.y;
				pt = undistortPoint( pt );
				x1min = Math::max( x1min, pt.x );
				x1max = Math::min( x1max, pt.x );
				//std::cout << roots[ i ].re * f.y << std::endl;
			}
		}

		x = ( input.x + input.width ) / f.x;
		ry2 = Polynomialf( 1.0f, -2.0f * c.y, x * x - 2.0f * c.x * x + c.x * c.x + c.y * c.y );
		ry4 = ry2 * ry2;
		ry6 = ry4 * ry2;
		polyry = _radial[ 0 ] * ry2 + _radial[ 1 ] * ry4 + _radial[ 2 ] * ry6;
		polyry[ 0 ] += 1.0f;
		ally = f.x * ( ( x - c.x ) * polyry + Polynomialf( 2.0f * x * _tangential[ 0 ], 0.0f ) + _tangential[ 1 ] * ry2 /* + tangential[ 1 ] * 2.0f * ( x - c.x ) */ );
		ally[ 0 ] += _intrinsics[ 0 ][ 2 ];
		deriv = ally.derivative();
		deriv.roots( roots );
		for( int i = 0; i < roots.size(); i++ ) {
			if( roots[ i ].im == 0.0f ) {
				pt.x = input.x + input.width;
				pt.y = roots[ i ].re * f.y;
				pt = undistortPoint( pt );
				x2min = Math::min( x1min, pt.x );
				x2max = Math::max( x1max, pt.x );
				//std::cout << roots[ i ].re * f.y << std::endl;
			}
		}

		min.x = x1min;
		min.y = y1min;
		min.width = x2min - x1min;
		min.height = y2min - y1min;

		max.x = x1max;
		max.y = y1max;
		max.width = x2max - x1max;
		max.height = y2max - y1max;
	}

    inline void CameraCalibration::updateProjectionMatrix()
    {
        // if intrinsics are set, this will copy it to projection,
        // if not, it will be I_4x4!
        for( size_t i = 0; i < 3; i++ )
            for( size_t k = 0; k < 3; k++ )
                _projection[ i ][ k ] = _intrinsics[ i ][ k ];
        _projection[ 0 ][ 3 ] = _projection[ 1 ][ 3 ] = _projection[ 2 ][ 3 ] = 0.0f;
        _projection[ 3 ][ 0 ] = _projection[ 3 ][ 1 ] = _projection[ 3 ][ 2 ] = 0.0f;
        _projection[ 3 ][ 3 ] = 1.0f;


        if( hasExtrinsics() )
            _projection *= _extrinsics;
    }
}

#endif

