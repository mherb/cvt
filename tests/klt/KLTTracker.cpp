#include "KLTTracker.h"

#include <cvt/math/Translation2D.h>
#include <cvt/util/EigenBridge.h>

namespace cvt
{
	KLTTracker::KLTTracker( size_t maxIters ) :
		_maxIters( maxIters ),
		_patchSize( 16 ),
		_ssdThresh( 0.0f )
	{
		_ssdThresh = Math::sqr( _patchSize ) * 300.0f;
	}

	void KLTTracker::trackFeatures( std::vector<size_t> & trackedIndices,
									std::vector<PoseType> & poses,
									const std::vector<KLTPatch<16> >& patches,
									const Image& current )
	{
		size_t currStride;
		const uint8_t* currImgPtr = current.map( &currStride );

		size_t w = current.width();
		size_t h = current.height();	

		Vector2f p;
		Eigen::Vector2f pp;
		Eigen::Vector2f p2;
		for( size_t i = 0; i < patches.size(); i++ ){
			// track each single feature
			PoseType & pose = poses[ i ];
			p = patches[ i ].position();

			EigenBridge::toEigen( p2, p );
		   	pose.transformInverse( pp, p2 );
			EigenBridge::toCVT( p, pp );
			if( !checkBounds( p, w, h ) ){
				continue;
			}

			if( trackSinglePatch( pose, patches[ i ].position(), patches[ i ], currImgPtr, currStride, w, h ) ){
				trackedIndices.push_back( i );
			}
		}

		current.unmap( currImgPtr );
	}

	bool KLTTracker::trackSinglePatch( PoseType & pose, 
									   const Vector2f & tempPos,
									   const KLTPatch<16> & patch,
									   const uint8_t* current, size_t currStride,
									   size_t width, size_t height )
	{
		size_t halfSize = _patchSize >> 1;
		
		Eigen::Vector2f p2;
		Eigen::Vector2f pp;

		size_t iter = 0;

		Vector2f point;
		KLTPatch<16>::JacType jSum;
		Eigen::Matrix<float, 2, 1> delta;
		float diffSum;
		size_t numLines;
		while( iter < _maxIters ){
			jSum.setZero();
			numLines = _patchSize;
			diffSum = 0.0f;

			p2[ 1 ] = tempPos.y - halfSize; 
			const KLTPatch<16>::JacType* J = patch.jacobians();
			const uint8_t* temp = patch.pixels();
			while( numLines-- ){
				p2[ 0 ] = tempPos.x - halfSize; 
				for( size_t i = 0; i < _patchSize; i++ ){
					pose.transformInverse( pp, p2 );
					float deltaImg = ( int16_t )current[ ( size_t )pp[ 1 ] * currStride + ( size_t )pp[ 0 ] ] - ( int16_t )*temp;
					diffSum += ( deltaImg * deltaImg );
					jSum += ( *J *  deltaImg );

					temp++;
					J++;
					p2[ 0 ] += 1;
				}
				p2[ 1 ] += 1;
			}

			// solve for the delta:
			delta = patch.inverseHessian() * jSum;
			
			pose.applyInverse( delta );
			
			p2[ 0 ] = tempPos.x;
			p2[ 1 ] = tempPos.y;
			pose.transformInverse( pp, p2 );
			EigenBridge::toCVT( point, pp );
			if( !checkBounds( point, width, height ) ){
				return false;
			}

			iter++;
		}

		if( diffSum > _ssdThresh ){
			return false;
		}
		return true;
	}


	void KLTTracker::trackMultiscale( std::vector<size_t> & trackedIndices,
									  std::vector<PoseType> & poses,
									  const std::vector<KLTPatch<16> >& patches,
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
			const KLTPatch<16> & patch = patches[ p ];
			currPosition = allScale * patches[ p ].position();
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

