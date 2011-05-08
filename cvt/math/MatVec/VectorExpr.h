#ifndef CVT_VECTOREXPR_H
#define CVT_VECTOREXPR_H

namespace cvt {
	template<typename DERIVED>
	class VectorExpr
	{
		public:
		inline const DERIVED& derived() const
		{
			return static_cast<const DERIVED&>( *this );
		}
	};
}

#endif
