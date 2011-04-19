#ifndef CVT_FACESHAPE_H
#define CVT_FACESHAPE_H

#include <cvt/geom/PointSet.h>
#include <cvt/gfx/GFX.h>
#include <cvt/io/xml/XMLNode.h>
#include <cvt/io/xml/XMLElement.h>
#include <cvt/io/xml/XMLAttribute.h>
#include <cvt/io/xml/XMLSerializable.h>
#include <cvt/util/Exception.h>

namespace cvt {
	class FaceShape : public PointSet2f, public XMLSerializable
	{
		public:
			FaceShape();
			virtual ~FaceShape();
			void draw( GFX* g, const Matrix3f& t ) const;
			void deserialize( XMLNode* node );
			XMLNode* serialize() const;
		private:
			void drawLines( GFX* g, const PointSet2f& pts, int start, int size, bool loop = false ) const;
			void deserializeXMLFacePart( XMLNode* node, size_t offset, size_t pts );
			void serializeXMLFacePart( XMLNode* node, size_t offset, size_t pts ) const;
	};

	inline FaceShape::FaceShape()
	{
		resize( 85 );
	}

	inline FaceShape::~FaceShape()
	{
	}

	inline void FaceShape::draw( GFX* g, const Matrix3f& t ) const
	{
		PointSet2f pts( *this );
		pts.transform( t );
		/* outline */
		drawLines( g, pts , 0, 15 );
		/* mouth outer */
		drawLines( g, pts , 15, 12, true );
		/* mouth inner */
		drawLines( g, pts , 27, 8, true );
		/* left eye */
		drawLines( g, pts , 35, 8, true );
		/* right eye */
		drawLines( g, pts , 43, 8, true );
		/* left eye brow */
		drawLines( g, pts , 51, 10, true );
		/* right eye brow */
		drawLines( g, pts , 61, 10, true );
		/* nose */
		drawLines( g, pts , 71, 14 );
	}

	inline void FaceShape::drawLines( GFX* g, const PointSet2f& pts, int start, int size, bool loop ) const
	{
		for( int i = start, end = start + size - 1 ; i < end; i++ ) {
			g->drawLine( pts[ i ], pts[ i + 1 ] );
		}
		if( loop )
			g->drawLine( pts[ start + size - 1 ], pts[ start ] );
	}

	inline void FaceShape::deserialize( XMLNode* node )
	{
		XMLNode* tmp;
		XMLNode* tmp2;

		if( !node )
			throw CVTException( "No face node" );
		if( node->name() != "face" )
			throw CVTException( "No face node" );

		/* outline */
		if( !( tmp = node->childByName("outline")))
			throw CVTException( "No outline node" );
		deserializeXMLFacePart( tmp, 0, 15 );

		/* mouth */
		if( !( tmp = node->childByName("mouth")))
			throw CVTException( "No mouth node" );

			/* outer */
			if( !( tmp2 = tmp->childByName("outer")))
				throw CVTException( "No outher mouth node" );
			deserializeXMLFacePart( tmp2, 15, 12 );

			/* inner */
			if( !( tmp2 = tmp->childByName("inner")))
				throw CVTException( "No inner mouth node" );
			deserializeXMLFacePart( tmp2, 27, 8 );

		/* left eye */
		if( !( tmp = node->childByName("lefteye")))
			throw CVTException( "No lefteye node" );
		deserializeXMLFacePart( tmp, 35, 8 );

		/* right eye */
		if( !( tmp = node->childByName("righteye")))
			throw CVTException( "No righteye node" );
		deserializeXMLFacePart( tmp, 43, 8 );

		/* left eye brow */
		if( !( tmp = node->childByName("lefteyebrow")))
			throw CVTException( "No lefteyebrow node" );
		deserializeXMLFacePart( tmp, 51, 10 );

		/* right eye brow */
		if( !( tmp = node->childByName("righteyebrow")))
			throw CVTException( "No lefteyebrow node" );
		deserializeXMLFacePart( tmp, 61, 10 );

		/* nose */
		if( !( tmp = node->childByName("nose")))
			throw CVTException( "No nose node" );
		deserializeXMLFacePart( tmp, 71, 14 );
	}

	inline XMLNode* FaceShape::serialize() const
	{
		XMLNode* ret = new XMLElement("face");
		XMLNode* c;
		XMLNode* tmp;

		/* outline */
		c = new XMLElement("outline");
		serializeXMLFacePart( c, 0, 15 );
		ret->addChild( c );
		/* mouth */
			c = new XMLElement("mouth");
			/* outer */
			tmp = new XMLElement("outer");
			serializeXMLFacePart( tmp, 15, 12 );
			c->addChild( tmp );
			/* inner */
			tmp = new XMLElement("inner");
			serializeXMLFacePart( tmp, 27, 8 );
			c->addChild( tmp );
		ret->addChild( c );

		/* left eye */
		c = new XMLElement("lefteye");
		serializeXMLFacePart( c, 35, 8 );
		ret->addChild( c );

		/* right eye */
		c = new XMLElement("righteye");
		serializeXMLFacePart( c, 43, 8 );
		ret->addChild( c );

		/* left eye brow */
		c = new XMLElement("lefteyebrow");
		serializeXMLFacePart( c, 51, 10 );
		ret->addChild( c );

		/* right eye brow */
		c = new XMLElement("righteyebrow");
		serializeXMLFacePart( c, 61, 10 );
		ret->addChild( c );

		/* nose */
		c = new XMLElement("nose");
		serializeXMLFacePart( c, 71, 14 );
		ret->addChild( c );

		return ret;
	}

	inline void FaceShape::deserializeXMLFacePart( XMLNode* node, size_t offset, size_t pts )
	{
		size_t count = 0;
		size_t n;
		XMLNode* tmp;

		for( int i = 0, end = node->childSize(); i < end; i++ ) {
			Point2f pt;
			XMLNode* c = node->child( i );
			if( c->name() != "point" )
				continue;
			count++;
			if( !( tmp = c->childByName("x") ) )
				throw CVTException("Missing x coordinate");
			pt.x = tmp->value().toFloat();
			if( !( tmp = c->childByName("y") ) )
				throw CVTException("Missing x coordinate");
			pt.y = tmp->value().toFloat();
			if( !( tmp = c->childByName("n") ) )
				throw CVTException("Missing index");
			n = tmp->value().toInteger();
			if( n < 1 || n > pts )
				throw CVTException("Invalid index");
			( *this )[ offset + n - 1 ] = pt;
		}
		if( count != pts )
			throw CVTException("Invalid number of points");
	}

	inline void FaceShape::serializeXMLFacePart( XMLNode* node, size_t offset, size_t pts ) const
	{
		String str;

		for( size_t i = 0; i < pts; i++ ) {
			const Point2f pt = ( *this )[ offset + i ];

			XMLElement* ptnode = new XMLElement( "point" );
			str.sprintf( "%f", pt.x );
			ptnode->addChild( new XMLAttribute( "x" , str ) );
			str.sprintf( "%f", pt.y );
			ptnode->addChild( new XMLAttribute( "y" , str ) );
			str.sprintf( "%z", i + 1 );
			ptnode->addChild( new XMLAttribute( "n" , str ) );
			node->addChild( ptnode );
		}
	}

}

#endif
