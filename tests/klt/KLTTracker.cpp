#include "KLTTracker.h"

#include <cvt/math/Translation2D.h>

namespace cvt
{
	KLTTracker::KLTTracker( size_t maxIters, size_t patchSize ) :
		_maxIters( maxIters ),
		_patchSize( patchSize ),
		_ssdThresh( 0.0f )
	{
		_ssdThresh = Math::sqr( _patchSize ) * 300.0f;
	}

	void KLTTracker::trackFeatures( std::vector<size_t> & trackedIndices,
								    std::vector<Vector2f> & currPos,
									const Image& current, 
									const Image& last,																
									const std::vector<Vector2f> & lastPos )
	{
		size_t currStride, lastStride;
		const uint8_t* currImgPtr = current.map( &currStride );
		const uint8_t* lastImgPtr = last.map( &lastStride );

		size_t w = current.width();
		size_t h = current.height();
		
		Vector2f p;
		for( size_t i = 0; i < lastPos.size(); i++ ){
			// track each single feature
			const Vector2f & point = lastPos[ i ];

			if( !checkBounds( point, w, h ) )
				continue;

			if( trackSinglePatch( p, point, currImgPtr, currStride, lastImgPtr, lastStride, w, h ) ){
				trackedIndices.push_back( i );
				currPos.push_back( p );
			}
		}

		current.unmap( currImgPtr );
		last.unmap( lastImgPtr );
	}

	bool KLTTracker::trackSinglePatch( Vector2f & pos, const Vector2f & lastPos,
									   const uint8_t* current, size_t currStride,
									   const uint8_t* last,    size_t lastStride,
									   size_t width, size_t height )
	{
		// start from identity pose
		Eigen::Matrix<float, 2, 2> H; 
		Eigen::Matrix<float, 2, 2> Hinv; 
		Eigen::Matrix<float, 2, 1> J[ _patchSize * _patchSize ];
		H.setZero(); 

		size_t halfSize = _patchSize >> 1;

		size_t x = lastPos.x - halfSize; 
		size_t y = lastPos.y - halfSize;
		const uint8_t* startLine = last + y * lastStride + x;
		const uint8_t* lastLine  = startLine; 
		const uint8_t* nextLine  = lastLine + lastStride;


		Eigen::Vector2f g;

		size_t numLines = _patchSize;
		size_t currPix = 0;
		Eigen::Vector2f point;
		while( numLines-- ){
			for( size_t i = 0; i < _patchSize; i++, currPix++ ){
				g[ 0 ] = ( int16_t )lastLine[ i + 1 ] - ( int16_t )lastLine[ i ];
				g[ 1 ] = ( int16_t )nextLine[ i ] - ( int16_t )lastLine[ i ];
				J[ currPix ] = g;
				H +=  J[ currPix ] * g.transpose();
			}
			lastLine = nextLine;
			nextLine += lastStride;
		}
		bool invertable = true;
		H.computeInverseWithCheck( Hinv, invertable );

		if( !invertable )
			return false;

		size_t iter = 0;

		pos.x = lastPos.x;
		pos.y = lastPos.y;

		lastLine = startLine;
		const uint8_t* patchPtr = current;
		Eigen::Matrix<float, 2, 1> jSum;
		Eigen::Matrix<float, 2, 1> delta;
		float diffSum;
		while( iter < _maxIters ){
			jSum.setZero();
			patchPtr = current + ( ( ( size_t )pos.y - halfSize ) * currStride + ( size_t )pos.x - halfSize ); 
			lastLine = startLine;
			numLines = _patchSize;
			currPix  = 0;
			diffSum = 0.0f;
			while( numLines-- ){
				for( size_t i = 0; i < _patchSize; i++, currPix++ ){
					float deltaImg = ( int16_t )patchPtr[ i ] - ( int16_t )lastLine[ i ]; 
					diffSum += ( deltaImg * deltaImg );
					jSum += ( J[ currPix ] *  deltaImg );
				}
				lastLine += lastStride;
				patchPtr += currStride;		
			}

			// solve for the delta:
			delta = Hinv * jSum;

			pos[ 0 ] -= delta[ 0 ];
			pos[ 1 ] -= delta[ 1 ];

			if( !checkBounds( pos, width, height ) )
				return false;

			iter++;
		}

		if( diffSum > _ssdThresh ){
			return false;
		}

		
		return true;
	}
}

