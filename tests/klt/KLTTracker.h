#ifndef CVT_KLT_TRACKER_H
#define CVT_KLT_TRACKER_H

#include <cvt/math/Vector.h>
#include <cvt/gfx/Image.h>
#include <vector>

#include "KLTPatch.h"
#include <cvt/math/Translation2D.h>

namespace cvt
{
	class KLTTracker
	{
		public:
			typedef Translation2D<float> PoseType;
			KLTTracker( size_t maxIters = 20 );

			void trackFeatures( std::vector<size_t> & trackedIndices,
							    std::vector<PoseType> & poses,
							    const std::vector<KLTPatch<16> >& patches,
							    const Image& current );

			void trackMultiscale( std::vector<size_t> & trackedIndices,
								  std::vector<PoseType> & poses,
								  const std::vector<KLTPatch<16> >& patches,
								  const std::vector<Image>& pyramid );

		private:
			size_t _maxIters;
			size_t _patchSize;
			float  _ssdThresh;
			
			bool trackSinglePatch( PoseType& pose, 
								   const Vector2f & tempPos,
								   const KLTPatch<16> & patch,
								   const uint8_t* current, size_t currStride,
								   size_t width, size_t height );

			bool checkBounds( const Vector2f & p, size_t width, size_t height ) const
			{
				size_t h = _patchSize >> 1;
				if( p.x <= h || p.y <= h || p.x + h >= width || p.y + h >= height )
					return false;
				return true;
			}

	};
}

#endif
