#ifndef CVT_KLT_TRACKER_H
#define CVT_KLT_TRACKER_H

#include <cvt/math/Vector.h>
#include <cvt/gfx/Image.h>
#include <vector>

namespace cvt
{
	class KLTTracker
	{
		public:
			KLTTracker( size_t maxIters = 20, size_t patchSize = 16 );

			void trackFeatures( std::vector<size_t> & trackedIndices,
							    std::vector<Vector2f> & newPositions,
							    const Image& current, 
							    const Image& last,																
								const std::vector<Vector2f> & lastPos );
		private:
			size_t _maxIters;
			size_t _patchSize;
			float  _ssdThresh;
			

			bool trackSinglePatch( Vector2f & pos, const Vector2f & lastPos,
								   const uint8_t* current, size_t currStride,
								   const uint8_t* last,    size_t lastStride,
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
