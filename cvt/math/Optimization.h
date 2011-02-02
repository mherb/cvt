#ifndef CVT_OPTIMIZATION_H
#define CVT_OPTIMIZATION_H

#include <cvt/util/Flags.h>
#include <limits>

namespace cvt
{
	enum TerminationTypeFlags {
		TERM_MAX_ITER = ( 1 << 0 ),
		TERM_COSTS_THRESH = ( 1 << 1 )
	};

	CVT_ENUM_TO_FLAGS( TerminationTypeFlags, TerminationType )

	template<typename T>
	class TerminationCriteria {
		public:
			TerminationCriteria() :
				costThreshold( std::numeric_limits<T>::min() ),
				maxIterations( 100 )
			{}

			TerminationCriteria( const TerminationCriteria & other ) :
				costThreshold( other.costThreshold ), maxIterations( other.maxIterations )
			{}

			void setCostThreshold( T c ){ costThreshold = c; }
			void setMaxIterations( size_t c ){ maxIterations = c; }

			bool finished( TerminationType & termType, T costs, size_t iters ) const
			{
				bool ret = false;
				if( termType & TERM_MAX_ITER ){
					ret |= ( iters > maxIterations );
				}
				if( termType & TERM_COSTS_THRESH ){
					ret |= ( costs < costThreshold );
				}
				return ret;
			}

			private:
				T		costThreshold;
				size_t	maxIterations;
	};
}

#endif
