#include <cvt/gfx/IKernel.h>
#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>

namespace cvt {
	static float _id[] = { 1.0f };

	static float _gauss3_data[] = { 0.25f, 0.5f, 0.25f };
	static float _gauss5_data[] = { 1.0f / 17.0f, 4.0f / 17.0f, 7.0f / 17.0f, 4.0f / 17.0f, 1.0f / 17.0f };
	static float _gauss7_data[] ={ 1.0f / 226.0f, 12.0f / 226.0f, 55.0f / 226.0f, 90.0f / 226.0f, 55.0f / 226.0f, 12.0f / 226.0f, 1.0f / 226.0f };

	static float _mean3_data[] = { 1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f };
	static float _mean5_data[] = { 1.0f / 5.0f, 1.0f / 5.0f, 1.0f / 5.0f, 1.0f / 5.0f, 1.0f / 5.0f };
	static float _mean7_data[] ={ 1.0f / 7.0f, 1.0f / 7.0f, 1.0f / 7.0f, 1.0f / 7.0f, 1.0f / 7.0f, 1.0f / 7.0f, 1.0f / 7.0f };

	static float _haar2_data[] = { 1.0f, -1.0f };
	static float _haar3_data[] = { 1.0f, 0.0f, -1.0f };
	static float _fivept_data[] = { -1.0f / 12.0f, 8.0f / 12.0f, 0.0f, -8.0f / 12.0f, 1.0f / 12.0f };

	static float _laplace33_data[] = { -1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f };
	static float _laplace3_data[] = { -1.0f,  2.0f, -1.0f };

	static float _laplace5_data[] = { -1.0f, 0.0f,  2.0f, 0.0f, -1.0f };

	const IKernel IKernel::IDENTITY = IKernel( 1, 1, _id );

	const IKernel IKernel::GAUSS_HORIZONTAL_3 = IKernel( 3, 1, _gauss3_data );
	const IKernel IKernel::GAUSS_HORIZONTAL_5 = IKernel( 5, 1, _gauss5_data );
	const IKernel IKernel::GAUSS_HORIZONTAL_7 = IKernel( 7, 1, _gauss7_data );

	const IKernel IKernel::MEAN_HORIZONTAL_3 = IKernel( 3, 1, _mean3_data );
	const IKernel IKernel::MEAN_HORIZONTAL_5 = IKernel( 5, 1, _mean5_data );
	const IKernel IKernel::MEAN_HORIZONTAL_7 = IKernel( 7, 1, _mean7_data );

	const IKernel IKernel::HAAR_HORIZONTAL_2 = IKernel( 2, 1, _haar2_data );
	const IKernel IKernel::HAAR_HORIZONTAL_3 = IKernel( 3, 1, _haar3_data );
	const IKernel IKernel::FIVEPOINT_DERIVATIVE_HORIZONTAL = IKernel( 5, 1, _fivept_data );

	const IKernel IKernel::GAUSS_VERTICAL_3 = IKernel( 1, 3, _gauss3_data );
	const IKernel IKernel::GAUSS_VERTICAL_5 = IKernel( 1, 5, _gauss5_data );
	const IKernel IKernel::GAUSS_VERTICAL_7 = IKernel( 1, 7, _gauss7_data );

	const IKernel IKernel::MEAN_VERTICAL_3 = IKernel( 1, 3, _mean3_data );
	const IKernel IKernel::MEAN_VERTICAL_5 = IKernel( 1, 5, _mean5_data );
	const IKernel IKernel::MEAN_VERTICAL_7 = IKernel( 1, 7, _mean7_data );

	const IKernel IKernel::HAAR_VERTICAL_2 = IKernel( 1, 2, _haar2_data );
	const IKernel IKernel::HAAR_VERTICAL_3 = IKernel( 1, 3, _haar3_data );
	const IKernel IKernel::FIVEPOINT_DERIVATIVE_VERTICAL = IKernel( 1, 5, _fivept_data );

	const IKernel IKernel::LAPLACE_33 = IKernel( 3, 3, _laplace33_data );
	const IKernel IKernel::LAPLACE_3_XX = IKernel( 3, 1, _laplace3_data );
	const IKernel IKernel::LAPLACE_3_YY = IKernel( 1, 3, _laplace3_data );

	const IKernel IKernel::LAPLACE_5_XX = IKernel( 5, 1, _laplace5_data );
	const IKernel IKernel::LAPLACE_5_YY = IKernel( 1, 1, _laplace5_data );

	void IKernel::toImage( Image& dst ) const
	{
		dst.reallocate( width(), height(), IFormat::GRAY_FLOAT );

		IMapScoped<float> map( dst );
		size_t h = dst.height();
		size_t w = dst.width();
		for( size_t y = 0; y < h; y++ ) {
			float* ptr = map.ptr();
			for( size_t x = 0; x < w; x++ )
				ptr[ x ] = operator()( x, y );
			map++;
		}
	}

}
