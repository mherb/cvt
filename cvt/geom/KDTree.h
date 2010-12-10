#ifndef CVT_KDTREE_H
#define CVT_KDTREE_H

#include <vector>
#include <cvt/util/Rect.h>

namespace cvt
{
	/* 2D */
	class KDNode : public Rectd
	{
		public:
			KDNode( KDNode * parent, size_t index, double xMin, double yMin, double width, double height ) :
				Rectd( xMin, yMin, width, height ),
				_dataIndex( index ),
				_parent( parent ),
				_smaller( 0 ),
				_bigger( 0 )
			{
			}

			~KDNode()
			{
				if( _smaller )
					delete _smaller;
				if( _bigger )
					delete _bigger;
			}

			int	_dataIndex;

			KDNode *	_parent;
			KDNode *	_smaller;
			KDNode *	_bigger;
	};

	/* 2D for begining */
	class KDTree
	{
		public:
			KDTree( const std::vector<Eigen::Vector2d> & data ) : _root( 0 ), _data( data )
		{
			_indices.resize( _data.size() );
			for( size_t i = 0; i < _data.size(); i++ )
				_indices[ i ] = i;

			// recursevily build the tree
			_root = construct( 0, 0, _data.size() - 1, 0, 0, 0, 800, 600 );

		}

			~KDTree()
			{
			}


		private:
			KDNode*									_root;
			std::vector<size_t>						_indices;
			const std::vector<Eigen::Vector2d> &	_data;

			/*	recursive function:
			 *	start:	starting index of data,
			 *	stop:	stopping index of data,
			 *	depth: dimension index used for splitting
			 */
			KDNode * construct( KDNode * parent, size_t start, size_t end, size_t depth, double xMin, double yMin, double width, double height )
			{
				KDNode * node = new KDNode( parent, start, xMin, yMin, width, height );
				if( start != end ){
					// use depth % dimensions as splitting dimension
					// call construct for smaller and bigger values recursively
				}

				return node;
			}
	};
}

#endif
