#include <cvt/gfx/Image.h>
#include <cvt/geom/Polygon.h>
#include <cvt/geom/Rect.h>
#include <cvt/math/Fixed.h>
#include <cvt/container/List.h>

namespace cvt
{
	class IPolygonRaster
	{
		public:
			IPolygonRaster( const Polygonf& poly, const Rectf& cliprect );
			void rasterize( Image& dst );

		private:
			void polygonToEdges( const Polygonf& poly );

			struct PolyEdge {

				PolyEdge( const Vector2f& p1, const Vector2f& p2 )
				{
					if( p1.y > p2.y ) {
						pt1.x = p1.x;
						pt1.y = p1.y;
						pt2.x = p2.x;
						pt2.y = p2.y;
						dir = 1;
					} else {
						pt1.x = p1.x;
						pt1.y = p1.y;
						pt2.x = p2.x;
						pt2.y = p2.y;
						dir = 127;
					}

					Fixed dx = pt2.x - pt1.x;
					Fixed dy = pt2.y - pt1.y;

					if( dy.native() )
						slope = dx / dy;
					else
						slope = dy;
				}

				bool operator<( const PolyEdge& edge ) const
				{
					return pt1.y.native() < edge.pt1.y.native();
				}

				struct {
					Fixed x, y;
				} pt1;
				struct {
					Fixed x, y;
				} pt2;
				Fixed  slope;
				Fixed  subslope;
				Fixed  cx;
				int8_t dir;
			};

			Rectf			_cliprect;
			List<PolyEdge>  _edges;

			static const Fixed _offsets8[ 8 ];
	};


	inline IPolygonRaster::IPolygonRaster( const Polygonf& poly, const Rectf& cliprect ) : _cliprect( cliprect )
	{
		polygonToEdges( poly );
	}
}
