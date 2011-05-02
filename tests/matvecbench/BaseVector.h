#ifndef BASEVECTOR_H
#define BASEVECTOR_H

#include <stdint.h>
#include <stdlib.h>

template<class DERIVED>
class BaseVector {
	public:
		inline const DERIVED& derived( void ) const {
			return static_cast<const DERIVED&>( *this );
		}
};

#endif
