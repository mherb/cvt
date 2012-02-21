/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#include <cvt/io/PlyModel.h>

#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

namespace cvt {
	enum PlyFormat { PLY_ASCII, PLY_BIN_LE, PLY_BIN_BE };
	enum PlyPropertyType { PLY_U8, PLY_U16, PLY_U32,
						   PLY_S8, PLY_S16, PLY_S32,
						   PLY_FLOAT, PLY_DOUBLE, PLY_LIST };

	struct PlyProperty {
		std::string name;
		PlyPropertyType type;
		PlyPropertyType lsizetype; /* type of list size */
		PlyPropertyType ltype; /* type of list elements */
	};

	struct PlyElement {
		std::string name;
		size_t size;
		std::vector<PlyProperty> properties;
		bool hasProperty( const std::string& name ) const;
	};


	bool PlyElement::hasProperty( const std::string& name ) const
	{
		for( std::vector<PlyProperty>::const_iterator it = properties.begin();
			 it != properties.end(); ++it )
			if( ( *it ).name == name )
				return true;
		return false;
	}

	static inline size_t PlyTypeSize( PlyPropertyType type )
	{
		switch( type ) {
			case PLY_U8:
			case PLY_S8: return 1;
			case PLY_U16:
			case PLY_S16: return 2;
			case PLY_U32:
			case PLY_S32:
			case PLY_FLOAT: return 4;
			case PLY_DOUBLE: return 8;
			default: return 0;
		}
	}

	static inline uint8_t* PlyEatWS( uint8_t* pos, uint8_t* end )
	{
		while( pos < end && ( *pos == ' ' || *pos == '\t' || *pos == '\n' || *pos == '\r' ) )
			pos++;
		return pos;
	}

	static inline uint8_t* PlyEatLine( uint8_t* pos, uint8_t* end )
	{
		while( pos < end && *pos != '\n' )
			pos++;
		return pos;
	}


	static inline uint8_t* PlyMatchWord( uint8_t* pos, uint8_t* end, const char* str, size_t n )
	{
		pos = PlyEatWS( pos, end );
		if( pos + n > end )
			return 0;
		if( strncmp( ( const char* ) pos, str, n ) )
		   return 0;
		return pos + n + 1;
	}

	static inline uint8_t* PlyNextWord( uint8_t* pos, uint8_t* end, std::string& str )
	{
		uint8_t* wspos;
		pos = PlyEatWS( pos, end );
		wspos = pos;
		while( wspos < end && ( *wspos != ' ' && *wspos != '\t' && *wspos != '\n' && *wspos != '\r' ) ) {
			wspos++;
		}
		if( pos != wspos )
			str.assign( ( const char* ) pos, wspos - pos );
		else
			return 0;

		return wspos;
	}

	static inline uint8_t* PlyDiscardWord( uint8_t* pos, uint8_t* end )
	{
		uint8_t* wspos;
		pos = PlyEatWS( pos, end );
		wspos = pos;
		while( wspos < end && ( *wspos != ' ' && *wspos != '\t' && *wspos != '\n' && *wspos != '\r' ) ) {
			wspos++;
		}
		if( pos == wspos )
			return 0;
		return wspos;
	}

	static inline uint8_t* PlyNextIntNumber( uint8_t* pos, uint8_t* end, ssize_t& num )
	{
		uint8_t* dpos;
		bool neg = false;

		pos = PlyEatWS( pos, end );
		if( pos + 1 < end && ( *pos == '+' || *pos == '-' ) ) {
			if( *pos++ == '-' )
				neg = true;
		}
		dpos = pos;

		num = 0;
		while( dpos < end && ( *dpos >= '0' && *dpos <= '9' ) ) {
			num = num * 10 + ( ( *dpos++ ) - '0' );
		}
		if( pos == dpos )
			return 0;
		if( neg )
			num = -num;
		return dpos;
	}

