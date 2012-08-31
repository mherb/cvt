#include "IPolygonRaster.h"

namespace cvt {
	const Fixed IPolygonRaster::_offsets8[ 8 ] = {
		( 5.0f / 8.0f ),
		( 0.0f / 8.0f ),
		( 3.0f / 8.0f ),
		( 6.0f / 8.0f ),
		( 1.0f / 8.0f ),
		( 4.0f / 8.0f ),
		( 7.0f / 8.0f ),
		( 2.0f / 8.0f )
	};

	void IPolygonRaster::polygonToEdges( const Polygonf& poly )
	{
		size_t n = poly.size() - 1;
		for( size_t i = 0; i < n; i++ )
			_edges.append( PolyEdge( poly[ i ], poly[ i + 1 ] ) );
		_edges.append( PolyEdge( poly[ n ], poly[ 0 ] ) );

		_edges.sort();
	}


	void IPolygonRaster::rasterize( Image& dst )
	{
	
	}

}
