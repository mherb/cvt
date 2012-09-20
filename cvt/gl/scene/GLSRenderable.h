/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2011, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
*/
#ifndef CVT_GLSRENDERABLE_H
#define CVT_GLSRENDERABLE_H

#include <cvt/gl/scene/GLSTransformable.h>

namespace cvt {
	class GLSVisitor;

	class GLSRenderable : public GLSTransformable {
		public:
			virtual ~GLSRenderable() {}
			virtual void accept( GLSVisitor& visitor ) = 0;
	};
}

#endif
