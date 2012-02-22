/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_VECTORSCALAREXPR_H
#define CVT_VECTORSCALAREXPR_H

#include <cvt/math/MatVec/VectorExpr.h>
#include <cvt/math/MatVec/CWiseOp.h>
#include <stdlib.h>

namespace cvt {

	/*
	   SCALAR x VEC    -> VEC
	   VEC    x SCALAR -> VEC
	*/

	template<typename VEC, typename VECOP>
	class VectorScalarExpr : public VectorExpr< VectorScalarExpr<VEC, VECOP> >
	{
		public:
			typedef typename VEC::ELEMENT_TYPE ELEMENT_TYPE;

			VectorScalarExpr( const ELEMENT_TYPE scalar, const VEC& vec  ) : _scalar( scalar ), _vec( vec ) {}

			size_t size() const { return _vec.size(); }

			ELEMENT_TYPE eval( size_t i ) const { return VECOP::eval( _scalar, _vec.eval( i ) ); }

			const ELEMENT_TYPE  _scalar;
			const VEC&			_vec;

	};

#if 0
	/* constant folding */
	template<typename VEC>
	VectorScalarExpr< VEC, CWiseAdd> operator+( const typename VEC::ELEMENT_TYPE scalar, const VectorScalarExpr<VEC, CWiseAdd>& vec )
	{
		return VectorScalarExpr<VEC, CWiseAdd>( scalar + vec._scalar, vec._vec.derived() );
	}

	/* constant folding */
	template<typename VEC>
	VectorScalarExpr< VEC, CWiseAdd> operator+( const VectorScalarExpr<VEC, CWiseAdd>& vec, const typename VEC::ELEMENT_TYPE scalar  )
	{
		return VectorScalarExpr<VEC, CWiseAdd>( scalar + vec._scalar, vec._vec.derived() );
	}

	/* constant folding */
	template<typename VEC>
	VectorScalarExpr< VEC, CWiseMul> operator*( const typename VEC::ELEMENT_TYPE scalar, const VectorScalarExpr<VEC, CWiseMul>& vec )
	{
		return VectorScalarExpr<VEC, CWiseMul>( scalar * vec._scalar, vec._vec.derived() );
	}

	/* constant folding */
	template<typename VEC>
	VectorScalarExpr< VEC, CWiseMul> operator*( const VectorScalarExpr<VEC, CWiseMul>& vec, const typename VEC::ELEMENT_TYPE scalar  )
	{
		return VectorScalarExpr<VEC, CWiseMul>( scalar * vec._scalar, vec._vec.derived() );
	}
#endif

	template<typename VEC>
	VectorScalarExpr< VEC, CWiseAdd> operator+( const typename VEC::ELEMENT_TYPE scalar, const VectorExpr<VEC>& vec )
	{
		return VectorScalarExpr<VEC, CWiseAdd>( scalar, vec.derived() );
	}

	template<typename VEC>
	VectorScalarExpr< VEC, CWiseAdd> operator+( const VectorExpr<VEC>& vec, const typename VEC::ELEMENT_TYPE scalar )
	{
		return VectorScalarExpr<VEC, CWiseAdd>( scalar, vec.derived() );
	}

	template<typename VEC>
	VectorScalarExpr< VEC, CWiseSub> operator-( const typename VEC::ELEMENT_TYPE scalar, const VectorExpr<VEC>& vec )
	{
		return VectorScalarExpr<VEC, CWiseSub>( scalar, vec.derived() );
	}

	template<typename VEC>
	VectorScalarExpr< VEC, CWiseSubRev> operator-( const VectorExpr<VEC>& vec, const typename VEC::ELEMENT_TYPE scalar )
	{
		return VectorScalarExpr<VEC, CWiseSubRev>( scalar, vec.derived() );
	}

	template<typename VEC>
	VectorScalarExpr< VEC, CWiseMul> operator*( const typename VEC::ELEMENT_TYPE scalar, const VectorExpr<VEC>& vec )
	{
		return VectorScalarExpr<VEC, CWiseMul>( scalar, vec.derived() );
	}

	template<typename VEC>
	VectorScalarExpr< VEC, CWiseMul> operator*( const VectorExpr<VEC>& vec, const typename VEC::ELEMENT_TYPE scalar )
	{
		return VectorScalarExpr<VEC, CWiseMul>( scalar, vec.derived() );
	}

	template<typename VEC>
	VectorScalarExpr< VEC, CWiseDiv> operator/( const typename VEC::ELEMENT_TYPE scalar, const VectorExpr<VEC>& vec )
	{
		return VectorScalarExpr<VEC, CWiseDiv>( scalar, vec.derived() );
	}

	template<typename VEC>
	VectorScalarExpr< VEC, CWiseDivRev> operator/( const VectorExpr<VEC>& vec, const typename VEC::ELEMENT_TYPE scalar )
	{
		return VectorScalarExpr<VEC, CWiseDivRev>( scalar, vec.derived() );
	}
}

#endif
