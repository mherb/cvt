#include "ObjLoader.h"

#include <cvt/io/FileSystem.h>
#include <cvt/util/DataIterator.h>

namespace cvt {

	String ObjLoader::_name = "OBJ";
	String ObjLoader::_extensions[ 2 ] = { ".obj", ".OBJ" };

	struct ObjFace {
		ObjFace( unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4,
				 unsigned int vn1, unsigned int vn2, unsigned int vn3, unsigned int vn4,
				 unsigned int vt1, unsigned int vt2, unsigned int vt3, unsigned int vt4 )
		{
			vidx[ 0 ] = v1;
			vidx[ 1 ] = v2;
			vidx[ 2 ] = v3;
			vidx[ 3 ] = v4;

			vtidx[ 0 ] = vt1;
			vtidx[ 1 ] = vt2;
			vtidx[ 2 ] = vt3;
			vtidx[ 3 ] = vt4;

			vnidx[ 0 ] = vn1;
			vnidx[ 1 ] = vn2;
			vnidx[ 2 ] = vn3;
			vnidx[ 3 ] = vn4;
		}

		bool isTriangle() const
		{
			return vidx[ 3 ] == 0;
		}

		bool hasNormals() const
		{
			return vnidx[ 0 ] != 0 && vnidx[ 1 ] != 0 && vnidx[ 2 ] != 0 && ( isTriangle() || vnidx[ 3 ] != 0 );
		}

		bool hasTexcoords() const
		{
			return vtidx[ 0 ] != 0 && vtidx[ 1 ] != 0 && vtidx[ 2 ] != 0 && ( isTriangle() || vtidx[ 3 ] != 0 );
		}

		unsigned int vidx[ 4 ];
		unsigned int vtidx[ 4 ];
		unsigned int vnidx[ 4 ];
	};

	static bool ObjFacesToMesh( SceneMesh& mesh, std::vector<ObjFace>& faces,
						 const std::vector<Vector3f>& vertices,
						 const std::vector<Vector3f>& normals,
						 const std::vector<Vector2f>& texcoords )
	{
		std::vector<Vector3f> mvertices;
		std::vector<Vector3f> mnormals;
		std::vector<Vector2f> mtexcoords;
		std::vector<unsigned int> mfaces;
		unsigned int idx = 0;
		bool hasTex = true, hasNormal = true;

		for( std::vector<ObjFace>::iterator it = faces.begin(); it != faces.end() && ( hasTex || hasNormal ); ++it ) {
			if( !it->hasTexcoords() ) hasTex = false;
			if( !it->hasNormals() ) hasNormal = false;
		}

		for( std::vector<ObjFace>::iterator it = faces.begin(); it != faces.end(); ++it ) {
			if( !it->isTriangle() ) {
				unsigned int table[] = { 0, 1, 2, 2, 3, 0 };
				// Convert quad to triangle
				for( int i = 0; i < 6; i++ ) {
					unsigned int lidx;
					unsigned int ti = table[ i ];
					lidx = it->vidx[ ti ];
					if( lidx && lidx <= vertices.size() ) {
						mvertices.push_back( vertices[ lidx ] );
						// texcoords
						if( hasTex ) {
							lidx = it->vtidx[ ti ];
							if( lidx && lidx <= texcoords.size() ) {
								mtexcoords.push_back( texcoords[ lidx ] );
							} else
								return false;
						}
						// normals
						if( hasNormal ) {
							lidx = it->vnidx[ ti ];
							if( lidx && lidx <= normals.size() ) {
								mnormals.push_back( normals[ lidx ] );
							} else
								return false;
						}

						mfaces.push_back( idx++ );
					} else
						return false;
				}
			} else {
				for( int i = 0; i < 3; i++ ) {
					unsigned int lidx;
					lidx = it->vidx[ i ];
					if( lidx && lidx <= vertices.size() ) {
						mvertices.push_back( vertices[ lidx ] );
						// texcoords
						if( hasTex ) {
							lidx = it->vtidx[ i ];
							if( lidx && lidx <= texcoords.size() ) {
								mtexcoords.push_back( texcoords[ lidx ] );
							} else
								return false;
						}
						// normals
						if( hasNormal ) {
							lidx = it->vnidx[ i ];
							if( lidx && lidx <= normals.size() ) {
								mnormals.push_back( normals[ lidx ] );
							} else
								return false;
						}

						mfaces.push_back( idx++ );
					} else
						return false;
				}
			}
		}
		mesh.setVertices( &mvertices[ 0 ], mvertices.size() );
		mesh.setFaces( &mfaces[ 0 ], mfaces.size(), SCENEMESH_TRIANGLES );
		if( hasTex )
			mesh.setTexcoords( &mtexcoords[ 0 ], mtexcoords.size() );
		if( hasNormal )
			mesh.setNormals( &mnormals[ 0 ], mnormals.size() );

		mesh.removeRedundancy();

		return true;
	}

	static void ObjReadVertices( DataIterator& d, std::vector<Vector3f>& vertices )
	{
		String ws( " \r\n\t" );
		String token;
		Vector3f v;

		while( d.peekNextToken( token, ws ) ) {
			if( token != "v" )
				return;

			d.nextToken( token, ws );
			v.x = d.nextDouble();
			v.y = d.nextDouble();
			v.z = d.nextDouble();
			vertices.push_back( v );
		}
	}

