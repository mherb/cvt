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
	};

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
		if( strtype.compare("uchar") == 0 )
			p.type  = PLY_U8;
		else if( strtype.compare("ushort") == 0 )
			p.type  = PLY_U16;
		else if( strtype.compare("uint") == 0 )
			p.type  = PLY_U32;
		else if( strtype.compare("char") == 0 )
			p.type  = PLY_S8;
		else if( strtype.compare("short") == 0 )
			p.type  = PLY_S16;
		else if( strtype.compare("int") == 0 )
			p.type  = PLY_S32;
		else if( strtype.compare("float") == 0 )
			p.type  = PLY_FLOAT;
		else if( strtype.compare("double") == 0 )
			p.type  = PLY_DOUBLE;
		else if( strtype.compare("list") == 0 )
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

			if( strtype.compare("uchar") == 0 )
				type  = PLY_U8;
			else if( strtype.compare("ushort") == 0 )
				type  = PLY_U16;
			else if( strtype.compare("uint") == 0 )
				type  = PLY_U32;
			else if( strtype.compare("char") == 0 )
				type  = PLY_S8;
			else if( strtype.compare("short") == 0 )
				type  = PLY_S16;
			else if( strtype.compare("int") == 0 )
				type  = PLY_S32;
			else
				return 0;

			p.lsizetype = type;

			if( !( pos = PlyNextWord( pos, end, strtype ) ) )
				return 0;

			if( strtype.compare("uchar") == 0 )
				type  = PLY_U8;
			else if( strtype.compare("ushort") == 0 )
				type  = PLY_U16;
			else if( strtype.compare("uint") == 0 )
				type  = PLY_U32;
			else if( strtype.compare("char") == 0 )
				type  = PLY_S8;
			else if( strtype.compare("short") == 0 )
				type  = PLY_S16;
			else if( strtype.compare("int") == 0 )
				type  = PLY_S32;
			else if( strtype.compare("float") == 0 )
				type  = PLY_FLOAT;
			else if( strtype.compare("double") == 0 )
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

			if( str.compare( "comment" ) == 0 ) {
				if( !( pos = PlyEatLine( pos, end ) ) )
					return 0;
			} else if( str.compare( "property" ) == 0 ) {
				e.properties.resize( e.properties.size() + 1 );
				if( !( pos = PlyReadProperty( pos, end, e.properties[ e.properties.size() - 1 ] ) ) )
					return 0;
			} else
				return pold;
		}
	}

	static uint8_t* PlyReadHeader( uint8_t* base, size_t size, std::vector<PlyElement>& elements, PlyFormat& format )
	{
		uint8_t* pos = base;
		uint8_t* end = base + size;
		std::string str;

		if( !( pos = PlyMatchWord( pos, end, "ply", 3 ) ) )
			return 0;
		if( !( pos = PlyMatchWord( pos, end, "format", 6 ) ) )
			return 0 ;
		if( !( pos = PlyNextWord( pos, end, str ) ) )
			return 0;
		/* which format */
		if( str.compare( "ascii" ) == 0 )
			format = PLY_ASCII;
		else if( str.compare("binary_big_endian") == 0 )
			format = PLY_BIN_BE;
		else if( str.compare("binary_little_endian") == 0 )
			format = PLY_BIN_LE;
		else
			return 0;

		if( !( pos = PlyNextWord( pos, end, str ) ) )
			return 0;
		std::cout << "PLY VERSION: " << str << std::endl;

		while( 1 ) {
			if( !( pos = PlyNextWord( pos, end, str ) ) )
				return 0;
			if( str.compare( "comment" ) == 0 ) {
				if( !( pos = PlyEatLine( pos, end ) ) )
					return 0;
			} else if( str.compare( "element" ) == 0 ) {
				elements.resize( elements.size() + 1 );
				if( !( pos = PlyReadElement( pos, end, elements[ elements.size() - 1 ] ) ) )
					return 0;
			} else if( str.compare( "end_header" ) == 0 )
				break;
		}
		pos = PlyEatWS( pos, end );

		return pos;
	}

	void PlyModel::load( Model& mdl, const char* file )
	{
		int fd;
		uint8_t* base;
		uint8_t* data;
		struct stat statbuf;
		PlyFormat format;
		std::vector<PlyElement> elements;

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

		data = PlyReadHeader(base, statbuf.st_size, elements, format );

		switch( format )
		{
			case PLY_ASCII: break;
			case PLY_BIN_LE: break;
			case PLY_BIN_BE: break;
		}

		munmap( ( void* ) base, statbuf.st_size );
	}
}
