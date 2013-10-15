
#include <cvt/vision/features/Harris.h>

namespace cvt {
	const float Harris::_kappa = 0.04f; // 0.04 to 0.15 - TODO: make parameter
	const int	Harris::_radius = 3;
}
