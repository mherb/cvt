#include <cvt/vision/features/ORB.h>

namespace cvt {

	const int ORB::_circularoffset[ 31 ] = {
		3,  6,  8,  9, 10, 11, 12, 13, 13, 14, 14, 14, 15, 15, 15, 15,
		15, 15, 15, 14, 14, 14, 13, 13, 12, 11, 10,  9,  8,  6,  3
	};

	#include "ORBPattern.h"
}
