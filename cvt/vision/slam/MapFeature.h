/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_MAP_FEATURE_H
#define CVT_MAP_FEATURE_H

#include <Eigen/Core>
#include <set>
#include <cvt/io/xml/XMLNode.h>
#include <cvt/io/xml/XMLSerializable.h>
#include <cvt/io/xml/XMLElement.h>
#include <cvt/io/xml/XMLText.h>
#include <cvt/util/EigenBridge.h>

namespace cvt
{
	class MapFeature : public XMLSerializable
	{
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW

			typedef std::set<size_t>::const_iterator ConstPointTrackIterator;

			MapFeature( const Eigen::Vector4d & p, const Eigen::Matrix4d & covariance );
			MapFeature();

			Eigen::Vector4d&		estimate()		   { return _point; }
			const Eigen::Vector4d&	estimate()   const { return _point; }
			Eigen::Matrix4d&		covariance()	   { return _covariance; }
			const Eigen::Matrix4d&	covariance() const { return _covariance; }

			ConstPointTrackIterator pointTrackBegin() const	{ return _pointTrack.begin(); };
			ConstPointTrackIterator pointTrackEnd() const	{ return _pointTrack.end();   };

			void addPointTrack( size_t camId ) { _pointTrack.insert( camId ); }
			
			XMLNode* serialize() const;
			void     deserialize( XMLNode* node );

		private:
			Eigen::Vector4d		_point;
			Eigen::Matrix4d		_covariance;

			// camera ids which have measurements of this point
			std::set<size_t>	_pointTrack;

	};
			
	inline MapFeature::MapFeature( const Eigen::Vector4d & p, 
								   const Eigen::Matrix4d & covariance ):
		_point( p ),
		_covariance( covariance )
	{
	}

	inline MapFeature::MapFeature():
		_point( Eigen::Vector4d::Zero() ),
		_covariance( Eigen::Matrix4d::Identity() )
	{
	}

	inline XMLNode* MapFeature::serialize() const
	{
		XMLElement* mf = new XMLElement( "MapFeature" );

		XMLElement* n;

		Vector4d p;
		EigenBridge::toCVT( p, _point );
		n = new XMLElement( "Point3d" );
		n->addChild( new XMLText( p.toString() ) );
		mf->addChild( n );

		Matrix4d cov;
		EigenBridge::toCVT( cov, _covariance );
		n = new XMLElement( "Covariance" );
		n->addChild( new XMLText( cov.toString() ) );
		mf->addChild( n );

		n = new XMLElement( "PointTrack" );
		std::set<size_t>::const_iterator it = _pointTrack.begin();
		const std::set<size_t>::const_iterator itEnd = _pointTrack.end();

		String val;
		while( it != itEnd ){
			XMLElement* id = new XMLElement( "KeyframeId" );
			val.sprintf( "%d", *it );
			id->addChild( new XMLText( val ) );
			n->addChild( id );
			++it;
		}
		mf->addChild( n );

		return mf;
	}

	inline void MapFeature::deserialize( XMLNode* node )
	{
		if( node->name() != "MapFeature" ){
			throw CVTException( "This is not a MapFeature node!" );
		}

		Vector4d p = Vector4d::fromString( node->childByName( "Point3d" )->child( 0 )->value() );
		EigenBridge::toEigen( _point, p );
		
		Matrix4d cov = Matrix4d::fromString( node->childByName( "Covariance" )->child( 0 )->value() );
		EigenBridge::toEigen( _covariance, cov );

		XMLNode* n = node->childByName( "PointTrack" );
		for( size_t i = 0; i < n->childSize(); i++ ){
			XMLNode* kfNode = n->child( i );
			_pointTrack.insert( kfNode->child( 0 )->value().toInteger() );
		}
	}

}

#endif
