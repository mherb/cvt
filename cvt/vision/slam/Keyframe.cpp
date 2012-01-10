#include <cvt/vision/slam/Keyframe.h>

#include <cvt/io/xml/XMLNode.h>
#include <cvt/io/xml/XMLElement.h>
#include <cvt/io/xml/XMLAttribute.h>

namespace cvt
{
	Keyframe::Keyframe( const Eigen::Matrix4d& pose ) 
	{
		_pose.set( pose );
	}

	Keyframe::Keyframe()
	{
	} 

	double Keyframe::distance( const Eigen::Matrix4d & transform ) const
	{
		const Eigen::Matrix4d & poseMat = _pose.transformation();
		return ( transform.block<3, 1>( 0, 3 ) - poseMat.block<3, 1>( 0, 3 ) ).squaredNorm();
	}


	void Keyframe::deserialize( XMLNode* node )
	{
		// TODO: 
		if( node->name() != "Keyframe" )
			throw CVTException( "this is not a Keyframe node" );

		XMLNode * n;
		n = node->childByName( "Pose" );
		if( n != NULL ){
			Matrix4d m = Matrix4d::fromString( n->child( 0 )->value() );
			Eigen::Matrix4d eM;
			EigenBridge::toEigen( eM, m );
			_pose.set( eM );
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

		XMLElement* element;

		// the pose:
		element = new XMLElement( "Pose" );
		Matrix4d mat;
		EigenBridge::toCVT( mat, _pose.transformation() );
		element->addChild( new XMLText( mat.toString() ) );
		node->addChild( element );

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
