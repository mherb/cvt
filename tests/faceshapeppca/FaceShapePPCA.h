#ifndef CVT_FACESHAPEPPCA_H
#define CVT_FACESHAPEPPCA_H

#include <cvt/math/PPCA.h>
#include <cvt/geom/PointSet.h>

namespace cvt {
	template<typename T>
		class FaceShapePPCA : public PPCA<T> {
			public:
			FaceShapePPCA( size_t dimension, size_t subdimension, size_t mixcomponents ) : PPCA<T>( dimension, subdimension, mixcomponents ) {}

			void preprocessSample( Eigen::Matrix<T, Eigen::Dynamic, 1>& output, const Eigen::Matrix<T, Eigen::Dynamic, 1>& mean, const Eigen::Matrix<T, Eigen::Dynamic, 1>& value );
			void postprocessMean( Eigen::Matrix<T, Eigen::Dynamic, 1>& mean );

		};

	template<typename T>
	inline void FaceShapePPCA<T>::preprocessSample( Eigen::Matrix<T, Eigen::Dynamic, 1>& output, const Eigen::Matrix<T, Eigen::Dynamic, 1>& mean, const Eigen::Matrix<T, Eigen::Dynamic, 1>& value )
	{
		PointSet<2,T> s( value.data(), value.rows() / 2  );
		PointSet<2,T> m( mean.data(), mean.rows() / 2  );
		s.transform( s.alignSimilarity( m ) );
		const Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1> > aligned( ( T* )s.ptr(), mean.rows() );
		output = aligned;
	}

	template<typename T>
	inline void FaceShapePPCA<T>::postprocessMean( Eigen::Matrix<T, Eigen::Dynamic, 1>& mean )
	{
		PointSet<2,T> m( mean.data(), mean.rows() / 2  );
		m.normalize();
		const Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1> > normalized( ( T* )m.ptr(), mean.rows() );
		mean = normalized;
	}

}

#endif
