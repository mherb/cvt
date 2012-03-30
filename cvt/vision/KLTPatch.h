/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_KLT_PATCH_H
#define CVT_KLT_PATCH_H

#include <Eigen/Dense>
#include <cvt/vision/ImagePyramid.h>
#include <cvt/gfx/IMapScoped.h>

namespace cvt
{
	template <size_t pSize, class PoseType>
	class KLTPatch
	{
		public:
			typedef Eigen::Matrix<float, PoseType::NPARAMS, PoseType::NPARAMS> HessType;
			typedef Eigen::Matrix<float, PoseType::NPARAMS, 1>				   JacType;

			KLTPatch( size_t octaves = 1 );

			/**
			 *	\brief update the internal patch data
			 *	\param ptr		ptr to the image
			 *	\param stride	stride of the image
			 *	\param pos		position of the feature in the image
			 * */
			bool update( const uint8_t* ptr, size_t stride, const Vector2f& pos, size_t octave = 0 );

			void currentCenter( Vector2f& center )  const; 
			
			PoseType&	pose()	{ return _pose; }
			void		initPose( const Vector2f& pos );

			const uint8_t*   pixels( size_t octave = 0 ) const { return _patchDataForScale[ octave ].patch; }
        
			const uint8_t*   transformed( size_t octave = 0 )  const { return _patchDataForScale[ octave ].transformed; }    
			uint8_t*		 transformed( size_t octave = 0 )		 { return _patchDataForScale[ octave ].transformed; }    

			const HessType&  inverseHessian( size_t octave = 0 ) const { return _patchDataForScale[ octave ].inverseHessian; }
			const JacType*   jacobians( size_t octave = 0 )		 const { return _patchDataForScale[ octave ].jac; }

			size_t size() const { return pSize; }

			/**
			 *	\brief	number of scales that are stored with this patch
			 * */
			size_t numScales() const { return _patchDataForScale.size(); }

			static void extractPatches( std::vector<KLTPatch<pSize, PoseType>* > & patches,
									    const std::vector<Vector2f> & positions, 
										const Image & img );
			
			/**
			 * \brief extract patches a multiscale fashion from a image pyramid 
			 * */
			static void extractPatches( std::vector<KLTPatch<pSize, PoseType>* > & patches,
									    const std::vector<Vector2f> & positions, 
										const ImagePyramid & pyramid );
		private:
			/* this stores the transform from the 
			 * Template space to the image space*/
			PoseType	_pose;

			struct PatchData
			{
				/* the pixel original information */
				uint8_t		patch[ pSize * pSize ];
			
				/* the transformed information */
				uint8_t		transformed[ pSize * pSize ];

				HessType	inverseHessian;
				JacType		jac[ pSize * pSize ];

			};

			std::vector<PatchData>	_patchDataForScale;
			float					_scaleFactor;

			KLTPatch( const KLTPatch& );
			KLTPatch& operator= (const KLTPatch& );
	};

	template <size_t pSize, class PoseType>
	inline KLTPatch<pSize, PoseType>::KLTPatch( size_t octaves ) : 
		_scaleFactor( 1.0f )
	{
		_patchDataForScale.resize( octaves );
	}

	template <size_t pSize, class PoseType>
	inline bool KLTPatch<pSize, PoseType>::update( const uint8_t* imgPtr, size_t stride, const Vector2f& pos, size_t octave )
	{
		const float pHalf = ( pSize >> 1 );
		const uint8_t* ptr = imgPtr + ( int )( pos.y - pHalf ) * stride + ( int )( pos.x - pHalf );
		const uint8_t* nextLine = ptr + stride;
		const uint8_t* prevLine = ptr - stride;

		size_t numLines = pSize;

		JacType* J = _patchDataForScale[ octave ].jac;
		HessType& invH = _patchDataForScale[ octave ].inverseHessian;
		uint8_t* p = _patchDataForScale[ octave ].patch;
		uint8_t* t = _patchDataForScale[ octave ].transformed;

		Eigen::Matrix<float, 2, 1> g;
		HessType hess( HessType::Zero() );
		typename PoseType::ScreenJacType sj;

		Eigen::Vector2f point( -pHalf, -pHalf );
		while( numLines-- )
		{			
			point[ 0 ] = -pHalf;
			for( size_t i = 0; i < pSize; i++ ){
				*p = *t = ptr[ i ];

				// sobel style
				g[ 0 ] = -( int16_t )prevLine[ i - 1 ] + ( int16_t )prevLine[ i + 1 ]
					     + 2 * ( -( int16_t ) ptr[ i - 1 ] + ptr[ i + 1 ] )
						 -( int16_t )nextLine[ i - 1 ] + ( int16_t )nextLine[ i + 1 ];

				g[ 1 ] = ( int16_t )nextLine[ i - 1 ] + 2 * ( int16_t )nextLine[ i ] + ( int16_t )nextLine[ i + 1 ]
						-( int16_t )prevLine[ i - 1 ] - 2 * ( int16_t )prevLine[ i ] - ( int16_t )prevLine[ i + 1 ];
				g *= 0.125f;

				// simple central derivative
				//g[ 0 ] = ( int16_t )ptr[ i + 1 ] - ( int16_t )ptr[ i - 1 ];
				//g[ 1 ] = ( int16_t )nextLine[ i ] - ( int16_t )prevLine[ i ];
				//g *= 0.5f;

				_pose.screenJacobian( sj, point );
				*J =  sj.transpose() * g;

				hess += ( *J ) * J->transpose();
				J++;
				p++;
				t++;

				point[ 0 ] += 1.0f;
			}

			prevLine = ptr;
			ptr = nextLine;
			nextLine += stride;
			point[ 1 ] += 1.0f;
		}
		
		// initialize the _pose if at upper most octave
		if( octave == 0 )
			initPose( pos );

		// TODO: maybe better check by evaluating the determinant of H?
		bool invertable = true;
		invH = hess.inverse();
		if( ( invH * hess - HessType::Identity() ).array().sum() / PoseType::NPARAMS > 1.01 )
			invertable = false;

		return invertable;
	}

