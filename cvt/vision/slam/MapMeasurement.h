#ifndef CVT_MAP_MEASUREMENT_H
#define CVT_MAP_MEASUREMENT_H

#include <Eigen/Core>
#include <cvt/io/xml/XMLSerializable.h>
#include <cvt/io/xml/XMLNode.h>
#include <cvt/io/xml/XMLElement.h>
#include <cvt/io/xml/XMLText.h>

#include <cvt/util/EigenBridge.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>

namespace cvt
{
	class MapMeasurement : public XMLSerializable
	{
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			MapMeasurement() :
				point( 0, 0 ),
				information( Eigen::Matrix2d::Identity() )
			{
			}

			Eigen::Vector2d	point;

			// information is inverse covariance
			Eigen::Matrix2d	information;

			XMLNode* serialize() const
			{
				XMLElement* mapMeas = new XMLElement( "PointMeasurement" );

				XMLElement* n = new XMLElement( "Point2d" );
				Vector2d p2d;
				EigenBridge::toCVT( p2d, point );
				n->addChild( new XMLText( p2d.toString() ) );
				mapMeas->addChild( n );

				n = new XMLElement( "Information" );
				Matrix2d inf;
				EigenBridge::toCVT( inf, information );
				n->addChild( new XMLText( inf.toString() ) );

				mapMeas->addChild( n );

				return mapMeas;
			}

			void deserialize( XMLNode* node )
			{
				if( node->name() != "PointMeasurement" )
					throw CVTException( "this is not a PointMeasurement node" );

				XMLNode * n;
				n = node->childByName( "Point2d" );
				if( n != NULL ){
					Vector2d v = Vector2d::fromString( n->child( 0 )->value() );
					EigenBridge::toEigen( point, v );
				}

				n = node->childByName( "Information" );
				if( n != NULL ){
					Matrix2d m = Matrix2d::fromString( n->child( 0 )->value() );
					EigenBridge::toEigen( information, m );
				}
			}
	};
}

#endif
