#include <cvt/util/Time.h>

namespace cvt 
{
#ifdef APPLE
	mach_timebase_info_data_t Time::_machTimebase = {0, 0};
#endif
}