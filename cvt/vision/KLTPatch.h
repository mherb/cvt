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

#include <cvt/math/Translation2D.h>
#include <Eigen/Dense>

namespace cvt
{
	template <size_t pSize, class PoseType>
	class KLTPatch
	{
		public:
			typedef Eigen::Matrix<float, PoseType::NPARAMS, PoseType::NPARAMS> HessType;
			typedef Eigen::Matrix<float, PoseType::NPARAMS, 1>				   JacType;

			KLTPatch();

			/**
			 *	\brief update the internal patch data
			 *	\param ptr		ptr to the image
			 *	\param stride	stride of the image
			 *	\param pos		position of the feature in the image
			 * */
			bool update( const uint8_t* ptr, size_t stride, const Vector2f& pos );

			void currentCenter( Vector2f& center )  const; 
			
			PoseType&	pose()	{ return _pose; }
			void		initPose( const Vector2f& pos );

			const uint8_t*   pixels()		const { return _patch; }
        
			const uint8_t*   transformed()  const { return _transformed; }    
            uint8_t*   transformed() { return _transformed; }

			const HessType&  inverseHessian() const { return _inverseHessian; }
			const JacType*   jacobians()	  const { return _jac; }

			size_t size() const { return pSize; }


			static void extractPatches( std::vector<KLTPatch<pSize, PoseType>* > & patches,
									    const std::vector<Vector2f> & positions, 
										const Image & img );
		private:
			/* this stores the transform from the 
			 * Template space to the image space*/
			PoseType	_pose;

			/* the pixel original information */
			uint8_t		_patch[ pSize * pSize ];
			uint8_t		_transformed[ pSize * pSize ];
			HessType	_inverseHessian;
			JacType		_jac[ pSize * pSize ];

			KLTPatch( const KLTPatch& );
			KLTPatch& operator= (const KLTPatch& );
	};

	template <size_t pSize, class PoseType>
	inline KLTPatch<pSize, PoseType>::KLTPatch()
	{
	}

	template <size_t pSize, class PoseType>
	inline bool KLTPatch<pSize, PoseType>::update( const uint8_t* imgPtr, size_t stride, const Vector2f& pos )
	{
		const size_t pHalf = ( pSize >> 1 );
		const uint8_t* ptr = imgPtr + ( int )( pos.y - pHalf ) * stride + ( int )pos.x - pHalf;
		const uint8_t* nextLine = ptr + stride;
		const uint8_t* prevLine = ptr - stride;

		size_t numLines = pSize;

		JacType* J = _jac;
		uint8_t* p = _patch;
		uint8_t* t = _transformed;

		Eigen::Matrix<float, 2, 1> g;
		HessType hess( HessType::Zero() );
		typename PoseType::ScreenJacType sj;

		Eigen::Vector2f point( 0.0f, 0.0f );
		while( numLines-- )
		{
			for( size_t i = 0; i < pSize; i++ ){
				point[ 0 ] = i;
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
			}

			prevLine = ptr;
			ptr = nextLine;
			nextLine += stride;
			point[ 1 ] += 1.0f;
		}
		
		// initialize the _pose
		initPose( pos );

		bool invertable = true;
		_inverseHessian = hess.inverse();
		if( ( _inverseHessian * hess - HessType::Identity() ).array().sum() / PoseType::NPARAMS > 1.01 )
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
	inline void KLTPatch<pSize, PoseType>::currentCenter( Vector2f& center ) const
	{
		Eigen::Vector2f p0( pSize / 2.0f, pSize / 2.0f );
		Eigen::Vector2f curCenter;
		_pose.transform( curCenter, p0 );
		
		EigenBridge::toCVT( center, curCenter );		
	}
	
	template <size_t pSize, class PoseType>
	inline void KLTPatch<pSize, PoseType>::initPose( const Vector2f& pos ) 
	{
		Matrix3f m;
		m.setIdentity();
		const float sizeHalf = pSize / 2.0f;
		m[ 0 ][ 2 ] = pos.x - sizeHalf;
		m[ 1 ][ 2 ] = pos.y - sizeHalf;
		_pose.set( m );
	}

}

#endif
