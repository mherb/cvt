/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_MORPHERODE_H
#define CVT_MORPHERODE_H

#include <cvt/gfx/IFilter.h>
#include <cvt/cl/CLKernel.h>

namespace cvt {
	class MorphErode : public IFilter {
		public:
			MorphErode();
			void apply( Image& dst, const Image& src, size_t radius, IFilterType = IFILTER_CPU ) const;
			void apply( const ParamSet* set, IFilterType t = IFILTER_CPU ) const;

		private:
			void erodeU8( Image& dst, const Image& src, size_t radius ) const;
			void erodeU16( Image& dst, const Image& src, size_t radius ) const;
			void erodeF( Image& dst, const Image& src, size_t radius ) const;
	};
}

#endif
