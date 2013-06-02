#ifndef CVT_RDFNODE_H
#define CVT_RDFNODE_H

#include <string.h>
#include <cvt/ml/rdf/RDFTest.h>

namespace cvt {

	template<typename DATA, typename NODEDATA>
	class RDFNode
	{
		public:
			RDFNode();
			RDFNode( NODEDATA* data, RDFTest<DATA>*, RDFNode<DATA,NODEDATA>* left, RDFNode<DATA,NODEDATA>* right );
			~RDFNode();

			bool				   test( const DATA& data, size_t index );
			RDFNode<DATA,NODEDATA>* left();
			RDFNode<DATA,NODEDATA>* right();
			RDFTest<DATA>*		   test();
			NODEDATA*			   data();
			const NODEDATA*		   data() const;

		private:
			NODEDATA*			   _data;
			RDFTest<DATA>*		   _test;
			RDFNode<DATA,NODEDATA>* _left;
			RDFNode<DATA,NODEDATA>* _right;
	};


	template<typename DATA, typename NODEDATA>
	inline RDFNode<DATA, NODEDATA>::RDFNode() :
		_data( NULL ),
		_test( NULL ),
		_left( NULL ),
		_right( NULL )
	{
	}

	template<typename DATA, typename NODEDATA>
	inline RDFNode<DATA, NODEDATA>::RDFNode( NODEDATA* data, RDFTest<DATA>* test, RDFNode<DATA,NODEDATA>* left, RDFNode<DATA,NODEDATA>* right  ) :
		_data( data ),
		_test( test ),
		_left( left ),
		_right( right )
	{
	}


	template<typename DATA, typename NODEDATA>
	inline RDFNode<DATA, NODEDATA>::~RDFNode()
	{
	}

	template<typename DATA, typename NODEDATA>
	inline bool RDFNode<DATA, NODEDATA>::test( const DATA& data, size_t index )
	{
		return _test->operator()( data, index );
	}

	template<typename DATA, typename NODEDATA>
	inline RDFNode<DATA,NODEDATA>* RDFNode<DATA, NODEDATA>::left()
	{
		return _left;
	}

	template<typename DATA, typename NODEDATA>
	inline RDFNode<DATA,NODEDATA>* RDFNode<DATA, NODEDATA>::right()
	{
		return _right;
	}

	template<typename DATA, typename NODEDATA>
	inline NODEDATA* RDFNode<DATA, NODEDATA>::data()
	{
		return _data;
	}

	template<typename DATA, typename NODEDATA>
	inline const NODEDATA* RDFNode<DATA, NODEDATA>::data() const
	{
		return _data;
	}
}

#endif
