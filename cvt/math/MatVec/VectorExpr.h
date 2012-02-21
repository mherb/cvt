/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
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
