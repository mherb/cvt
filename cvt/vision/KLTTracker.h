/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_KLT_TRACKER_H
#define CVT_KLT_TRACKER_H

#include <cvt/math/Vector.h>
#include <cvt/geom/Line2D.h>
#include <cvt/gfx/Image.h>
#include <vector>

#include <cvt/vision/KLTPatch.h>
#include <cvt/math/Translation2D.h>
#include <cvt/math/Math.h>
#include <cvt/util/EigenBridge.h>


namespace cvt
{
	template <class PoseType, size_t pSize=16>
		class KLTTracker
		{
			public:
				typedef KLTPatch<pSize, PoseType>	KLTPType;

				KLTTracker( size_t maxIters = 20 );

				/**
				 *	\brief return the current ssd threshold
				 */
				float ssdThreshold() const { return _ssdThresh; }

				/**
				 * \brief	set max. avg. SSD thresh per pixel
				 */
				void  setSSDThreshold( float ssd ) { _ssdThresh = Math::sqr( pSize ) * ssd; }

				/**
				 *	\brief	set the maximum number of iterations
				 */
				void setMaxIterations( size_t maxIter ) { _maxIters = maxIter; }

				/**
				 *	\brief	get the maximum number of iterations
				 */
				size_t maxIterations() const { return _maxIters; }

				/* track a single scale patch */
				bool trackPatch( KLTPType& patch,
								const uint8_t* current, size_t currStride,
								size_t width, size_t height );


				/* track patch using scale space */
				bool trackPatchMultiscale( KLTPType& patch,
										  const ImagePyramid& pyramid );

			private:
				size_t _maxIters;
				float  _ssdThresh;

				/* warps all the patch pixels */
				float buildSystem( KLTPType& patch, typename KLTPType::JacType& jacSum, const Matrix3f& pose, const uint8_t* iPtr, size_t iStride, size_t octave = 0 );

				bool patchIsInImage( const Matrix3f& pose, size_t w, size_t h ) const;
		};


		template <class PoseType, size_t pSize>
		inline KLTTracker<PoseType, pSize>::KLTTracker( size_t maxIters ) :
			_maxIters( maxIters ),
			_ssdThresh( 0.0f )
		{
			setSSDThreshold( Math::sqr( 40 ) );
		}

		template <class PoseType, size_t pSize>
		inline bool KLTTracker<PoseType, pSize>::trackPatch( KLTPType& patch,
															 const uint8_t* current, size_t currStride,
															 size_t width, size_t height )
		{
			typename KLTPType::JacType jSum;
			typename PoseType::ParameterVectorType delta;

			float diffSum = 0.0f;
			size_t iter = 0;
			while( iter < _maxIters ){
				jSum.setZero();
				diffSum = 0.0f;

				//pose matrix
				Matrix3f pose;
				EigenBridge::toCVT( pose, patch.pose().transformation() );

				// first test if all points transform into the image
				if( !patchIsInImage( pose, width, height ) ){
					return false;
				}

				diffSum = buildSystem( patch, jSum, pose, current, currStride );

				// solve for the delta:
				delta = patch.inverseHessian() * jSum;
				patch.pose().applyInverse( -delta );

				iter++;
			}

			if( diffSum > _ssdThresh ){
				return false;
			}
			return true;
		}

