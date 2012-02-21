/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_GRAPHEDGE_H
#define CVT_GRAPHEDGE_H

namespace cvt {
	template<typename,typename> class GraphNode;
	template<typename,typename> class Graph;

	/* Edge */
	template<typename TNODE, typename TEDGE>
		class GraphEdge {
			template<typename,typename> friend class Graph;
			template<typename,typename> friend class GraphNode;
			public:
				typedef GraphNode<TNODE,TEDGE> NODETYPE;

				GraphEdge(  NODETYPE* src, NODETYPE* dst, const TEDGE& data );
				~GraphEdge();

				TEDGE& data();
				const TEDGE& data() const;
				NODETYPE* source() const;
				NODETYPE* dest() const;

			public:
				NODETYPE* _src;
				NODETYPE* _dst;
				TEDGE	  _data;
		};


	template<typename TNODE,typename TEDGE>
		inline GraphEdge<TNODE,TEDGE>::GraphEdge( NODETYPE* src, NODETYPE* dst, const TEDGE& data ) : _src( src ), _dst( dst ), _data( data )
	{
	}

	template<typename TNODE,typename TEDGE>
		inline GraphEdge<TNODE,TEDGE>::~GraphEdge()
		{
		}

	template<typename TNODE,typename TEDGE>
		inline TEDGE& GraphEdge<TNODE,TEDGE>::data()
		{
			return _data;
		}

	template<typename TNODE,typename TEDGE>
		inline const TEDGE& GraphEdge<TNODE,TEDGE>::data() const
		{
			return _data;
		}

	template<typename TNODE,typename TEDGE>
		inline GraphNode<TNODE,TEDGE>* GraphEdge<TNODE,TEDGE>::source() const
		{
			return _src;
		}

	template<typename TNODE,typename TEDGE>
		inline GraphNode<TNODE,TEDGE>* GraphEdge<TNODE,TEDGE>::dest() const
		{
			return _dst;
		}

}

#endif
