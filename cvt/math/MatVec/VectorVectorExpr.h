#ifndef CVT_VECTORVECTOREXPR_H
#define CVT_VECTORVECTOREXPR_H

#include <cvt/math/MatVec/VectorExpr.h>
#include <cvt/math/MatVec/CWiseOp.h>
#include <stdlib.h>

namespace cvt {

	/*
	   VEC x VEC -> VEC
	 */

	template<typename VEC1, typename VEC2, typename VECOP>
		class VectorVectorExpr : public VectorExpr< VectorVectorExpr<VEC1, VEC2, VECOP> >
	{
		public:
			typedef typename VEC1::ELEMENT_TYPE ELEMENT_TYPE;

			VectorVectorExpr( const VEC1& vec1, const VEC2& vec2  ) : _vec1( vec1 ), _vec2( vec2 ) {}

			size_t size() const { return _vec1.size(); }

			ELEMENT_TYPE eval( size_t i ) const { return VECOP::eval( _vec1.eval( i), _vec2.eval( i ) ); }

			const VEC1&  _vec1;
			const VEC2&	 _vec2;

	};

	template<typename VEC1, typename VEC2>
		VectorVectorExpr< VEC1, VEC2, CWiseAdd> operator+( const VectorExpr<VEC1>& vec1, const VectorExpr<VEC2>& vec2 )
		{
			const VEC1& v1 = vec1.derived();
			const VEC2& v2 = vec2.derived();
			if( v1.size() != v2.size() )
				throw CVTException( "Can't add vectors with different size!" );
			return VectorVectorExpr<VEC1, VEC2, CWiseAdd>( v1, v2 );
		}

	template<typename VEC1, typename VEC2>
		VectorVectorExpr< VEC1, VEC2, CWiseSub> operator-( const VectorExpr<VEC1>& vec1, const VectorExpr<VEC2>& vec2 )
		{
			const VEC1& v1 = vec1.derived();
			const VEC2& v2 = vec2.derived();
			if( v1.size() != v2.size() )
				throw CVTException( "Can't add vectors with different size!" );
			return VectorVectorExpr<VEC1, VEC2, CWiseSub>( v1, v2 );
		}

}
#endif
