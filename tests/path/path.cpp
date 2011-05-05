#include <cvt/gfx/Path.h>
#include <cvt/geom/Polygon.h>

using namespace cvt;

void dumpPathNode( const Pathf::PathNode& node )
{
	if( node.type == Pathf::PATHNODE_MOVE )
		std::cout << "MOVE: " << node.pt[ 0 ] << std::endl;
	else if( node.type == Pathf::PATHNODE_LINE )
		std::cout << "LINE: " << node.pt[ 0 ] << std::endl;
	else if( node.type == Pathf::PATHNODE_CLOSE )
		std::cout << "CLOSE: " << std::endl;
	else if( node.type == Pathf::PATHNODE_CURVE )
		std::cout << "CRUVE: " << node.pt[ 0 ]  << " " << node.pt[ 1 ] << " " << node.pt[ 2 ] << std::endl;
}

int main()
{
	Pathf p;
	p.moveTo( 0.0f, 0.0f );
	p.lineTo( 1.0f, 0.0f );
	p.lineTo( 1.0f, 1.0f );
	p.lineTo( 0.0f, 1.0f );
	p.close();

	for( int i = 0, end = p.size(); i < end; ++i ) {
		dumpPathNode( p[ i ] );
	}

	PolygonSet<float> polyset( p );
	std::cout << polyset.size() << std::endl;
	const Polygon<float>& poly = polyset[ 0 ];
	for( int i = 0, end = poly.size(); i < end; ++i ) {
		std::cout << poly[ i ] << std::endl;
	}

	std::cout << poly.bbox() << std::endl;
}