	static uint8_t* PlyReadProperty( uint8_t* pos, uint8_t* end, PlyProperty& p )
	{
		std::string strtype;
		std::string strname;

		if( !( pos = PlyNextWord( pos, end, strtype ) ) )
			return 0;

		/* type */
		if( strtype == "uchar" )
			p.type  = PLY_U8;
		else if( strtype == "ushort" )
			p.type  = PLY_U16;
		else if( strtype == "uint" )
			p.type  = PLY_U32;
		else if( strtype == "char" )
			p.type  = PLY_S8;
		else if( strtype == "short" )
			p.type  = PLY_S16;
		else if( strtype == "int" )
			p.type  = PLY_S32;
		else if( strtype == "float" )
			p.type  = PLY_FLOAT;
		else if( strtype == "double" )
			p.type  = PLY_DOUBLE;
		else if( strtype == "list" )
			p.type  = PLY_LIST;
		else
			return 0;


		if( p.type != PLY_LIST ) {
			if( !( pos = PlyNextWord( pos, end, p.name ) ) )
				return 0;
		} else {
			PlyPropertyType type;
			if( !( pos = PlyNextWord( pos, end, strtype ) ) )
				return 0;

			if( strtype == "uchar" )
				type  = PLY_U8;
			else if( strtype == "ushort" )
				type  = PLY_U16;
			else if( strtype == "uint" )
				type  = PLY_U32;
			else if( strtype == "char" )
				type  = PLY_S8;
			else if( strtype == "short" )
				type  = PLY_S16;
			else if( strtype == "int" )
				type  = PLY_S32;
			else
				return 0;

			p.lsizetype = type;

			if( !( pos = PlyNextWord( pos, end, strtype ) ) )
				return 0;

			if( strtype == "uchar" )
				type  = PLY_U8;
			else if( strtype == "ushort" )
				type  = PLY_U16;
			else if( strtype == "uint" )
				type  = PLY_U32;
			else if( strtype == "char" )
				type  = PLY_S8;
			else if( strtype == "short" )
				type  = PLY_S16;
			else if( strtype == "int" )
				type  = PLY_S32;
			else if( strtype == "float" )
				type  = PLY_FLOAT;
			else if( strtype == "double" )
				type  = PLY_DOUBLE;
			else
				return 0;

			p.ltype = type;

			if( !( pos = PlyNextWord( pos, end, p.name ) ) )
				return 0;
		}


//		std::cout << "\tProperty: " << p.type << " Name: " << p.name << std::endl;
		return pos;
	}

	static uint8_t* PlyReadElement( uint8_t* pos, uint8_t* end, PlyElement& e )
	{
		uint8_t* pold;
		std::string strname, str;
		ssize_t size;

		if( !( pos = PlyNextWord( pos, end, strname ) ) )
			return 0;
		if( !( pos = PlyNextIntNumber( pos, end, size ) ) )
			return 0;

		if( size < 0 )
			return 0;

		e.name = strname;
		e.size = ( size_t ) size;

//		std::cout << "Element: " << strname << " " << size << std::endl;
		while( 1 ) {

			pold = pos;
			if( !( pos = PlyNextWord( pos, end, str ) ) )
				return 0;

			if( str == "comment"  ) {
				if( !( pos = PlyEatLine( pos, end ) ) )
					return 0;
			} else if( str == "property"  ) {
				e.properties.resize( e.properties.size() + 1 );
				if( !( pos = PlyReadProperty( pos, end, e.properties[ e.properties.size() - 1 ] ) ) )
					return 0;
			} else
				return pold;
		}
	}

	static bool PlyCheckElements( std::vector<PlyElement>& elements, bool& normals )
	{
		bool vertex, face;

		vertex = face = normals = false;

		for( std::vector<PlyElement>::iterator it = elements.begin(); it != elements.end(); ++it ) {
			if( ( *it ).name == "vertex" ) {
				if( ( *it ).hasProperty( "x" )
				   && ( *it ).hasProperty( "y" )
				   && ( *it ).hasProperty( "z" ) )
					vertex = true;
				if( ( *it ).hasProperty( "nx" )
				   && ( *it ).hasProperty( "ny" )
				   && ( *it ).hasProperty( "nz" ) )
					normals = true;
			} else if( ( *it ).name == "face" ) {
				if( ( *it ).hasProperty( "vertex_indices" ) )
					face = true;
			}
		}
		return vertex && face;
	}

	static uint8_t* PlyReadHeader( uint8_t* pos, uint8_t* end, std::vector<PlyElement>& elements, PlyFormat& format )
	{
		std::string str;

		if( !( pos = PlyMatchWord( pos, end, "ply", 3 ) ) )
			return 0;
		if( !( pos = PlyMatchWord( pos, end, "format", 6 ) ) )
			return 0 ;
		if( !( pos = PlyNextWord( pos, end, str ) ) )
			return 0;
		/* which format */
		if( str == "ascii" )
			format = PLY_ASCII;
		else if( str == "binary_big_endian" )
			format = PLY_BIN_BE;
		else if( str == "binary_little_endian" )
			format = PLY_BIN_LE;
		else
			return 0;

		if( !( pos = PlyNextWord( pos, end, str ) ) )
			return 0;
//		std::cout << "PLY VERSION: " << str << std::endl;

		while( 1 ) {
			if( !( pos = PlyNextWord( pos, end, str ) ) )
				return 0;
			if( str == "comment" ) {
				if( !( pos = PlyEatLine( pos, end ) ) )
					return 0;
			} else if( str == "element" ) {
				elements.resize( elements.size() + 1 );
				if( !( pos = PlyReadElement( pos, end, elements[ elements.size() - 1 ] ) ) )
					return 0;
			} else if( str == "end_header" )
				break;
		}
		pos = PlyEatWS( pos, end );

		return pos;
	}

