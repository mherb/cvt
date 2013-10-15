#ifndef CVT_FEATURE_DESCRIPTOR_EXTRACTOR_H
#define CVT_FEATURE_DESCRIPTOR_EXTRACTOR_H

#include <cvt/vision/features/FeatureMatch.h>
#include <cvt/vision/features/FeatureDescriptor.h>
#include <cvt/gfx/Image.h>
#include <cvt/vision/ImagePyramid.h>

namespace cvt {
	class RowLookupTable;
	class FeatureDescriptorExtractor
	{
		public:
			virtual ~FeatureDescriptorExtractor() {}

			virtual size_t						size() const = 0;
			virtual FeatureDescriptorExtractor* clone() const = 0;
			virtual FeatureDescriptor&			operator[]( size_t i ) = 0;
			virtual const FeatureDescriptor&	operator[]( size_t i ) const = 0;

			virtual void clear() = 0;
			virtual void extract( const Image& img, const FeatureSet& features ) = 0;
			virtual void extract( const ImagePyramid& pyr, const FeatureSet& features ) = 0;

			virtual void matchBruteForce( std::vector<FeatureMatch>& matches,
										  const FeatureDescriptorExtractor& other,
										  float maxDistance ) const = 0;

			virtual void matchInWindow( std::vector<MatchingIndices>& matches,
										const std::vector<FeatureDescriptor*>& other,
										float maxFeatureDist,
										float maxDescDistance ) const = 0;

			virtual void matchInWindow( std::vector<MatchingIndices>& matches,
										const RowLookupTable& rlt,
										const std::vector<FeatureDescriptor*>& other,
										float maxFeatureDist,
										float maxDescDistance ) const = 0;

			virtual void scanLineMatch( std::vector<FeatureMatch>& matches,
										const std::vector<const FeatureDescriptor*>& left,
										float minDisp,
										float maxDisp,
										float maxDescDist,
										float maxLineDist = 1 ) const = 0;

			class CmpY
			{
				public:
					bool operator()( const FeatureDescriptor& f1, const FeatureDescriptor& f2 )
					{
						return f1.pt.y < f2.pt.y;
					}
			};

			class CmpYi
			{
				public:
					bool operator()( const FeatureDescriptor& f1, const FeatureDescriptor& f2 )
					{
						return ( int )f1.pt.y < ( int )f2.pt.y;
					}
			};
	};
}

#endif
