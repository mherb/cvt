#ifndef CVT_KLT_TRACKER_H
#define CVT_KLT_TRACKER_H

#include <cvt/math/Vector.h>
#include <cvt/gfx/Image.h>
#include <vector>

#include <cvt/vision/KLTPatch.h>
#include <cvt/math/Translation2D.h>
#include <cvt/util/EigenBridge.h>

namespace cvt
{
	template <class PoseType>
	class KLTTracker
	{
		public:
			typedef KLTPatch<16, PoseType>	KLTPType;

			KLTTracker( size_t maxIters = 20 );

			void trackFeatures( std::vector<size_t> & trackedIndices,
							    std::vector<PoseType> & poses,
							    const std::vector<KLTPType*>& patches,
							    const Image& current );

			void trackMultiscale( std::vector<size_t> & trackedIndices,
								  std::vector<PoseType> & poses,
								  const std::vector<KLTPType*>& patches,
								  const std::vector<Image>& pyramid );

			float ssdThreshold() const { return _ssdThresh; }
			void setSSDThreshold( float ssd ) { _ssdThresh = ssd; }

		private:
			size_t _maxIters;
			size_t _patchSize;
			float  _ssdThresh;
			
			bool trackSinglePatch( PoseType& pose, 
								   const Vector2f & tempPos,
								   const KLTPType& patch,
								   const uint8_t* current, size_t currStride,
								   size_t width, size_t height );

			bool checkBounds( const Eigen::Vector2f & p, size_t width, size_t height ) const
			{
				size_t h = _patchSize >> 1;
				if( p[ 0 ] <= h || p[ 1 ] <= h || p[ 0 ] + h >= width || p[ 1 ] + h >= height )
					return false;
				return true;
			}

	};


	template <class PoseType>
	inline KLTTracker<PoseType>::KLTTracker( size_t maxIters ) :
		_maxIters( maxIters ),
		_patchSize( 16 ),
		_ssdThresh( 0.0f )
	{
		_ssdThresh = Math::sqr( 20.0f );
	}

	template <class PoseType>
	inline void KLTTracker<PoseType>::trackFeatures( std::vector<size_t> & trackedIndices,
									std::vector<PoseType> & poses,
									const std::vector<KLTPType*>& patches,
									const Image& current )
	{
		size_t currStride;
		const uint8_t* currImgPtr = current.map( &currStride );

		size_t w = current.width();
		size_t h = current.height();	

		Eigen::Vector2f pp;
		Eigen::Vector2f p2;
		for( size_t i = 0; i < patches.size(); i++ ){
			// track each single feature
			PoseType & pose = poses[ i ];
			const KLTPType& patch = *patches[ i ];

			EigenBridge::toEigen( p2, patch.position() );
		   	pose.transformInverse( pp, p2 );
			if( !checkBounds( pp, w, h ) ){
				continue;
			}

			if( trackSinglePatch( pose, patch.position(), patch, currImgPtr, currStride, w, h ) ){
				trackedIndices.push_back( i );
			}
		}

		current.unmap( currImgPtr );
	}

	template <class PoseType>
	inline bool KLTTracker<PoseType>::trackSinglePatch( PoseType & pose, 
									   const Vector2f & tempPos,
									   const KLTPType& patch,
									   const uint8_t* current, size_t currStride,
									   size_t width, size_t height )
	{
		size_t halfSize = _patchSize >> 1;
		
		Eigen::Vector2f p2;
		Eigen::Vector2f pp;

		size_t iter = 0;

		Vector2f point;
		typename KLTPType::JacType jSum;
		typename PoseType::ParameterVectorType delta;
		float diffSum = 0;
		size_t npix = 0;
		size_t numLines;
		while( iter < _maxIters ){
			jSum.setZero();
			numLines = _patchSize;
			diffSum = 0.0f;
			npix = 0;

			p2[ 1 ] = tempPos.y - halfSize; 
			const typename KLTPType::JacType* J = patch.jacobians();
			const uint8_t* temp = patch.pixels();
			while( numLines-- ){
				p2[ 0 ] = tempPos.x - halfSize; 
				for( size_t i = 0; i < _patchSize; i++ ){
					pose.transformInverse( pp, p2 );
					if( ( size_t )pp[ 0 ] < width && ( size_t )pp[ 1 ] < height ){
						float deltaImg = ( int16_t )current[ ( size_t )pp[ 1 ] * currStride + ( size_t )pp[ 0 ] ] - ( int16_t )*temp;
						diffSum += ( deltaImg * deltaImg );
						jSum += ( *J *  deltaImg );
						npix++;
					}

					temp++;
					J++;
					p2[ 0 ] += 1;
				}
				p2[ 1 ] += 1;

			}
			if( npix < 100 )
				return false;

			// solve for the delta:
			delta = patch.inverseHessian() * jSum;
			pose.applyInverse( delta );
			
			iter++;
		}

		if( ( diffSum / npix ) > _ssdThresh ){
			return false;
		}
		return true;
	}

	template <class PoseType>
	inline void KLTTracker<PoseType>::trackMultiscale( std::vector<size_t> & trackedIndices,
									  std::vector<PoseType> & poses,
									  const std::vector<KLTPType*>& patches,
									  const std::vector<Image>& pyramid )
	{
		int i = pyramid.size() - 1;
		float allScale   = ( float )pyramid[ i ].width() / pyramid[ 0 ].width();
		float interScale = ( float )pyramid[ i - 1 ].width() / pyramid[ i ].width();

		// map all:
		std::vector<const uint8_t*> ptr;
		std::vector<size_t>     stride;
		ptr.resize( pyramid.size() );
		stride.resize( pyramid.size() );
		for( size_t i = 0; i < pyramid.size(); i++ ){
			ptr[ i ] = pyramid[ i ].map( &stride[ i ] );
		}

		float origSSD = _ssdThresh;


		Vector2f currPosition;
		for( size_t p = 0; p < patches.size(); p++ ){
			const KLTPType & patch = *patches[ p ];
			currPosition = allScale * patch.position();
			PoseType & currPose = poses[ p ];
			currPose.scale( allScale );

			i = pyramid.size() - 1;
			bool tracked = true;
			_ssdThresh = origSSD / allScale;
			while( i >= 0 ){
				tracked = trackSinglePatch( currPose,
										    currPosition,
											patch,
										   	ptr[ i ],
										   	stride[ i ],
											pyramid[ i ].width(), 
											pyramid[ i ].height() );
										  
				if( !tracked )
					break;
				if( i ){
					currPosition *= interScale;
					currPose.scale( interScale );
				}
				i--;
				_ssdThresh /= interScale;
			}
		
			if( tracked ){
				trackedIndices.push_back( p );
			}
			_ssdThresh = origSSD;
		}

		for( size_t i = 0; i < pyramid.size(); i++ ){
			pyramid[ i ].unmap( ptr[ i ] );
		}
	}
}

#endif
