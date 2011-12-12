#ifndef CVT_KEYFRAME_H
#define CVT_KEYFRAME_H

#include <Eigen/Core>
#include <Eigen/StdVector>

namespace cvt
{
	class Keyframe
	{
		public:
			EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
			Keyframe( const Eigen::Matrix4d& pose );

			void					addFeature( const Eigen::Vector2d & f, size_t id );

			size_t					numMeasurements()				const { return _features.size(); }
			const Eigen::Vector2d&	featureMeasurement( size_t n )	const { return _features[ n ]; }
			size_t					featureId( size_t n )			const { return _feature3dIds[ n ]; }

			const Eigen::Matrix4d&	pose()							const { return _pose; }	
			void					setPose( const Eigen::Matrix4d & pose ) { _pose = pose; }

			// distance measure of this keyframe to a given transform
			double					distance( const Eigen::Matrix4d & transform ) const;	

		private:
			Eigen::Matrix4d			_pose;

			/* ids of the 3d features belonging to 
			   the measurements in this keyframe */
			std::vector<size_t>				_feature3dIds;

			/* the 2D features */
			std::vector<Eigen::Vector2d>	_features;
	};

	inline void Keyframe::addFeature( const Eigen::Vector2d & f, size_t id )
	{
		_features.push_back( f );
		_feature3dIds.push_back( id );
	}
}

#endif