	static void ObjReadTexcoords( DataIterator& d, std::vector<Vector2f>& texcoords )
	{
		String ws( " \r\n\t" );
		String token;
		Vector2f t;

		while( d.peekNextToken( token, ws ) ) {
			if( token != "vt" )
				return;

			d.nextToken( token, ws );
			t.x = d.nextDouble();
			t.y = d.nextDouble();
			d.skipInverse("\n");
			d.skip( ws );
			texcoords.push_back( t );
		}
	}

	static void ObjReadNormals( DataIterator& d, std::vector<Vector3f>& normals )
	{
		String ws( " \r\n\t" );
		String token;
		Vector3f n;

		while( d.peekNextToken( token, ws ) ) {
			if( token != "vn" )
				return;

			d.nextToken( token, ws );
			n.x = d.nextDouble();
			n.y = d.nextDouble();
			n.z = d.nextDouble();
			normals.push_back( n );
		}
	}

	static inline bool ObjReadFaceEntry( DataIterator& d, unsigned int& v, unsigned int& vt, unsigned int& vn )
	{
		d.skip( " \r\t" );
		// empty entry
		if( *d.pos() == '\n' ) {
			v = vt = vn = 0;
			return true;
		}

		v = d.nextLong();
		if( *d.pos() != ' ' && *d.pos() != '\n' && *d.pos() != '\r' && *d.pos() != '\t' ) {
				// must be followed by vt and or vn
				// parse vt
				if( *d.pos() != '/' )
					return false;
				d.skip( 1 );
				// check if empty
				if( *d.pos() == '/' ) {
					vt = 0;
				} else
					vt = d.nextLong();

				// parse vn
				if( *d.pos() == ' ' ) // check if empty
					vn = 0;
				else if( *d.pos() == '/' ) {
					d.skip( 1 );
					if( *d.pos() == ' ' ) // check if empty
						vn = 0;
					else
						vn = d.nextLong();
				} else // error
					return false;
		} else {
			vt = 0;
			vn = 0;
		}

		return true;
	}

	static bool ObjReadFaces( DataIterator& d, std::vector<ObjFace>& faces )
	{
		String ws( " \r\n\t" );
		String token;
		unsigned int v1, v2, v3, v4;
		unsigned int vn1, vn2, vn3, vn4;
		unsigned int vt1, vt2, vt3, vt4;

		while( d.peekNextToken( token, ws ) ) {
			if( token != "f" )
				return true;

			d.nextToken( token, ws );

			// FIXME what about higher dimensional polygons ?
			if( !ObjReadFaceEntry( d, v1, vt1, vn1 ) )
				return false;
			if( !ObjReadFaceEntry( d, v2, vt2, vn2 ) )
				return false;
			if( !ObjReadFaceEntry( d, v3, vt3, vn3 ) )
				return false;
			if( !ObjReadFaceEntry( d, v4, vt4, vn4 ))
				return false;
			d.skip( " \r\t" );
			if( *d.pos() != '\n' )
				return false;

			faces.push_back( ObjFace( v1, v2, v3, v4,
									 vn1, vn2, vn3, vn4,
									 vt1, vt2, vt3, vt4 ) );
		}

		return true;
	}


	void ObjLoader::load( Scene& scene, const String& filename )
	{
		String ws( " \r\n\t" );
		String token;

		Data data;
		FileSystem::load( data, filename );
		SceneMesh* cur = new SceneMesh( "_NONAME_" );
		std::vector<Vector3f> vertices;
		std::vector<Vector3f> normals;
		std::vector<Vector2f> texcoords;
		std::vector<ObjFace> faces;

		DataIterator d( data );
		while( d.peekNextToken( token, ws ) ) {
			if( token == "g" ) { // group
				if( faces.size() ) {
					ObjFacesToMesh( *cur, faces, vertices, normals, texcoords );
					if( !cur->isEmpty() )
						scene.addGeometry( cur );
					faces.clear();
				} else
					delete cur;

				d.nextToken( token, ws );
				if( ! d.nextToken( token, ws ) ) {
					return;
				}
				cur = new SceneMesh( token );
			} else if( token == "o" ) { // object
				// discard
				d.skipInverse( "\n" );
				d.skip( ws );
			} else if( token == "mtllib" ) { // material library
				// discard
				d.skipInverse( "\n" );
				d.skip( ws );
			} else if( token == "usemtl" ) { // reference material
				// discard
				d.skipInverse( "\n" );
				d.skip( ws );
			} else if( token == "v" ) { // vertices
				ObjReadVertices( d, vertices );
			} else if( token == "vn" ) { // normals
				ObjReadNormals( d, normals );
			} else if( token == "vt" ) { // texcoords
				ObjReadTexcoords( d, texcoords );
			} else if( token == "f" ) { // faces
				if( !ObjReadFaces( d, faces ) ) {
					scene.clear();
					return;
				}
			} else if( token == "s" ) { // smooth group
				// discard
				d.skipInverse( "\n" );
				d.skip( ws );
			} else {
				// discard line, FIXME
				d.skipInverse( "\n" );
				d.skip( ws );
			}
		}

		std::cout << "Vertices: " << vertices.size() << std::endl;
		std::cout << "Normals: " << normals.size() << std::endl;
		std::cout << "Texcoords: " << texcoords.size() << std::endl;

		if( faces.size() ) {
			ObjFacesToMesh( *cur, faces, vertices, normals, texcoords );
			if( !cur->isEmpty() )
				scene.addGeometry( cur );
		} else
			delete cur;

	}


}

static void _init( cvt::PluginManager* pm )
{
	cvt::SceneLoader* obj = new cvt::ObjLoader();
	pm->registerPlugin( obj );
}

CVT_PLUGIN( _init )
