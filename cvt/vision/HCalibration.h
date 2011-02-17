#ifndef CVT_HCALIBRATION_H
#define CVT_HCALIBRATION_H

#include <cvt/math/Math.h>
#include <Eigen/Core>
#include <Eigen/SVD>

namespace cvt {
	class HCalibration {
		public:
			HCalibration();

			void reset();
			void addHomography( const Eigen::Matrix<double,3,3>& matrix );
			bool calibration( Eigen::Matrix<double,3,3>& calib  );
			size_t size() const;

		private:
			HCalibration( const HCalibration& );
			HCalibration& operator=( const HCalibration& );

			size_t _n;
			Eigen::Matrix<double,6,6> _V;
	};

	inline HCalibration::HCalibration() : _n( 0 )
	{
		_V.setZero();
	}

	inline void HCalibration::reset()
	{
		_n = 0;
		_V.setZero();
	}

	inline size_t HCalibration::size() const
	{
		return _n;
	}

	inline void HCalibration::addHomography( const Eigen::Matrix<double,3,3>& h )
	{
		Eigen::Matrix<double,6,1> v00;
		Eigen::Matrix<double,6,1> v01;
		Eigen::Matrix<double,6,1> v11;

		_n++;

		/*
			v_ij = hi0 hj0;  hi0 hj1 + hi1 hj0;  hi1 hj1;   hi2 hj0 + hi0 hj2;   hi2 hj1 + hi1 hj2;   hi2 hj2
		 */

		v00( 0 ) = h( 0, 0 ) * h( 0, 0 );
		v00( 1 ) = 2.0 * h( 0, 0 ) * h( 1, 0 );
		v00( 2 ) = h( 1, 0 ) * h( 1, 0 );
		v00( 3 ) = 2.0 * h( 0, 0 ) * h( 2, 0 );
		v00( 4 ) = 2.0 * h( 1, 0 ) * h( 2, 0 );
		v00( 5 ) = h( 2, 0 ) * h( 2, 0 );

		v01( 0 ) = h( 0, 0 ) * h( 0, 1 );
		v01( 1 ) = h( 0, 0 ) * h( 1, 1 ) + h( 1, 0 ) * h( 0, 1 );
		v01( 2 ) = h( 1, 0 ) * h( 1, 1 );
		v01( 3 ) = h( 2, 0 ) * h( 0, 1 ) + h( 0, 0 ) * h( 2, 1 );
		v01( 4 ) = h( 2, 0 ) * h( 1, 1 ) + h( 1, 0 ) * h( 2, 1 );
		v01( 5 ) = h( 2, 0 ) * h( 2, 1 );

		v11( 0 ) = h( 0, 1 ) * h( 0, 1 );
		v11( 1 ) = 2.0 * h( 0, 1 ) * h( 1, 1 );
		v11( 2 ) = h( 1, 1 ) * h( 1, 1 );
		v11( 3 ) = 2.0 * h( 0, 1 ) * h( 2, 1 );
		v11( 4 ) = 2.0 * h( 1, 1 ) * h( 2, 1 );
		v11( 5 ) = h( 2, 1 ) * h( 2, 1 );

		_V += ( v01 * v01.transpose() );
		v00 -= v11;
		_V += ( v00 * v00.transpose() );
	}


	inline bool HCalibration::calibration( Eigen::Matrix<double,3,3>& calib  )
	{
		if( _n < 3 )
			return false;

		Eigen::SVD<Eigen::Matrix<double,6,6> > svd( _V );
		Eigen::Matrix<double,6,1> b = svd.matrixV().col( 5 );

		double lambda;
		calib.setZero();
		calib( 1, 2 ) = ( b( 1 ) * b( 3 ) - b( 0 ) * b( 4 ) ) / ( b( 0 ) * b( 2 ) - Math::sqr( b( 1 ) ) );
		lambda = b( 5 ) - ( Math::sqr( b( 3 ) )  + calib( 1, 2 ) * ( b( 1 ) * b( 3 ) - b( 0 ) * b( 4 ) ) ) / b( 0 );
		calib( 0, 0 ) = Math::sqrt( lambda / b( 0 ) );
		calib( 1, 1 ) = Math::sqrt( ( lambda * b( 0 ) ) / ( b( 0 ) * b( 2 ) - Math::sqr( b( 1 ) ) ) );
		calib( 0, 1 ) = ( -b( 1 ) * Math::sqr( calib( 0, 0 ) ) * calib( 1, 1 ) ) / lambda;
		calib( 0, 2 ) = calib( 0, 1 ) * calib( 1, 2 ) / calib( 1, 1 ) - b( 3 ) * Math::sqr( calib( 0, 0 ) ) / lambda;
		calib( 2, 2 ) = 1.0f;
		return true;
	}
}



#endif
