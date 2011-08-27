#ifndef CVT_EPNP_H
#define CVT_EPNP_H

#include <cvt/math/PCA.h>
#include <cvt/geom/PointSet.h>
#include <cvt/math/Matrix.h>
#include <cvt/math/Vector.h>

namespace cvt {

	template <typename T>
	class EPnP {
		public:
			EPnP( const PointSet<3, T> & pointSet );

			/**
			 * Compute pose from 2D-3D matches
			 * @param transform	Output transformation (Rotation and Translation)
			 * @param pointSet	The 2D correspondences
			 * @param K			Intrinsic Matrix
			 */
			void solve( Matrix4<T> & transform, const PointSet<2, T> & pointSet, const Matrix3<T> & K ) const;

		private:
			const PointSet<3, T>&	_points3D;
			// the control points
			PointSet<3, T>			_controlPoints;
			Matrix3<T>				_pcInv;

			// the barycentric coords of the 3D pointset pts
			std::vector<Vector4<T> > _barycentricCoords;

			void computeControlPoints( const PointSet<3, T> & ptSet );
			void computeBarycentricCoords( const PointSet<3, T> & ptSet );
			void buildSystem( Eigen::Matrix<T, 12, 12> & A, const PointSet<2, T> & points2D, const Matrix3<T> & K ) const;

			void computeControlPointsDelta( Eigen::Matrix<T, 6, 1> & cpDelta ) const;

			T	 computeBeta1( const Eigen::Matrix<T, 12, 1> & v, const Eigen::Matrix<T, 6, 1> & deltaC ) const;

			void computeBetas2( Vector2<T> & betas, 
							    const Eigen::Matrix<T, 12, 1> & v0,
							    const Eigen::Matrix<T, 12, 1> & v1,
								const Eigen::Matrix<T,  6, 1> & cpDistanceSqr ) const;

			void computeBetas3( Vector3<T> & betas,
							    const Eigen::Matrix<T, 12, 1> & v0,
							    const Eigen::Matrix<T, 12, 1> & v1,
							    const Eigen::Matrix<T, 12, 1> & v2,
								const Eigen::Matrix<T,  6, 1> & cpDistanceSqr ) const;
			
			void computePose( Matrix4<T> & transform, 
							  const Eigen::Matrix<T, 12, 1> & estimatedCoords, 
							  const PointSet<3, T> & controlPoints ) const;

			T	reprojectionError( const Matrix4<T> & transform,
								   const Matrix4<T> & K44,
								   const PointSet<3, T> & p3d,
								   const PointSet<2, T> & p2d ) const;

	};

}

#endif
