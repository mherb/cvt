#ifndef CVT_KEYFRAME_H
#define CVT_KEYFRAME_H

#include "MapFeature.h"

namespace cvt
{
	class Keyframe
	{
		public:
			/**
			 *	@param img	the undistorted image
			 */
			Keyframe( const Image& img );

			/**
			 *	add a feature for this keyframe
			 */
			void addFeature( MapFeature* f );

			/**
			 *	get the current pose of this keyframe
			 */
			const Eigen::Matrix4d& pose() const { return _pose.transformation(); }
			void setPose( const Eigen::Matrix4d & transform ) { _pose.set( transform ); }

			/* get unique id of this keyframe */
			size_t id() const { return _id; }

			size_t numFeatures() const { return _features.size(); }
			MapFeature*	feature( size_t idx ){ return _features[ idx ]; }

		private:
			Image						_image;
			size_t						_id;
			std::vector<MapFeature*> 	_features;

			// the pose of the keyframe in world frame (world2Kf)
			SE3<double>					_pose;

			static size_t _nextId;
	};

	size_t Keyframe::_nextId = 0;

	inline Keyframe::Keyframe( const Image& img ) : 
		_image( img ),
		_id( _nextId++ ) /* TODO: thread safety for id! */
	{
	}

	inline void Keyframe::addFeature( MapFeature* f )
	{
		_features.push_back( f );
	}
}

#endif
