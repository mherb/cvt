/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef ISCALEFILTER_H
#define ISCALEFILTER_H

#include <string>
#include <cvt/math/Fixed.h>

namespace cvt {

	struct IConvolveAdaptiveSize {
		size_t numw;
		ssize_t incr;
	};

	struct IConvolveAdaptivef {
		IConvolveAdaptiveSize* size;
		float* weights;
	};

	struct IConvolveAdaptiveFixed {
		IConvolveAdaptiveSize* size;
		Fixed* weights;
	};

	class IScaleFilter {
		public:
			IScaleFilter( float support = 1.0f, float sharpsmooth = 0.0f ) : _support( support ), _sharpsmooth( sharpsmooth ) {}
			virtual ~IScaleFilter() {}
			virtual float eval( float x ) const = 0;
			virtual const std::string name() const = 0;
			size_t getAdaptiveConvolutionWeights( size_t dst, size_t src, IConvolveAdaptivef& conva, bool nonegincr = true ) const;
			size_t getAdaptiveConvolutionWeights( size_t dst, size_t src, IConvolveAdaptiveFixed& conva, bool nonegincr = true ) const;

		protected:
			float _support;
			float _sharpsmooth;
	};

	class IScaleFilterBilinear : public IScaleFilter
	{
		public:
			IScaleFilterBilinear( float sharpsmooth = 0.0f ) : IScaleFilter( 1.0f, sharpsmooth ) {}
			virtual float eval( float x ) const;
			virtual const std::string name() const { return "Bilinear"; }
	};

	class IScaleFilterCubic : public IScaleFilter
	{
		public:
			IScaleFilterCubic( float sharpsmooth = 0.0f ) : IScaleFilter( 2.0f, sharpsmooth ) {}
			virtual float eval( float x ) const;
			virtual const std::string name() const { return "Cubic"; }
	};

	class IScaleFilterCatmullRom : public IScaleFilter
	{
		public:
			IScaleFilterCatmullRom( float sharpsmooth = 0.0f ) : IScaleFilter( 2.0f, sharpsmooth ) {}
			virtual float eval( float x ) const;
			virtual const std::string name() const { return "CatmullRom"; }
	};

	class IScaleFilterMitchell : public IScaleFilter
	{
		public:
			IScaleFilterMitchell( float sharpsmooth = 0.0f ) : IScaleFilter( 2.0f, sharpsmooth ) {}
			virtual float eval( float x ) const;
			virtual const std::string name() const { return "Mitchell"; }
	};

	class IScaleFilterLanczos : public IScaleFilter
	{
		public:
			IScaleFilterLanczos( float support = 3.0f, float sharpsmooth = 0.0f ) : IScaleFilter( support, sharpsmooth ) {}
			virtual float eval( float x ) const;
			virtual const std::string name() const { return "Lanczos"; }
	};

	class IScaleFilterBlackman : public IScaleFilter
	{
		public:
			IScaleFilterBlackman( float support = 3.0f, float sharpsmooth = 0.0f ) : IScaleFilter( support, sharpsmooth ) {}
			virtual float eval( float x ) const;
			virtual const std::string name() const { return "Blackman"; }
	};

	class IScaleFilterBlackmanHarris : public IScaleFilter
	{
		public:
			IScaleFilterBlackmanHarris( float support = 3.0f, float sharpsmooth = 0.0f ) : IScaleFilter( support, sharpsmooth ) {}
			virtual float eval( float x ) const;
			virtual const std::string name() const { return "BlackmanHarris"; }
	};

	class IScaleFilterGauss : public IScaleFilter
	{
		public:
			IScaleFilterGauss( float support = 2.0f, float sharpsmooth = 0.0f ) : IScaleFilter( support, sharpsmooth ) {}
			virtual float eval( float x ) const ;
			virtual const std::string name() const { return "Gauss"; }
	};

};


#endif