	template <size_t pSize, class PoseType>
	inline void KLTPatch<pSize, PoseType>::extractPatches( std::vector<KLTPatch<pSize, PoseType>* > & patches,
														   const std::vector<Vector2f> & positions, 
														   const Image & img )
	{
		size_t s;
		const uint8_t* ptr = img.map( &s );

		size_t w = img.width();
		size_t h = img.height();
		size_t phalf = pSize >> 1;

		int x, y;

		KLTPatch<pSize, PoseType>* patch = 0;

		for( size_t i = 0; i < positions.size(); i++ ){
			x = positions[ i ].x;
			y = positions[ i ].y;

			if( x < ( int )phalf || ( x + phalf - 1 ) > w ||
			    y < ( int )phalf || ( y + phalf - 2 ) > h )
				continue;

			if( patch == 0 )	
				patch = new KLTPatch<pSize, PoseType>();

			if( patch->update( ptr, s, positions[ i ] ) ){
				patches.push_back( patch );
				patch = 0;
			}
		}

		if( patch )
			delete patch;

		img.unmap( ptr );
	}
	
	template <size_t pSize, class PoseType>
	inline void KLTPatch<pSize, PoseType>::extractPatches( std::vector<KLTPatch<pSize, PoseType>* > & patches,
														   const std::vector<Vector2f> & positions, 
														   const ImagePyramid & pyr )
	{


		int x, y;

		KLTPatch<pSize, PoseType>* patch = 0;


		std::vector<const uint8_t*> maps;
		std::vector<size_t> strides;
		std::vector<float> scales;

		maps.resize( pyr.octaves() );
		strides.resize( pyr.octaves() );
		scales.resize( pyr.octaves() );

		maps[ 0 ] = pyr[ 0 ].map( &strides[ 0 ] );
		scales[ 0 ] =  1.0f;
		for( size_t i = 1; i < pyr.octaves(); i++ ){
			maps[ i ] = pyr[ i ].map( &strides[ i ] );
			scales[ i ] = scales[ i - 1 ] * pyr.scaleFactor();
		}

		Vector2f octavePos;
		size_t phalf = pSize >> 1;
		for( size_t i = 0; i < positions.size(); i++ ){

			float fx = positions[ i ].x;
			float fy = positions[ i ].y;

			bool isGood = true;
			if( patch == 0 )	
				patch = new KLTPatch<pSize, PoseType>( pyr.octaves() );
			for( int o = pyr.octaves()-1; o >= 0; o-- ){
				x = scales[ o ] * fx;
				y = scales[ o ] * fy;
		
				size_t w = pyr[ o ].width();
				size_t h = pyr[ o ].height();

				octavePos = positions[ i ] * scales[ o ];

				if( x < ( int )phalf || ( x + phalf - 1 ) > w ||
				    y < ( int )phalf || ( y + phalf - 2 ) > h ){
					isGood = false;
					break;
				}

				if( !patch->update( maps[ o ], strides[ o ], octavePos, o ) ){
					isGood = false;
					break;
				}
			}

			if( isGood ){
				patches.push_back( patch );
				patch = 0;
			}
		}

		if( patch )
			delete patch;
	
		// unmap	
		for( size_t i = 0; i < maps.size(); i++ ){
			pyr[ i ].unmap( maps[ i ] );
		}
	}

	template <size_t pSize, class PoseType>
	inline void KLTPatch<pSize, PoseType>::currentCenter( Vector2f& center ) const
	{
		const Eigen::Matrix3f& tmp = _pose.transformation();
		center.x = tmp( 0, 2 );
		center.y = tmp( 1, 2 );
	}
	
	template <size_t pSize, class PoseType>
	inline void KLTPatch<pSize, PoseType>::initPose( const Vector2f& pos ) 
	{
		Matrix3f m;
		m.setIdentity();
		m[ 0 ][ 2 ] = pos.x;
		m[ 1 ][ 2 ] = pos.y;
		_pose.set( m );
	}

}

#endif
