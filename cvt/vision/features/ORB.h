/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_ORB_H
#define CVT_ORB_H

#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/vision/IntegralImage.h>
#include <cvt/vision/features/FeatureSet.h>
#include <cvt/vision/features/FeatureDescriptor.h>
#include <cvt/vision/features/FeatureDescriptorExtractor.h>

#include "MatchBruteForce.inl"

namespace cvt {

	class ORB : public FeatureDescriptorExtractor
	{
		public:
			typedef FeatureDescriptorInternal<32, uint8_t, FEATUREDESC_CMP_HAMMING> Descriptor;

			ORB();
			ORB( const ORB& orb );
			~ORB();

			size_t					  size() const;
			ORB*					  clone() const;
			FeatureDescriptor&		  operator[]( size_t i );
			const FeatureDescriptor&  operator[]( size_t i ) const;

			void clear();
			void extract( const Image& img, const FeatureSet& features );
			void extract( const ImagePyramid& pyr, const FeatureSet& features );

			void matchBruteForce( std::vector<FeatureMatch>& matches, const FeatureDescriptorExtractor& other, float distThresh ) const;
			void matchInWindow( std::vector<MatchingIndices>& matches, const std::vector<FeatureDescriptor*>& other, float maxFeatureDist, float maxDescDistance ) const;
			void scanLineMatch( std::vector<FeatureMatch>& matches,
								const std::vector<const FeatureDescriptor*>& left,
								float minDisp,
								float maxDisp,
								float maxDescDist,
								float maxLineDist ) const;

		private:
			struct DistFunc {
				DistFunc() : _simd( SIMD::instance() )
				{
				}

				float operator()( const Descriptor& a, const Descriptor& b ) const
				{
					return _simd->hammingDistance( a.desc, b.desc, 32 );
				}

				SIMD* _simd;
			};

			void centroidAngle( Descriptor& feature, const float* iimgptr, size_t widthstep );
			void descriptor( Descriptor& feature, const float* iimgptr, size_t widthstep );

			static const int		_patterns[ 30 ][ 512 ][ 2 ];
			static const int		_circularoffset[ 31 ];

			std::vector<Descriptor> _features;
	};

	inline ORB::ORB()
	{
	}

	inline ORB::ORB( const ORB& orb ) :
		_features( orb._features )
	{
	}

	inline ORB::~ORB()
	{
	}

	inline size_t ORB::size() const
	{
		return _features.size();
	}

	inline ORB*	ORB::clone() const
	{
		ORB* ocopy = new ORB();
		ocopy->_features = _features;
		return ocopy;
	}

	inline FeatureDescriptor& ORB::operator[]( size_t i )
	{
		return _features[ i ];
	}

	inline const FeatureDescriptor& ORB::operator[]( size_t i ) const
	{
		return _features[ i ];
	}

	inline void ORB::clear()
	{
		_features.clear();
	}

	inline void ORB::extract( const ImagePyramid& img, const FeatureSet& features )
	{
	}

	inline void ORB::extract( const Image& img, const FeatureSet& features )
	{
		if( img.channels() != 1 || ( img.format() != IFormat::GRAY_UINT8 && img.format() != IFormat::GRAY_FLOAT ) )
			throw CVTException( "Unimplemented" );

		IntegralImage iimage( img );
		IMapScoped<const float> map( iimage.sumImage() );
		const float* ptr = map.base();
		const size_t widthstep = map.stride() / sizeof( float );

		size_t iend = features.size();
		for( size_t i = 0; i < iend; ++i ) {
			_features.push_back( Descriptor( features[ i ] ) );
			Descriptor& desc = _features.back();
			centroidAngle( desc, ptr, widthstep );
			descriptor( desc, ptr, widthstep );
		}
	}

