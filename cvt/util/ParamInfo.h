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

	#define X( TYPE, PTYPE ) template<> struct PTypeCheck<TYPE>{ static const ParamType paramType = PTYPE; static bool check( ParamType other ) { if( other != paramType ) return false; return true; } };
	#include <cvt/util/internal/ParamTypes.def>
	#undef X
	
	template<ParamType p> struct PTypeSize;
	#define X( TYPE, PTYPE ) template<> struct PTypeSize<PTYPE>{ static const size_t SIZE = sizeof( TYPE ); };
	#include <cvt/util/internal/ParamTypes.def>
	#undef X
	
	// this generates a Union including all parameter types accessible by union._PTYPE_XXX
	#define X( TYPE, PTYPE ) TYPE _##PTYPE;
	union UnifiedType
	{
		#include <cvt/util/internal/ParamTypes.def>
	};
	#undef X	

	class ParamInfo
	{
		public:
			ParamInfo( ParamType t, 
					   const std::string & n, 
					   size_t c = 1, 
					   size_t o = 0, 
					   bool input = true ) :
				type( t ), name( n ), count( c ), offset( o ), input( input ), rangeAndDefaultSet( false )
			{}
					
			ParamInfo( ParamType t, 
					   const std::string & n,
					   UnifiedType min, 
					   UnifiedType max, 
					   UnifiedType defVal,
				       size_t c = 1, 
				       size_t o = 0, 
					   bool input = true ) :
				type( t ), name( n ), count( c ), offset( o ), 
				input( input ), rangeAndDefaultSet( true ), min( min ), max( max ), defaultValue( defVal )
			{}

			ParamType	type;			

			/* descriptive name */
			std::string	name;

			/* count times this type */
			size_t		count;

			/* offset in parameter set */
			size_t		offset;
		
			const bool input;
			const bool rangeAndDefaultSet;
		
			UnifiedType min;
			UnifiedType max;
			UnifiedType defaultValue;
	};
}

#endif
