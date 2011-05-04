#ifndef CVT_PATH_H
#define CVT_PATH_H

#include <cvt/math/Vector.h>
#include <vector>

namespace cvt {
	template<typename T>
	class Path
	{
		public:
			enum PathNodeType { PATHNODE_MOVE, PATHNODE_LINE, PATHNODE_CURVE, PATHNODE_CLOSE };
			struct PathNode {
				PathNode( PathNodeType t ) : type( t ) {};
				PathNode( PathNodeType t, const Vector2<T>& pt1 ) : type( t ) { pt[ 0 ] = pt1; };
				PathNode( PathNodeType t, const Vector2<T>& pt1, const Vector2<T>& pt2, const Vector2<T>& pt3 ) : type( t )
				{ pt[ 0 ] = pt1; pt[ 1 ] = pt2; pt[ 2 ] = pt3; };

				PathNodeType type;
				Vector2<T> pt[ 3 ];
			};


			Path();
			Path( const Path<T>& p );

			void moveTo( T x, T y );
			void moveTo( const Vector2<T>& pt );
			void lineTo( T x, T y );
			void lineTo( const Vector2<T>& pt );
			void curveTo( T dx1, T dy1, T dx2, T dy2, T x, T y );
			void curveTo( const Vector2<T>& d1, const Vector2<T>& d2, const Vector2<T>& pt );
			void close();

			size_t size() const;
			const PathNode& operator[]( int i ) const { return _nodes[ i ]; };
		private:
			std::vector< PathNode > _nodes;
	};

	template<typename T>
	inline Path<T>::Path()
	{
	}

	template<typename T>
	inline Path<T>::Path( const Path& p ) : _nodes( p._nodes )
	{
	}

	template<typename T>
	inline void Path<T>::moveTo( T x, T y )
	{
		_nodes.push_back( PathNode( PATHNODE_MOVE, Vector2<T>( x, y ) ) );
	}

	template<typename T>
	inline void Path<T>::moveTo( const Vector2<T>& pt )
	{
		_nodes.push_back( PathNode( PATHNODE_MOVE, pt ) );
	}

	template<typename T>
	inline void Path<T>::lineTo( T x, T y )
	{
		_nodes.push_back( PathNode( PATHNODE_MOVE, Vector2<T>( x, y ) ) );
	}

	template<typename T>
	inline void Path<T>::lineTo( const Vector2<T>& pt )
	{
		_nodes.push_back( PathNode( PATHNODE_MOVE, pt ) );
	}

	template<typename T>
	inline void Path<T>::curveTo( T dx1, T dy1, T dx2, T dy2, T x, T y )
	{
		_nodes.push_back( PathNode( PATHNODE_CURVE, Vector2<T>( dx1, dy1 ), Vector2<T>( dx2, dy2), Vector2<T>( x, y ) ) );
	}

	template<typename T>
	inline void Path<T>::curveTo( const Vector2<T>& d1, const Vector2<T>& d2, const Vector2<T>& pt )
	{
		_nodes.push_back( PathNode( PATHNODE_CURVE, d1, d2, pt ) );
	}

	template<typename T>
	inline void Path<T>::close()
	{
		_nodes.push_back( PathNode( PATHNODE_CLOSE ) );
	}

	template<typename T>
	inline size_t Path<T>::size() const
	{
		return _nodes.size();
	}

	typedef Path<float> Pathf;
	typedef Path<double> Pathd;
}

#endif
