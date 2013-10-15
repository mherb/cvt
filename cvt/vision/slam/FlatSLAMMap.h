/*
   The MIT License (MIT)

   Copyright (c) 2011 - 2013, Philipp Heise and Sebastian Klose

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#ifndef FLATSLAMMAP_H
#define FLATSLAMMAP_H

#include <cvt/vision/slam/SlamMap.h>

#include <cvt/math/Vector.h>
#include <cvt/util/EigenBridge.h>


namespace cvt
{

	class FlatSLAMMap
	{
		public:
			FlatSLAMMap( const SlamMap& map );
			~FlatSLAMMap( );

			const cvt::Matrix3f* intrinsics( ) const
			{
				return _intrinsics.data( );
			}

			const cvt::Vector4f* features( ) const
			{
				return _features.data( );
			}

			const cvt::Matrix4f* cameras( ) const
			{
				return _cameras.data( );
			}

			const Vector2f* measurements2D( ) const
			{
				return _measurements2D.data( );
			}

			const size_t* camIdx( ) const
			{
				return _camIdx.data( );
			}

			const size_t* featIdx( ) const
			{
				return _featIdx.data( );
			}

			size_t numMeasurements( ) const
			{
				return _measurementCounter;
			}

			size_t numFeatures( ) const
			{
				return _features.size();
			}

			size_t numCameras( ) const
			{
				return _cameras.size();
			}

			void setFeatureParam( size_t position, const cvt::Vector4f& featParam )
			{
				_features[ position ] = featParam;
			}

			void setCameraParam( size_t position, const cvt::Matrix4f& camParam )
			{
				_cameras[ position ] = camParam;
			}

		private:
			std::vector<cvt::Matrix3f> _intrinsics;
			std::vector<cvt::Vector4f> _features;
			std::vector<cvt::Matrix4f> _cameras;
			std::vector<cvt::Vector2f> _measurements2D;
			std::vector<size_t>        _camIdx;
			std::vector<size_t>        _featIdx;
			size_t _measurementCounter;
	};

}

#endif // FLATSLAMMAP_H
