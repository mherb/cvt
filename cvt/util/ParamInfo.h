#ifndef CVT_PARAMINFO_H
#define CVT_PARAMINFO_H

#include <stdint.h>
#include <string>
#include <cvt/gfx/Image.h>

namespace cvt
{
	enum ParamType
	{
#define X( TYPE, PTYPE ) PTYPE,
#include <cvt/util/internal/ParamTypes.def>
#undef X
	};

	template<class T> struct PTypeCheck;

	#define X(TYPE, PTYPE) template<> struct PTypeCheck<TYPE>{ static const ParamType paramType = PTYPE; static bool check( ParamType other ) { if( other != paramType ) return false; return true; } };
	#include <cvt/util/internal/ParamTypes.def>
	#undef X

	class ParamInfo
	{
		public:
			ParamInfo( ParamType t, const std::string & n, size_t c = 1, size_t o = 0 ) :
				type( t ),
				name( n ),
				count( c ),
				offset( o )
			{}

			ParamType	type;

			/* descriptive name */
			std::string	name;

			/* count times this type */
			size_t		count;

			/* offset in parameter set */
			size_t		offset;

	};
}

#endif
