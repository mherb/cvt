/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */

#include <cvt/geom/scene/SceneMesh.h>
#include <cvt/math/Vector.h>

namespace cvt {

	class MarchingCubes {
		public:
			MarchingCubes( const float* volume, size_t width, size_t height, size_t depth );
			~MarchingCubes();

			void triangulate( SceneMesh& mesh, float isolevel = 0.0f ) const;
			void triangulateWithNormals( SceneMesh& mesh, float isolevel = 0.0f ) const;

		private:
			static void vertexInterp( Vector3f& vtx, const Vector3f& p1, const Vector3f& p2, float val1, float val2, float isolevel );
			static void vertexNormalInterp( Vector3f& vtx, const Vector3f& p1, const Vector3f& p2, Vector3f& norm, const Vector3f& n1, const Vector3f& n2, float val1, float val2, float isolevel );

			const float* _volume;
			size_t		 _width;
			size_t		 _height;
			size_t		 _depth;
	};

	inline MarchingCubes::MarchingCubes( const float* volume, size_t width, size_t height, size_t depth ) :
		_volume( volume ),
		_width( width ),
		_height( height ),
		_depth( depth )
	{
	}

	inline MarchingCubes::~MarchingCubes()
	{
	}

}
