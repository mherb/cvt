#include <cvt/vision/SlamMap.h>

namespace cvt
{
	SlamMap()::SlamMap()
	{
	}

	SlamMap::~SlamMap()
	{
	}

	size_t SlamMap::addKeyframe( const Image & img )
	{
		size_t id = _keyframes.size();
		_keyframes.push_back( Keyframe( img ) );
		return id;
	}

	void SlamMap::addFeatureToKeyframe( const MapFeature & world, 
									    const ORBFeature & feature, 
										size_t keyframeId )
	{
		size_t pointId = _worldFeatures.size();
		_worldFeatures.push_back( world );

		// add the measurement to the keyframe
		_keyframes[ keyframeId ].addFeature( feature, pointId );
	}

}
