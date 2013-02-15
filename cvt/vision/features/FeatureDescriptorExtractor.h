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
