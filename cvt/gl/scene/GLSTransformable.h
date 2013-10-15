#ifndef CVT_GLSTRANSFORMABLE_H
#define CVT_GLSTRANSFORMABLE_H

#include <cvt/math/Matrix.h>

namespace cvt {
	class GLSTransformable
	{
		public:
			GLSTransformable( const GLSTransformable& t );
			GLSTransformable( );
			GLSTransformable( const Matrix4f& mat );
			virtual ~GLSTransformable();

			const Matrix4f& transformation() const;
			Matrix4f&		transformation();
			void			setTransformation( const Matrix4f& t );

		protected:
			Matrix4f	_transformation;
	};

	inline GLSTransformable::GLSTransformable( const GLSTransformable& t ) : _transformation( t._transformation )
	{
	}

	inline GLSTransformable::GLSTransformable( )
	{
		_transformation.setIdentity();
	}

	inline GLSTransformable::GLSTransformable( const Matrix4f& mat ) : _transformation( mat )
	{
	}

	inline GLSTransformable::~GLSTransformable()
	{
	}

	inline void GLSTransformable::setTransformation( const Matrix4f& t )
	{
		_transformation = t;
	}

	inline const Matrix4f& GLSTransformable::transformation() const
	{
		return _transformation;
	}

	inline Matrix4f& GLSTransformable::transformation()
	{
		return _transformation;
	}
}

#endif
