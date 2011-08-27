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

