/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/Keyframe.h>

#include <cvt/io/FileSystem.h>
#include <cvt/io/xml/XMLNode.h>
#include <cvt/io/xml/XMLElement.h>
#include <cvt/io/xml/XMLAttribute.h>

namespace cvt
{
	Keyframe::Keyframe( const Eigen::Matrix4d& pose, size_t id ) :
		_id( id ),
		_img( 0 )
	{
		_pose.set( pose );
	}

	Keyframe::Keyframe() :
		_id( 0 ),
		_img( 0 )
	{
	} 
			
	Keyframe::~Keyframe()
	{
		if( _img )
			delete _img;
	}
			
	Keyframe::Keyframe( const Keyframe & other ) :
		XMLSerializable(),
		_id( other._id ),
		_pose( other._pose ),
		_img( 0 ),
		_featMeas( other._featMeas )
	{
		if( other._img )
			_img = new Image( *other._img );
	}

	void Keyframe::setImage( const Image& img )
	{
		if( !hasImage() ){
			_img = new Image( img );
		} else {
			*_img = img;
		}
	}

	const Image & Keyframe::image() const	
	{
		return *_img;
	}

	bool Keyframe::hasImage() const 
	{
		return ( _img != 0 );
	}

	double Keyframe::distance( const Eigen::Matrix4d & transform ) const
	{
		const Eigen::Matrix4d & poseMat = _pose.transformation();
		double dist = ( transform.block<3, 1>( 0, 3 ) - poseMat.block<3, 1>( 0, 3 ) ).norm();
	
		const Eigen::Matrix3d & poseR = poseMat.block<3, 3>( 0, 0 );
		const Eigen::Matrix3d & kfR   = transform.block<3, 3>( 0, 0 );
		Eigen::Matrix3d deltaR = poseR.transpose() * kfR;
		Eigen::Vector3d euler = deltaR.eulerAngles( 0, 1, 2 );
		dist += euler.norm();

		return dist;
	}


	void Keyframe::deserialize( XMLNode* node )
	{
		// TODO: 
		if( node->name() != "Keyframe" ){
			throw CVTException( "this is not a Keyframe node" );
		}

		_id = node->childByName( "id" )->value().toInteger();

		XMLNode * n;
		n = node->childByName( "Pose" );
		if( n != NULL ){
			Matrix4d m = Matrix4d::fromString( n->child( 0 )->value() );
			Eigen::Matrix4d eM;
			EigenBridge::toEigen( eM, m );
			_pose.set( eM );
		}

		n = node->childByName( "Image" );
		if( n ){
			_img = new Image();
			String fileName = n->childByName( "file" )->value();
			std::cout << "Loading file: " << fileName << std::endl;
			_img->load( fileName );
		}

		// the measurements:
		n = node->childByName( "Measurements" );
		size_t numMeas = n->childSize();
		MapMeasurement mm;
		_featMeas.clear();
		for( size_t i = 0; i < numMeas; i++ ){
			XMLNode* mapMeasNode = n->child( i );
			size_t measId = mapMeasNode->childByName( "featureId" )->value().toInteger();

			XMLNode* measNode = mapMeasNode->childByName( "PointMeasurement" );
			mm.deserialize( measNode );

			addFeature( mm, measId );			
		}
	}

	XMLNode* Keyframe::serialize() const
	{
		XMLElement* node = new XMLElement( "Keyframe" );
		String kfId; kfId.sprintf( "%d", _id );
		XMLAttribute* attr = new XMLAttribute( "id", kfId );
		node->addChild( attr );

		XMLElement* element;
		// the pose:
		element = new XMLElement( "Pose" );
		Matrix4d mat;
		EigenBridge::toCVT( mat, _pose.transformation() );
		element->addChild( new XMLText( mat.toString() ) );
		node->addChild( element );

		if( hasImage() ){
			if( !FileSystem::exists( "keyframe_images" ) ){
				FileSystem::mkdir( "keyframe_images" );
			}
			String filename;
			filename.sprintf( "keyframe_images/keyframe_%06d.cvtraw", _id );
			_img->save( filename );
			element = new XMLElement( "Image" );
			element->addChild( new XMLAttribute( "file", filename ) );
			node->addChild( element );
		}

		// measurements:
		XMLElement* meas = new XMLElement( "Measurements" );
		MeasurementIterator it = _featMeas.begin();
		const MeasurementIterator itEnd = _featMeas.end();
		
		XMLAttribute* featId;
		String val;
		while( it != itEnd ){
			element = new XMLElement( "Measurement" );

			// the id:
			val.sprintf( "%d", it->first );			
			featId = new XMLAttribute( "featureId", val );
			element->addChild( featId );
			element->addChild( it->second.serialize() );

			meas->addChild( element );

			++it;
		}

		node->addChild( meas );

		return node;
	}
}
