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

			/* add a Neighbor to this keyframe */
			void addNeighboringKeyframe( size_t neighorId );

			/* get unique id of this keyframe */
			size_t id() const { return _id; }

			const std::vector<size_t>& neighbors() const { return _neighbors; }

		private:
			Image						_image;
			size_t						_id;
			std::vector<MapFeature> &	_features;

			// the pose of the keyframe in world frame (world2Kf)
			// TODO: maybe store as SE3! Later on template on it
			Matrix4f					_pose;
			std::vector<size_t>			_neighbors;

	};

	inline Keyframe::Keyframe( const Image& img, size_t id ) : 
		_image( img ),
		_id( id )
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
