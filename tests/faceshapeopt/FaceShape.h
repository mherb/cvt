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
			Matrix3f& transform();
			void optimize( Image& i, size_t iter );

		private:
			void updateCurrent();
			static bool sampleNormal( uint8_t* ptr, uint8_t th, int _x, int _y, int x1, int y1, int x2, int y2, int n, size_t w, size_t h, size_t bpp, size_t stride, float& dist );

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

	inline Matrix3f& FaceShape::transform()
	{
		return _transform;
	}
}

#endif
