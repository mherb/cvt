#include <cvt/gui/TimerInfo.h>

namespace cvt {
	IDGenerator TimerInfo::_idgen;

	TimerInfo* TimerInfo::create( size_t msinterval, EventTimer* t )
	{
		return new TimerInfo( msinterval, t );
	}

}

