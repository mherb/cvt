#ifndef VECTOR_EXPR_H
#define VECTOR_EXPR_H

#include "BaseVector.h"

template <class LEFT,class OP, class RIGHT>
class VectorExpr: public BaseVector< VectorExpr< LEFT, OP, RIGHT > > {
	public:
		typedef const VectorExpr& StoreType;
		typedef typename RIGHT::ElementType ElementType;

		VectorExpr( const BaseVector<LEFT> & l, const BaseVector<RIGHT>& r ) :
			_left( l.derived() ), _right( r.derived() )
		{
		}

		inline ElementType operator[]( size_t i) const {
			ElementType result=OP::apply( _left[ i ], _right[ i ] );
			return result;
		}

		inline size_t size( void ) const { return _right.size(); }

	private:
		typename LEFT::StoreType _left;
		typename RIGHT::StoreType _right;
};

struct AddOp {
	template <class T>
		static inline T apply(T left, T right){
			T result=left + right;
			return result;
		}
};

struct SubOp {
	template <class T>
		static inline T apply(T left, T right){
			T result = left - right;
			return result;
		}
};

template <class L, class R >
VectorExpr<L, SubOp, R> operator-( const BaseVector<L>& left, const BaseVector<R>& right )
{
	return VectorExpr<L, SubOp, R >(left,right);
}

template <class L, class R >
VectorExpr<L, AddOp, R> operator+( const BaseVector<L>& left, const BaseVector<R>& right )
{
	return VectorExpr<L, AddOp, R >(left,right);
}


#endif
