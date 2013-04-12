#ifndef CVT_FFT_H
#define CVT_FFT_H

#include <cvt/math/Complex.h>

namespace cvt {
	class FFT {
		public:
			template<typename T>
			static void fftRadix2( Complex<T>* data, size_t n, bool backward );

			template<typename T>
			static void fftStridedRadix2( Complex<T>* data, size_t n, size_t stride, bool inverse );


		private:
			FFT();
			FFT( const FFT& );
	};
}

#endif
