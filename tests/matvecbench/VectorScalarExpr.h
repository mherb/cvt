#ifndef VECTOR_SCALAR_EXPR_H
#define VECTOR_SCALAR_EXPR_H

#include "BaseVector.h"

template <class V>
class VectorScalarExpr: public BaseVector< VectorScalarExpr< V > > {
	public:
		typedef const VectorScalarExpr& StoreType;
		typedef typename V::ElementType ElementType;

		VectorScalarExpr( const BaseVector<V> & v,
						    const ElementType & a ): _a(a), _v( v.derived() )
		{
		}

		inline ElementType operator[]( size_t i) const {
			ElementType result= _a * _v[i];
			return result;
		}

		inline size_t size( void ) const { return _v.size(); }

	private:
		ElementType	_a;
		typename V::StoreType _v;
};

template <class L>
VectorScalarExpr<L> operator*( const BaseVector<L>& v, const typename L::ElementType & a){
	return VectorScalarExpr<L>(v,a);
}

template <class R>
VectorScalarExpr<R> operator*(const typename R::ElementType & a, const BaseVector<R> & v){
	return VectorScalarExpr<R>(v,a);
}
#endif
