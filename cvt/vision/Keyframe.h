#ifndef CVT_KEYFRAME_H
#define CVT_KEYFRAME_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/ORB.h>

namespace cvt
{
	class Keyframe
	{
		public:
			Keyframe( const Image & img );

			void				addFeature( const ORBFeature & f, size_t id );

			size_t				numMeasurements()			   const { return _features.size(); }
			const ORBFeature&	featureMeasurement( size_t n ) const { return _features[ n ]; }
			size_t				featureId( size_t n )		   const { return _feature3dIds[ n ]; }

		private:
			Image					_img;

			/* ids of the 3d features belonging to 
			   the measurements in this keyframe */
			std::vector<size_t>		_feature3dIds;

			/* the 2D features */
			std::vector<ORBFeature>	_features;
	};

	inline void Keyframe::addFeature( const ORBFeature & f, size_t id )
	{
		_features.push_back( f );
		_feature3dIds.push_back( id );
	}
}

#endif
