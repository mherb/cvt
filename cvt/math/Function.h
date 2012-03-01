#ifndef CVT_FUNCTION_H
#define CVT_FUNCTION_H

namespace cvt
{
	template <class ResultType, class ParamType>
	class Function
	{
		public:
			virtual ResultType operator()( const ParamType& ) const { throw CVTException( "FUNCTION OPERATOR NOT IMPLEMENTED" ); }

			virtual ~Function(){}
	};
}

#endif