	inline void ORB::centroidAngle( Descriptor& feature, const float* iimgptr, size_t widthstep )
	{
		float mx = 0;
		float my = 0;

		int cury = ( int ) feature.pt.y - 15;
		int curx = ( int ) feature.pt.x;

		for( int i = 0; i < 15; i++ ) {
			mx +=( ( float ) i - 15.0f ) * ( IntegralImage::area( iimgptr, curx - _circularoffset[ i ], cury + i, 2 * _circularoffset[ i ] + 1, 1, widthstep )
											- IntegralImage::area( iimgptr, curx - _circularoffset[ i ], cury + 30 - i, 2 * _circularoffset[ i ] + 1, 1, widthstep ) );
		}

		cury = ( int ) feature.pt.y;
		curx = ( int ) feature.pt.x - 15;
		for( int i = 0; i < 15; i++ ) {
			my += ( ( float ) i - 15.0f ) * ( IntegralImage::area( iimgptr, curx + i, cury - _circularoffset[ i ], 1, 2 * _circularoffset[ i ] + 1, widthstep )
											 - IntegralImage::area( iimgptr, curx + 30 - i, cury - _circularoffset[ i ], 1, 2 * _circularoffset[ i ] + 1, widthstep ) );
		}

		feature.angle = Math::atan2( my, mx );


		if( feature.angle < 0 )
			feature.angle += Math::TWO_PI;
		feature.angle = Math::TWO_PI - feature.angle + Math::HALF_PI;

		while( feature.angle > Math::TWO_PI )
			feature.angle -= Math::TWO_PI;
	}

	inline void ORB::descriptor( Descriptor& feature, const float* iimgptr, size_t widthstep )
	{
		size_t index = ( size_t ) ( feature.angle * 30.0f / Math::TWO_PI );
		if( index >= 30 )
			index = 0;
		int x = ( int ) feature.pt.x;
		int y = ( int ) feature.pt.y;


#define ORBTEST( n ) ( IntegralImage::area( iimgptr, x + _patterns[ index ][ ( n ) * 2 ][ 0 ] - 2,\
										             y + _patterns[ index ][ ( n ) * 2 ][ 1 ] - 2, 5, 5, widthstep ) < \
					   IntegralImage::area( iimgptr, x + _patterns[ index ][ ( n ) * 2 + 1 ][ 0 ] - 2,\
										             y + _patterns[ index ][ ( n ) * 2 + 1 ][ 1 ] - 2, 5, 5, widthstep ) )
		int idx;
		for( int i = 0; i < 32; i++ ) {
			idx = i << 3;
			feature.desc[ i ]  =   ORBTEST( idx );
			feature.desc[ i ] |= ( ORBTEST( idx + 1 ) ) << 1;
			feature.desc[ i ] |= ( ORBTEST( idx + 2 ) ) << 2;
			feature.desc[ i ] |= ( ORBTEST( idx + 3 ) ) << 3;
			feature.desc[ i ] |= ( ORBTEST( idx + 4 ) ) << 4;
			feature.desc[ i ] |= ( ORBTEST( idx + 5 ) ) << 5;
			feature.desc[ i ] |= ( ORBTEST( idx + 6 ) ) << 6;
			feature.desc[ i ] |= ( ORBTEST( idx + 7 ) ) << 7;
		}
	}

	inline void ORB::matchBruteForce( std::vector<FeatureMatch>& matches, const FeatureDescriptorExtractor& other, float distThresh ) const
	{
		DistFunc dfunc;
		FeatureMatcher::matchBruteForce<Descriptor,DistFunc>( matches, this->_features, ( ( const ORB& ) other)._features, dfunc, distThresh );
	}

	inline void ORB::matchInWindow( std::vector<MatchingIndices>& matches,
									const std::vector<FeatureDescriptor*>& other,
									float maxFeatureDist,
									float maxDescDistance ) const
	{
		DistFunc dfunc;
		FeatureMatcher::matchInWindow<Descriptor, DistFunc>( matches,
															 other,
															 this->_features,
															 dfunc,
															 maxFeatureDist,
															 maxDescDistance );
	}

	inline void ORB::scanLineMatch( std::vector<FeatureMatch>& matches,
									const std::vector<const FeatureDescriptor*>& left,
									float minDisp,
									float maxDisp,
									float maxDescDist,
									float maxLineDist ) const
	{
		DistFunc dfunc;
		FeatureMatcher::scanLineMatch( matches,
									   left,
									   _features,
									   dfunc,
									   minDisp,
									   maxDisp,
									   maxDescDist,
									   maxLineDist );
	}
}

#endif
