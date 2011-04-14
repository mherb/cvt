#ifndef CVT_FACESHAPE_H
#define CVT_FACESHAPE_H

#include <cvt/geom/PointSet.h>
#include <cvt/gfx/GFX.h>
#include <cvt/io/xml/XMLNode.h>
#include <cvt/util/Exception.h>

namespace cvt {
	class FaceShape : public PointSet2f
	{
		public:
			FaceShape();
			~FaceShape();
			void draw( GFX* g, const Matrix3f& t ) const;
			void deserializeXML( XMLNode* node );
		private:
			void drawLines( GFX* g, const PointSet2f& pts, int start, int size, bool loop = false ) const;
	};

	inline FaceShape::FaceShape()
	{
		resize( 65 );
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
		/* nose */
		drawLines( g, pts , 51, 14 );
	}

	inline void FaceShape::drawLines( GFX* g, const PointSet2f& pts, int start, int size, bool loop ) const
	{
		for( int i = start, end = start + size - 1 ; i < end; i++ ) {
			g->drawLine( pts[ i ], pts[ i + 1 ] );
		}
		if( loop )
			g->drawLine( pts[ start + size - 1 ], pts[ start ] );
	}

	inline void FaceShape::deserializeXML( XMLNode* node )
	{
		XMLNode* tmp;
		XMLNode* tmp2;
		XMLNode* tmp3;
		int count;
		int n;

		if( !node )
			throw CVTException( "No face node" );
		if( node->name() != "face" )
			throw CVTException( "No face node" );

		/* outline */
		if( !( tmp = node->childByName("outline")))
			throw CVTException( "No outline node" );
		count = 0;
		for( int i = 0, end = tmp->childSize(); i < end; i++ ) {
			Point2f pt;
			XMLNode* c = tmp->child( i );
			if( c->name() != "point" )
				continue;
			count++;
			tmp2 = c->childByName("x");
			pt.x = tmp2->value().toFloat();
			tmp2 = c->childByName("y");
			pt.y = tmp2->value().toFloat();
			tmp2 = c->childByName("n");
			n = tmp2->value().toInteger();
			if( n < 1 || n > 15 )
				throw CVTException("Invalid index");
			( *this )[ n - 1 ] = pt;
		}
		if( count != 15 )
			throw CVTException("Invalid number of points ");

		/* mouth */
		if( !( tmp = node->childByName("mouth")))
			throw CVTException( "No mouth node" );

		/* outer */
		if( !( tmp2 = tmp->childByName("outer")))
			throw CVTException( "No outher mouth node" );

		count = 0;
		for( int i = 0, end = tmp2->childSize(); i < end; i++ ) {
			Point2f pt;
			XMLNode* c = tmp2->child( i );
			if( c->name() != "point" )
				continue;
			count++;
			tmp3 = c->childByName("x");
			pt.x = tmp3->value().toFloat();
			tmp3 = c->childByName("y");
			pt.y = tmp3->value().toFloat();
			tmp3 = c->childByName("n");
			n = tmp3->value().toInteger();
			if( n < 1 || n > 12 )
				throw CVTException("Invalid index");
			( *this )[ 15 + n - 1 ] = pt;
		}
		if( count != 12 )
			throw CVTException("Invalid number of points ");

		/* inner */
		if( !( tmp2 = tmp->childByName("inner")))
			throw CVTException( "No inner mouth node" );

		count = 0;
		for( int i = 0, end = tmp2->childSize(); i < end; i++ ) {
			Point2f pt;
			XMLNode* c = tmp2->child( i );
			if( c->name() != "point" )
				continue;
			count++;
			tmp3 = c->childByName("x");
			pt.x = tmp3->value().toFloat();
			tmp3 = c->childByName("y");
			pt.y = tmp3->value().toFloat();
			tmp3 = c->childByName("n");
			n = tmp3->value().toInteger();
			if( n < 1 || n > 12 )
				throw CVTException("Invalid index");
			( *this )[ 27 + n - 1 ] = pt;
		}
		if( count != 8 )
			throw CVTException("Invalid number of points ");

		/* left eye */
		if( !( tmp = node->childByName("lefteye")))
			throw CVTException( "No lefteye node" );
		count = 0;
		for( int i = 0, end = tmp->childSize(); i < end; i++ ) {
			Point2f pt;
			XMLNode* c = tmp->child( i );
			if( c->name() != "point" )
				continue;
			count++;
			tmp2 = c->childByName("x");
			pt.x = tmp2->value().toFloat();
			tmp2 = c->childByName("y");
			pt.y = tmp2->value().toFloat();
			tmp2 = c->childByName("n");
			n = tmp2->value().toInteger();
			if( n < 1 || n > 15 )
				throw CVTException("Invalid index");
			( *this )[ 35 + n - 1 ] = pt;
		}
		if( count != 8 )
			throw CVTException("Invalid number of points ");

		/* right eye */
		if( !( tmp = node->childByName("righteye")))
			throw CVTException( "No righteye node" );
		count = 0;
		for( int i = 0, end = tmp->childSize(); i < end; i++ ) {
			Point2f pt;
			XMLNode* c = tmp->child( i );
			if( c->name() != "point" )
				continue;
			count++;
			tmp2 = c->childByName("x");
			pt.x = tmp2->value().toFloat();
			tmp2 = c->childByName("y");
			pt.y = tmp2->value().toFloat();
			tmp2 = c->childByName("n");
			n = tmp2->value().toInteger();
			if( n < 1 || n > 15 )
				throw CVTException("Invalid index");
			( *this )[ 43 + n - 1 ] = pt;
		}
		if( count != 8 )
			throw CVTException("Invalid number of points ");

		/* nose */
		if( !( tmp = node->childByName("nose")))
			throw CVTException( "No nose node" );
		count = 0;
		for( int i = 0, end = tmp->childSize(); i < end; i++ ) {
			Point2f pt;
			XMLNode* c = tmp->child( i );
			if( c->name() != "point" )
				continue;
			count++;
			tmp2 = c->childByName("x");
			pt.x = tmp2->value().toFloat();
			tmp2 = c->childByName("y");
			pt.y = tmp2->value().toFloat();
			tmp2 = c->childByName("n");
			n = tmp2->value().toInteger();
			if( n < 1 || n > 15 )
				throw CVTException("Invalid index");
			( *this )[ 51 + n - 1 ] = pt;
		}
		if( count != 14 )
			throw CVTException("Invalid number of points ");
	}

}

#endif
