#include <cvt/gl/GLModel.h>

namespace cvt {

	GLModel::GLModel() : _vtxbuf( GL_ARRAY_BUFFER ), _normbuf( GL_ARRAY_BUFFER ), _facebuf( GL_ELEMENT_ARRAY_BUFFER ),
						 _numvertices( 0 ), _numfaces( 0 )
	{
	}

	GLModel::GLModel( const Model& mdl ) : _vtxbuf( GL_ARRAY_BUFFER ), _normbuf( GL_ARRAY_BUFFER ),
										   _facebuf( GL_ELEMENT_ARRAY_BUFFER ), _numvertices( 0 ), _numfaces( 0 )
	{
		_numvertices = mdl.vertexSize();
		_numfaces	 = mdl.trianglesSize();

		_vtxbuf.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * _numvertices );
		_normbuf.alloc( GL_STATIC_DRAW, sizeof( GLfloat ) * 3 * _numvertices );
		_facebuf.alloc( GL_STATIC_DRAW, sizeof( GLuint ) * 3 * _numfaces );

		Vector3f* vtxdata = ( Vector3f* ) _vtxbuf.map( GL_WRITE_ONLY );
		for( size_t i = 0; i < _numvertices; i++ )
			vtxdata[ i ] = mdl.vertex( i );
		_vtxbuf.unmap();

		Vector3f* normdata = ( Vector3f* ) _normbuf.map( GL_WRITE_ONLY );
		for( size_t i = 0; i < _numvertices; i++ )
			normdata[ i ] = mdl.normal( i );
		_normbuf.unmap();

		GLuint* data = ( GLuint* ) _facebuf.map( GL_WRITE_ONLY );
		for( size_t i = 0; i < _numfaces * 3; i++ )
			data[ i ] = mdl.index( i );
		_facebuf.unmap();

		_vao.setVertexData( _vtxbuf, 3, GL_FLOAT );
		_vao.setNormalData( _normbuf, 3, GL_FLOAT );
	}

	GLModel::~GLModel()
	{

	}


}
