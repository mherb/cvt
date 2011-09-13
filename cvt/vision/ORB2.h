#ifndef CVT_ORB2_H
#define CVT_ORB2_H

#include <cvt/vision/Feature2D.h>
#include <cvt/gfx/Image.h>
#include <cvt/vision/IntegralImage.h>
#include <cvt/util/SIMD.h>
#include <vector>
#include <iostream>
#include <iomanip>

namespace cvt {

	struct ORB2Feature : public Feature2Df {
        ORB2Feature( float x, float y, float angle = 0.0f, size_t octave = 0, float score = 0.0f ) :
            Feature2Df( x, y, angle, octave, score ),
		    sx( 1.0f ),
		    sy( 1.0f ),
			brighter( false )
        {
        }

		size_t distance( const ORB2Feature& f ) const
		{
			return SIMD::instance()->hammingDistance( desc, f.desc, 32 );
		}

	    float   sx, sy;
		bool	brighter;   // center pixel is brighter than the ring
		uint8_t desc[ 32 ]; // 256 bit vector
	};


	static inline std::ostream& operator<<( std::ostream& out, const ORB2Feature& feature )
	{
		out << "( " << feature.pt.x << " , " << feature.pt.y << " ) Orientation: " << Math::rad2Deg( feature.angle );
        out << " Score: " << feature.score;
		out << "\nDescriptor: 0x";
		for( int i = 0; i < 32; i++ )
			out << std::hex << std::setfill( '0' ) << std::setw( 2 ) << (int)feature.desc[ i ];
		out << std::endl;
		return out;
	}


	class ORB2 {
		friend bool _centroidAngleTest();
		public:
			ORB2( const Image& img, size_t octaves = 3, float scalefactor = 0.5f, uint8_t cornerThreshold = 25, size_t numFeatures = 1000, bool nms = true );
            ~ORB2();

			size_t size() const;
			size_t numBrighter() const;
			size_t numDarker() const;
			const ORB2Feature& operator[]( size_t index ) const;

			const ORB2Feature& brighterFeature( size_t index ) const;
			const ORB2Feature& darkerFeature( size_t index ) const;

		private:
			void detect( const Image& img, size_t octave );
            void extract( size_t octaves );

			void centroidAngle( ORB2Feature& feature, const float* ptr, size_t widthstep );

			void descriptor( ORB2Feature& feature, const float* ptr, size_t widthstep );
			//void multiChannelDescriptor( ORB2Feature& feature, const float* ptr, size_t widthstep );

            void nonmaxSuppression( const std::vector<ORB2Feature> & features );

			void selectBestFeatures( size_t num );

			/**
			 * Test if the feautre pointed to by ptr is brighter or darker
			 */
			bool isBrighterFeature( const uint8_t * ptr, size_t stride ) const;

			typedef std::vector<ORB2Feature>  ContainerType;
			typedef std::vector<ORB2Feature*> ContainerPtrType;
			ContainerType		_features;
			ContainerPtrType	_brighterFeatures;
			ContainerPtrType	_darkerFeatures;

            IntegralImage*	_iimages;
            float*			_scaleFactors;
			size_t			_currentOctave;

            // for OFAST
            uint8_t			 _threshold;
			size_t			 _numFeatures;
            bool             _nms;

			static const int _patterns[ 30 ][ 512 ][ 2 ];
			static const int _circularoffset[ 31 ];

            // border where we do not detect features:
            // 17+2 17->maximum test coord within patch + 2 for the integral image access
            static const int _border = 42;
			
	};

	inline size_t ORB2::size() const
	{
		return _features.size();
	}

	inline size_t ORB2::numBrighter() const 
	{
		return _brighterFeatures.size();
	}

	inline size_t ORB2::numDarker() const 
	{
		return _darkerFeatures.size();
	}

	inline const ORB2Feature& ORB2::operator[]( size_t index ) const
	{
		return _features[ index ];
	}

	inline const ORB2Feature& ORB2::brighterFeature( size_t index ) const
	{
		return *_brighterFeatures[ index ];
	}
	
	inline const ORB2Feature& ORB2::darkerFeature( size_t index ) const
	{
		return *_darkerFeatures[ index ];
	}
}

#endif
