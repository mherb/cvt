#ifndef CVT_GLMESH_H
#define CVT_GLMESH_H


namespace cvt {
	class GLMesh {
		public:
			GLMesh();
			GLMesh( const Mesh& mesh );
			GLMesh( const Vector3f* vertices, const Vector3f* normals, const Vector2f* texcoords, size_t vsize,
				    const unsigned int* indices, size_t isize, MeshType type );
			~GLMesh();

			MeshType type() const;
			void draw() const;

		private:
			MeshType _type;

			GLVertexArray _vao;
			GLBuffer _vertices;
			GLBuffer _normals;
			GLBuffer _texcoords;
			GLBuffer _indices;
	};
}

#endif
