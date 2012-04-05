/*
         CVT - Computer Vision Tools Library

    Copyright (c) 2012, Philipp Heise, Sebastian Klose

   THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 */
#include <cvt/vision/slam/stereo/FeatureTracking.h>
#include <cvt/vision/Vision.h>

namespace cvt
{



   bool FeatureTracking::checkFeatureSAD( const Vector2f& point0, const Vector2f& point1,
                                 const Image & i0, const Image & i1 ) const
   {
      size_t s0, s1;
      const uint8_t* ptr0 = i0.map( &s0 );
      const uint8_t* ptr1 = i1.map( &s1 );

      const uint8_t* p0 = ptr0 + ( (int)point0.y - 8 ) * s0 + ( (int)point0.x - 8 );
      const uint8_t* p1 = ptr1 + ( (int)point1.y - 8 ) * s1 + ( (int)point1.x - 8 );

      float sad = 0;
      for( size_t i = 0; i < 16; i++ ){
         sad += SIMD::instance()->SAD( p0, p1, 16 );
         p0 += s0;
         p1 += s1;
      }

      i0.unmap( ptr0 );
      i1.unmap( ptr1 );

      // average & normalize
      sad = 1.0f - ( sad / Math::sqr( 256.0 ) );

      if( sad > 0.7 ){
         //std::cout << sad << std::endl;
         return true;
      } else {
         return false;
      }
   }
}