	static uint8_t* PlyDiscardPropertyAscii( uint8_t* pos, uint8_t* end, const PlyProperty& p )
	{
		if( p.type != PLY_LIST ) {
			if( !( pos = PlyDiscardWord( pos, end ) ) )
				return 0;
		} else {
			/* discard list elements */
			ssize_t nlist;
			if( !( pos = PlyNextIntNumber( pos, end, nlist ) ) )
				return 0;
			if( nlist < 0 )
				return 0;
			while( nlist-- ) {
				if( !( pos = PlyDiscardWord( pos, end ) ) )
					return 0;
			}
		}
		return pos;
	}

	static uint8_t* PlyDiscardElementAscii( uint8_t* pos, uint8_t* end, const PlyElement& e )
	{
		bool fastdiscard = true;

		for( std::vector<PlyProperty>::const_iterator it = e.properties.begin();
			it != e.properties.end(); ++it ) {
			if( ( *it ).type == PLY_LIST ) {
				fastdiscard = false;
				break;
			}
		}

		if( fastdiscard ) {
			size_t n = e.properties.size() * e.size;
			while( n-- ) {
				if( !( pos = PlyDiscardWord( pos, end ) ) )
					return 0;
			}
		} else {
			size_t n = e.size;
			while( n-- ) {
				for( std::vector<PlyProperty>::const_iterator it = e.properties.begin();
					it != e.properties.end(); ++it )
					if( !( pos = PlyDiscardPropertyAscii( pos, end, *it ) ) )
						return 0;
			}
		}
		return pos;
	}

	static uint8_t* PlyDiscardElementBinary( uint8_t* pos, uint8_t* end, const PlyElement& e, bool swap )
	{
		bool fastdiscard = true;
		size_t bytes = 0;

		for( std::vector<PlyProperty>::const_iterator it = e.properties.begin();
			it != e.properties.end(); ++it ) {
			if( ( *it ).type == PLY_LIST ) {
				fastdiscard = false;
				break;
			}
			bytes += PlyTypeSize( ( *it ).type );
		}

		if( fastdiscard ) {
			pos += bytes * e.size;
			if( pos >= end )
				return 0;
			return pos;
		} else {
			size_t n = e.size;
			while( n-- ) {
				for( std::vector<PlyProperty>::const_iterator it = e.properties.begin();
					it != e.properties.end(); ++it ) {
					if( ( *it ).type != PLY_LIST ) {
						pos += PlyTypeSize( ( *it ).type );
						if( pos >= end )
							return 0;
					} else {
						/* discard list elements */
						ssize_t nlist;
						// FIXME
						pos += nlist * PlyTypeSize( ( *it ).ltype );
						if( pos >= end )
							return 0;
					}
				}
			}
		}
		return pos;
	}

	static uint8_t* PlyReadVertexAscii( uint8_t* pos, uint8_t* end, const PlyElement& e, bool normals, Model& mdl )
	{
		size_t n = e.size;
		std::string str;
		double x , y, z, nx, ny, nz;

		x = y = z = nx = ny = nz = 0;

		while( n-- ) {
			for( std::vector<PlyProperty>::const_iterator it = e.properties.begin();
				it != e.properties.end(); ++it ) {
				if( it->name == "x" ) {
					if( !( pos = PlyNextWord( pos, end, str ) ) )
						return 0;
					x = strtod( str.c_str(), NULL );
				} else if( it->name == "y" ) {
					if( !( pos = PlyNextWord( pos, end, str ) ) )
						return 0;
					y = strtod( str.c_str(), NULL );
				} else if( it->name == "z" ) {
					if( !( pos = PlyNextWord( pos, end, str ) ) )
						return 0;
					z = strtod( str.c_str(), NULL );
				} else	if( it->name == "nx" ) {
					if( !( pos = PlyNextWord( pos, end, str ) ) )
						return 0;
					nx = strtod( str.c_str(), NULL );
				} else if( it->name == "ny" ) {
					if( !( pos = PlyNextWord( pos, end, str ) ) )
						return 0;
					ny = strtod( str.c_str(), NULL );
				} else if( it->name == "nz" ) {
					if( !( pos = PlyNextWord( pos, end, str ) ) )
						return 0;
					nz = strtod( str.c_str(), NULL );
				} else {
					if( !( pos = PlyDiscardPropertyAscii( pos, end, *it ) ) )
						return 0;
				}
			}
//			std::cout << x << " , " << y << " , " << z << std::endl;
			mdl.addVertex( Vector3f( x, y, z ) );
			if( normals )
				mdl.addNormal( Vector3f( nx, ny, nz ) );
		}

		return pos;
	}

