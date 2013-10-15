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
			TerminationCriteria( TerminationType termType = ( TERM_COSTS_THRESH | TERM_MAX_ITER ) ) :
				_costThreshold( std::numeric_limits<T>::min() ),
				_maxIterations( 40 ),
				_termType( termType )
			{}

			TerminationCriteria( const TerminationCriteria & other ) :
				_costThreshold( other._costThreshold ), _maxIterations( other._maxIterations )
			{}

			void setCostThreshold( T c ){ _costThreshold = c; }
			void setMaxIterations( size_t c ){ _maxIterations = c; }

			bool finished( T costs, size_t iters ) const
			{
				bool ret = false;
				if( _termType & TERM_MAX_ITER ){
					ret |= checkIters( iters );
				}
				if( _termType & TERM_COSTS_THRESH ){
					ret |= checkCosts( costs );
				}
				return ret;
			}
		
			bool checkCosts( T c ) const
			{
				return c < _costThreshold;
			}
		
			bool checkIters( size_t i ) const
			{
				return i > _maxIterations;
			}

			private:
				T					_costThreshold;
				size_t				_maxIterations;
				TerminationType		_termType;
	};
}

#endif
