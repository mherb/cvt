/*
			CVT - Computer Vision Tools Library

	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURE_DESCRIPTOR_EXTRACTOR_H
#define CVT_FEATURE_DESCRIPTOR_EXTRACTOR_H

namespace cvt {
	class FeatureDescriptorExtractor
	{
		public:
			virtual ~FeatureDescriptorExtractor() {}

			size_t					 size() const;
			FeatureDescriptor&		 operator[]( size_t i );
			const FeatureDescriptor& operator[]( size_t i ) const;

			virtual void extract( const Image& img, const FeatureSet& features ) = 0;
			virtual void extract( const ImagePyramid& pyr, const FeatureSet& features ) = 0;
	};
}

#endif
