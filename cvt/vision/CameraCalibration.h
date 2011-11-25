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

	inline void CameraCalibration::calcUndistortRects( Rectf& minvalid, Rectf& max, const Rectf& input ) const
	{
		Vector2f corners[ 4 ];
		Vector2f extremes[ 4 ];

		corners[ 0 ] = undistortPoint( Vector2f( input.x, input.y ) );
		corners[ 1 ] = undistortPoint( Vector2f( input.x + input.width, input.y ) );
//		corners[ 2 ] = undistortPoint( Vector2f( input.x + input.width, input.y + input.height ) );
//		corners[ 3 ] = undistortPoint( Vector2f( input.x, input.y + input.height ) );

		Vector2f c = Vector2f( _intrinsics[ 0 ][ 2 ], _intrinsics[ 1 ][ 2 ] );
		Vector2f f = Vector2f( _intrinsics[ 0 ][ 0 ], _intrinsics[ 1 ][ 1 ] );
		c /= f;

		float y = input.y / f.y;
		Polynomialf rx2( 1.0f, -2.0f * c.x, y * y - 2.0f * c.y * y + c.y * c.y + c.x * c.x );
		Polynomialf rx4 = rx2 * rx2;
		Polynomialf rx6 = rx4 * rx2;
		Polynomialf polyk = _radial[ 0 ] * rx2 + _radial[ 1 ] * rx4 + _radial[ 2 ] * rx6;
		polyk[ 0 ] += 1.0f;
		Polynomialf px( 1.0f, -c.x );
		Polynomialf all = f.x * ( px * polyk + Polynomialf( 2.0f * y * _tangential[ 0 ], 0.0f ) + _tangential[ 1 ] * rx2 + _tangential[ 1 ] * 2.0f * px );
		all[ 0 ] += _intrinsics[ 0 ][ 2 ];

		std::cout << all.eval( input.x ) << " " << corners[ 0 ] << std::endl;
		std::cout << all.eval( ( input.x + input.width ) / f.x ) << " " << corners[ 1 ] << std::endl;

		Polynomialf deriv = all.derivative();
		std::vector<Complexf> roots;
		deriv.roots( roots );
		for( int i = 0; i < roots.size(); i++ )
			std::cout << roots[ i ] * f.x << std::endl;
		std::cout << "Degree: " << deriv.degree() << std::endl;
		std::cout << deriv << std::endl;


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

