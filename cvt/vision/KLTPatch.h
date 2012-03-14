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

			bool update( const uint8_t* ptr, size_t stride );

				  Vector2f & position()		   { return _pos; }
			const Vector2f & position()  const { return _pos; }
			const uint8_t*   pixels()    const { return _patch; }

			const HessType&  inverseHessian() const { return _inverseHessian; }
			const JacType*   jacobians()	  const { return _jac; }

			size_t size() const { return pSize; }


			static void extractPatches( std::vector<KLTPatch<pSize, PoseType>* > & patches,
									    const std::vector<Vector2f> & positions, 
										const Image & img );
		private:
			Vector2f	_pos;
			uint8_t		_patch[ pSize * pSize ];
			HessType	_inverseHessian;
			JacType		_jac[ pSize * pSize ];
	};

	template <size_t pSize, class PoseType>
	inline KLTPatch<pSize, PoseType>::KLTPatch()
	{
	}

	template <size_t pSize, class PoseType>
	inline bool KLTPatch<pSize, PoseType>::update( const uint8_t* ptr, size_t stride )
	{
		const uint8_t* nextLine = ptr + stride;
		const uint8_t* prevLine = ptr - stride;

		size_t numLines = pSize;

		JacType* J = _jac;
		uint8_t* p = _patch;

		Eigen::Matrix<float, 2, 1> g;
		HessType hess( HessType::Zero() );
		PoseType pose;
		typename PoseType::ScreenJacType sj;

		Eigen::Vector2f point( 0.0f, 0.0f );
		while( numLines-- )
		{
			for( size_t i = 0; i < pSize; i++ ){
				point[ 0 ] = i;
				*p = ptr[ i ];

				g[ 0 ] = ( int16_t )ptr[ i + 1 ] - ( int16_t )ptr[ i - 1 ];
				g[ 1 ] = ( int16_t )nextLine[ i ] - ( int16_t )prevLine[ i ];
				
			//	g[ 0 ]  = 0.25f * ( ( int16_t ) prevLine[ i + 1 ] - ( int16_t ) prevLine[ i - 1 ] );
			//	g[ 0 ] += 0.5f  * ( ( int16_t ) ptr[ i + 1 ]	  -  ( int16_t ) ptr[ i - 1 ] );
			//	g[ 0 ] += 0.25f * ( ( int16_t ) nextLine[ i + 1 ] - ( int16_t ) nextLine[ i - 1 ] );

			//	g[ 1 ]  = 0.25f * ( ( int16_t )nextLine[ i - 1 ] - ( int16_t )prevLine[ i - 1 ] );
			//	g[ 1 ] += 0.5f  * ( ( int16_t )nextLine[ i ] - ( int16_t )prevLine[ i ] );
			//	g[ 1 ] += 0.25f * ( ( int16_t )nextLine[ i + 1 ] - ( int16_t )prevLine[ i + 1 ] );

				pose.screenJacobian( sj, point );
				*J = sj.transpose() * g;

				hess += ( *J ) * J->transpose();

				J++;
				p++;
			}
			prevLine = ptr;
			ptr = nextLine;
			nextLine += stride;
			point[ 1 ] += 1.0f;
		}

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
		bool lastGood = true;
		for( size_t i = 0; i < positions.size(); i++ ){
			x = positions[ i ].x;
			y = positions[ i ].y;

			if( x < ( int )phalf || ( x + phalf - 1 ) > w ||
			    y < ( int )phalf || ( y + phalf - 2 ) > h )
				continue;

			x -= phalf;
			y -= phalf;
			const uint8_t* p = ptr + y * s + x;

			if( lastGood )
				patches.push_back( new KLTPatch<pSize, PoseType>() );

			patches.back()->position() = positions[ i ];
			lastGood = patches.back()->update( p, s );
		}

		if( !lastGood ){
			delete patches.back();
			patches.pop_back();
		}

		img.unmap( ptr );
	}

}

#endif
