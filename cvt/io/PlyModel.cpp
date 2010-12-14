#include <cvt/io/PlyModel.h>

#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

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


		std::cout << "\tProperty: " << p.type << " Name: " << p.name << std::endl;
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

		std::cout << "Element: " << strname << " " << size << std::endl;
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
		std::cout << "PLY VERSION: " << str << std::endl;

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
		pos = PlyReadHeader(base, end, elements, format );
		if( !PlyCheckElements( elements, normals ) ) {
			munmap( ( void* ) base, statbuf.st_size );
			return;
		}

		switch( format )
		{
			case PLY_ASCII: break;
			case PLY_BIN_LE: break;
			case PLY_BIN_BE: break;
		}

		munmap( ( void* ) base, statbuf.st_size );
	}
}
