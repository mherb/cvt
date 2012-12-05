
#ifndef CVT_IBORDER_H
#define CVT_IBORDER_H

#include <cvt/math/Math.h>

namespace cvt {

	enum IBorderType {
		IBORDER_CLAMP		  = ( 1 << 0 ),
		IBORDER_MIRROR		  = ( 1 << 1 ),
		IBORDER_REPEAT		  = ( 1 << 2 ),
		IBORDER_CONSTANT	  = ( 1 << 3 )
	};

	namespace IBorder {

		template<typename T>
		T value( T value, T len, IBorderType type )
		{
			switch( type ) {
				case IBORDER_CLAMP: if( value < 0 )
										return 0;
									if( value >= len )
										return len - 1;
									return value;
				case IBORDER_MIRROR:
									if( value < 0 )
										return - ( ( value + 1 ) % len );
									if( value >= len )
										return len - 1 - value % len;
									return value;
				case IBORDER_REPEAT:
									if( value < 0 )
										return len - 1 + ( ( value + 1 ) % len );
									if( value >= len )
										return value % len;
									return value;
				case IBORDER_CONSTANT:
				default:
									if( value < 0 || value >= len ) return -1;
			}
			return -1;
		}


	}
}

#endif
