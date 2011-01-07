#ifndef CVT_PARAMINFO_H
#define CVT_PARAMINFO_H

#include <stdint.h>
#include <string>

namespace cvt
{	
	enum ParamType 
	{
		PTYPE_UINT8 = 0,
		PTYPE_INT8,
		PTYPE_UINT16,
		PTYPE_INT16,
		PTYPE_UINT32,
		PTYPE_INT32,
		PTYPE_UINT64,
		PTYPE_INT64,		
		PTYPE_FLOAT,
		PTYPE_DOUBLE
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
			
			ParamType 	type;	
		
			/* descriptive name */
			std::string	name;
			
			/* offset in parameter set */
			size_t 		offset;
			
			/* count times this type */
			size_t	  	count;
	};
}

#endif
