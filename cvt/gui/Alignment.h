#ifndef CVT_ALIGNMENT_H
#define CVT_ALIGNMENT_H

namespace cvt {
	enum AlignmentFlags {
		ALIGN_LEFT    = ( 1 << 0 ),
		ALIGN_CENTER  = ( 1 << 1 ),
		ALIGN_RIGHT   = ( 1 << 2 ),
		ALIGN_TOP     = ( 1 << 3 ),
		ALIGN_VCENTER = ( 1 << 4 ),
		ALIGN_BOTTOM  = ( 1 << 5 )
	};

	CVT_ENUM_TO_FLAGS( AlignmentFlags, Alignment )
}

#endif