		template <class PoseType, size_t pSize>
		bool KLTTracker<PoseType, pSize>::trackPatchMultiscale( KLTPType& patch,
															    const ImagePyramid& pyramid )
		{
			typename KLTPType::JacType jSum;
			typename PoseType::ParameterVectorType delta;

			size_t nOctaves = pyramid.octaves();
			float scale = Math::pow( pyramid.scaleFactor(), nOctaves-1 );
			float invScale = 1.0f / pyramid.scaleFactor();
			
			// get the pose of the patch
			Matrix3f poseMat;
			EigenBridge::toCVT( poseMat, patch.pose().transformation() );
			poseMat[ 0 ][ 2 ] *= scale;
			poseMat[ 1 ][ 2 ] *= scale;

			PoseType tmpPose;
			tmpPose.set( poseMat );

			const float maxDiff = Math::sqr( pSize * 255.0f );
			float diffSum = maxDiff;
			for( int oc = pyramid.octaves() - 1; oc >= 0; --oc ){ 
				IMapScoped<const uint8_t> map( pyramid[ oc ] );
				size_t w = pyramid[ oc ].width();
				size_t h = pyramid[ oc ].height();
				size_t iter = 0;
				diffSum = Math::sqr( pSize * 255 );
				while( iter < _maxIters ){
					//pose matrix
					EigenBridge::toCVT( poseMat, tmpPose.transformation() );

					// first test if all points transform into the image
					if( !patchIsInImage( poseMat, w, h ) ){
						return false;
					}

					jSum.setZero();
					diffSum = buildSystem( patch, jSum, poseMat, 
										   map.ptr(), map.stride(),
										   oc );

					if( diffSum >= maxDiff ){
						return false;
					}


					// solve for the delta:
					delta = patch.inverseHessian( oc ) * jSum;

					if( delta.norm() < 1e-7 )
						break;

					tmpPose.applyInverse( -delta );
					
					iter++;
				}

				if( oc != 0 ){
					// we need to scale up
					EigenBridge::toCVT( poseMat, tmpPose.transformation() );
					poseMat[ 0 ][ 2 ] *= invScale;
					poseMat[ 1 ][ 2 ] *= invScale;
					tmpPose.set( poseMat );
				}
			}
			
			// set the final patch pose accordingly
			patch.pose().transformation() = tmpPose.transformation();

			// if the ssd is below the threshold
			if( diffSum > _ssdThresh ){
				return false;
			}

			return true;
		}

		template <class PoseType, size_t pSize>
		float KLTTracker<PoseType, pSize>::buildSystem( KLTPType& patch, 
													    typename KLTPType::JacType& jacSum, 
														const Matrix3f& pose, 
														const uint8_t* imgPtr, size_t iStride,
													    size_t octave )
		{
			// the warped current ones
			uint8_t* warped = patch.transformed( octave );
			// the original template pixels
			const uint8_t* temp = patch.pixels( octave );
			const typename KLTPType::JacType* J = patch.jacobians( octave );

			static const float half = pSize >> 1;
			float diffSum = 0.0f;

			for( size_t i = 0; i < 3; i++ )
				for( size_t k = 0; k < 3; k++ )
					if( Math::isNaN( pose[ i ][ k ] ) )
						return Math::MAXF;
			
			Vector2f pcur, ppcur;
			pcur.y = -half;
			for( size_t y = 0; y < pSize; y++ ){
				pcur.x = -half;
				for( size_t i = 0; i < pSize; i++ ){
					ppcur = pose * pcur;

					int ix = ( int )ppcur.x;
					int iy = ( int )ppcur.y;
					float fracx = ppcur.x - ix;
					float fracy = ppcur.y - iy;

					const uint8_t* px = imgPtr + iy * iStride + ix;
					uint8_t v0 = Math::mix( px[ 0 ], px[ 1 ], fracx );
					px += iStride;

					uint8_t v1 = Math::mix( px[ 0 ], px[ 1 ], fracx );
					v0 = Math::mix( v0, v1, fracy );
					warped[ y * pSize + i ] = v0; 

					float deltaImg = ( int16_t )v0 - ( int16_t )temp[ y * pSize + i ];
					diffSum += Math::sqr( deltaImg );
					jacSum += ( *J *  deltaImg );
					J++;
					pcur.x += 1.0f;
				}
				pcur.y += 1.0f;
			}
			return diffSum;
		}

		template <class PoseType, size_t pSize>
		bool KLTTracker<PoseType, pSize>::patchIsInImage( const Matrix3f& pose, size_t w, size_t h ) const
		{
			static const float half = pSize >> 1;
			static const Vector2f a( -half, -half );
			static const Vector2f b(  half, -half );
			static const Vector2f c(  half,  half );
			static const Vector2f d( -half,  half );

			Vector2f pWarped;

			pWarped = pose * a;
			if( pWarped.x < 0.0f || pWarped.x > ( w-1 ) ||
			   pWarped.y < 0.0f || pWarped.y > ( h-1 ) )
				return false;
			pWarped = pose * b;
			if( pWarped.x < 0.0f || pWarped.x > ( w-1 ) ||
			   pWarped.y < 0.0f || pWarped.y > ( h-1 ) )
				return false;

			pWarped = pose * c;
			if( pWarped.x < 0.0f || pWarped.x > ( w-1 ) ||
			   pWarped.y < 0.0f || pWarped.y > ( h-1 ) )
				return false;
			pWarped = pose * d;
			if( pWarped.x < 0.0f || pWarped.x > ( w-1 ) ||
			   pWarped.y < 0.0f || pWarped.y > ( h-1 ) )
				return false;
			return true;
		}

}

#endif
