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
									    const Eigen::Matrix4d & pose, 
										size_t radius ) const;

			void predictVisibleFeatures( const Matrix4d & predictedPose ) const;

			Keyframe* selectClosestKeyframe( const Eigen::Matrix4d & worldT ) const;

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

	inline void Map::addKeyframe( Keyframe* kf )
	{
		_keyframes.push_back( kf );
	}

	inline const Keyframe& Map::keyframeWithId( size_t id ) const
	{
		return *_keyframes[ id ];
	}


	inline void Map::selectNearbyKeyframes( std::vector<Keyframe*> & closeKeyframes,
										    const Eigen::Matrix4d & pose, 
											size_t radius ) const
	{
		Vector3d poseTrans;
		poseTrans.x = pose( 0, 3 );
		poseTrans.y = pose( 1, 3 );
		poseTrans.z = pose( 2, 3 );

		for( size_t i = 0; i < _keyframes.size(); i++ ){
			Vector3d wKF;
			const Eigen::Matrix4d & curr = _keyframes[ i ]->pose();
			wKF.x = curr( 0, 3 );
			wKF.y = curr( 1, 3 );
			wKF.z = curr( 2, 3 );

			double dist = ( wKF - poseTrans ).length();
			if( dist < radius ){
				closeKeyframes.push_back( _keyframes[ i ] );
			}
		}
	}
			
	inline Keyframe* Map::selectClosestKeyframe( const Eigen::Matrix4d & worldT ) const
	{
		double nearest = 100000.0f;
		Keyframe* kf = 0;

		Vector3d wTrans;
		wTrans.x = worldT( 0, 3 );
		wTrans.y = worldT( 1, 3 );
		wTrans.z = worldT( 2, 3 );

		for( size_t i = 0; i < _keyframes.size(); i++ ){
			Vector3d wKF;
			const Eigen::Matrix4d & curr = _keyframes[ i ]->pose();
			wKF.x = curr( 0, 3 );
			wKF.y = curr( 1, 3 );
			wKF.z = curr( 2, 3 );

			double dist = ( wKF-wTrans ).length();
			if( dist < nearest ){
				nearest = dist;
				kf = _keyframes[ i ];
			}
		}

		return kf;
	}
}

#endif
