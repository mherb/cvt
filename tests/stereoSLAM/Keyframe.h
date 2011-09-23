#ifndef CVT_KEYFRAME_H
#define CVT_KEYFRAME_H

namespace cvt
{
	struct MapFeature
	{
		Vector4f	point3d;
		Matrix3f	covariance;
		Vector2f	projection;
	};

	class Keyframe
	{
		public:
			/**
			 *	@param img	the undistorted image
			 */
			Keyframe( const Image& img, size_t id );

			/**
			 *	add a feature for this keyframe
			 */
			void addFeature( const MapFeature& f );

			/**
			 *	get the current pose of this keyframe
			 */
			const Matrix4f& pose() const { return _pose; }

			void addNeighboringKeyframe( size_t neighorId );

			const std::vector<size_t>& neighbors() const { return _neighbors; }

		private:
			Image						_image;
			std::vector<MapFeature> &	_features;

			// the pose of the keyframe in world frame (world2Kf)
			Matrix4f					_pose;

			std::vector<size_t>			_neighbors;

	};

	inline Keyframe::Keyframe( const Image& img ) : 
		_image( img )
	{
	}

	inline void Keyframe::addFeature( const MapFeature& f )
	{
		_features.push_back( f );
	}

	inline void Keyframe::addNeighboringKeyframe( size_t nId )
	{
		_neighbors.push_back( nId );
	}
}

#endif
