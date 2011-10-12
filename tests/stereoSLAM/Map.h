#ifndef CVT_MAP_H
#define CVT_MAP_H

#include "Keyframe.h"

namespace cvt
{
	/* Map for SLAM */
	class Map
	{
		public:
			Map();
			~Map();

			void addKeyframe( Keyframe* kf );
			const Keyframe& keyframeWithId( size_t id ) const;

			/* retrieve the nearby keyframes for a given one */
			void selectNearbyKeyframes( std::vector<Keyframe*> & closeKeyframes,
									    const Keyframe & activeKeyFrame, 
										size_t radius ) const;

		private:
			std::vector<Keyframe*>	_keyframes;

	};

	inline Map::Map()
	{
	}

	inline Map::~Map()
	{
		for( size_t i = 0; i < _keyframes.size(); i++ ){
			delete _keyframes[ i ];
		}
		_keyframes.clear();
	}

	void Map::addKeyframe( Keyframe* kf )
	{
		_keyframes.push_back( kf );
	}

	const Keyframe& Map::keyframeWithId( size_t id ) const
	{
		return _keyframes[ id ];
	}
}

#endif
