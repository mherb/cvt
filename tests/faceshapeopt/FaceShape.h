#ifndef CVT_FACESHAPE_H
#define CVT_FACESHAPE_H

#include <Eigen/Core>
#include <cvt/gfx/GFX.h>
#include <cvt/math/Matrix.h>

namespace cvt {
	class FaceShape
	{
		public:
			FaceShape();
			~FaceShape();
			void load( const char* path );
			void drawMean( GFX* g, const Matrix3f& transform );
			void drawCurrent( GFX* g );
			void draw( GFX* g, const Matrix3f& transform, const Eigen::VectorXf& p );
			Eigen::VectorXf& weights();
			Eigen::VectorXf& points();

		private:
			void updateCurrent();

			size_t _ptsize;
			size_t _pcsize;
			size_t _lsize;
			Matrix3f _transform;
			Eigen::VectorXf _mean;
			Eigen::MatrixXf _pc;
			Eigen::VectorXf _p;
			Eigen::VectorXf _pts;
			uint32_t* _lines;
	};


	inline Eigen::VectorXf& FaceShape::weights()
	{
		return _p;
	}

	inline Eigen::VectorXf& FaceShape::points()
	{
		return _pts;
	}

}

#endif