	static uint8_t* PlyReadFacesAscii( uint8_t* pos, uint8_t* end, const PlyElement& e, Model& mdl )
	{
		size_t n = e.size;
		ssize_t nindeces, index;
		std::vector<size_t> indices;

		while( n-- ) {
			indices.clear();
			for( std::vector<PlyProperty>::const_iterator it = e.properties.begin();
				it != e.properties.end(); ++it ) {
				if( it->name == "vertex_indices" ) {
					if( !( pos = PlyNextIntNumber( pos, end, nindeces ) ) )
						return 0;
					if( nindeces < 0 )
						return 0;
					while( nindeces-- ) {
						if( !( pos = PlyNextIntNumber( pos, end, index ) ) )
							return 0;
						if( index < 0 )
							return 0;
						indices.push_back( ( size_t ) index );
					}
				} else {
					if( !( pos = PlyDiscardPropertyAscii( pos, end, *it ) ) )
						return 0;
				}
			}
			if( indices.size() == 3 ) {
				mdl.addTriangle( indices[ 0 ], indices[ 1 ], indices[ 2  ] );
			} else {
				//			mdl.addFace( indices );
			}
		}
		return pos;
	}

	void PlyModel::load( Model& mdl, const char* file )
	{
		int fd;
		uint8_t* base;
		uint8_t* pos;
		uint8_t* end;
		struct stat statbuf;
		PlyFormat format;
		std::vector<PlyElement> elements;
		bool normals = false;

		mdl.clear();

		fd = open( file, O_RDONLY, 0 );
		if( fd < 0 )
			return;

		if( fstat( fd, &statbuf ) < 0 ) {
			close( fd );
			return;
		}

		base = ( uint8_t* ) mmap( NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0 );
		close( fd );
		if( ( ( void* ) base ) == MAP_FAILED ) {
			return;
		}

		end = base + statbuf.st_size;
		if( !( pos = PlyReadHeader(base, end, elements, format ) ) ) {
			munmap( ( void* ) base, statbuf.st_size );
			return;
		}

		if( !PlyCheckElements( elements, normals ) ) {
			munmap( ( void* ) base, statbuf.st_size );
			return;
		}

		switch( format )
		{
			case PLY_ASCII:
				{
					for( std::vector<PlyElement>::iterator it = elements.begin();
						 it != elements.end(); ++it ) {
						if( it->name == "vertex" ) {
							if( !( pos = PlyReadVertexAscii( pos, end, *it, normals, mdl ) ) )
								return;
						} else if( it->name == "face" ) {
							if( !( pos = PlyReadFacesAscii( pos, end, *it, mdl ) ) )
								return;
						} else {
							if( !( pos = PlyDiscardElementAscii( pos, end, *it ) ) )
								return;
						}
					}
				}
				break;
			case PLY_BIN_LE:
			case PLY_BIN_BE:
				{
					std::cout << "currently not supported" << std::endl;
				}
				break;
		}

		munmap( ( void* ) base, statbuf.st_size );
	}


	void PlyModel::save( const char* file, const Model& mdl )
	{
		FILE* f;
		Vector3f v, n;

		f = fopen( file, "w+" );
		if( f == NULL )
			return;
		fprintf( f, "ply\nformat ascii 1.0\ncomment CVT Ply\nelement vertex %zd\n" \
				"property float x\nproperty float y\nproperty float z\n", mdl.vertexSize() );
		if( mdl.normalsSize() == mdl.vertexSize() )
			fprintf( f, "property float nx\nproperty float ny\nproperty float nz\n" );
		fprintf( f, "element face %zd\nproperty list uchar uint vertex_indices\nend_header\n", mdl.trianglesSize() );

		if( mdl.normalsSize() == mdl.vertexSize() ) {
			for( size_t i = 0; i < mdl.vertexSize(); i++ ) {
				v = mdl.vertex( i );
				n = mdl.normal( i );
				fprintf( f, "%f %f %f %f %f %f\n", v[ 0 ], v[ 1 ], v[ 2 ], n[ 0 ], n[ 1 ], n[ 2 ] );
			}
		} else {
			for( size_t i = 0; i < mdl.vertexSize(); i++ ) {
				v = mdl.vertex( i );
				fprintf( f, "%f %f %f\n", v[ 0 ], v[ 1 ], v[ 2 ] );
			}
		}
		for( size_t i = 0; i < mdl.trianglesSize(); i++ ) {
			fprintf( f, "3 %zd %zd %zd\n", mdl.index( i * 3 + 0 ), mdl.index( i * 3 + 1 ), mdl.index( i * 3 + 2 ) );
		}
		fclose( f );
	}
}
