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
	
	class ParamInfo
	{		
		public:
			virtual ~ParamInfo(){}
		
			ParamType	type;			

			/* descriptive name */
			std::string	name;

			/* count times this type */
			size_t		count;

			/* offset in parameter set */
			size_t		offset;
		
			const bool input;
			const bool rangeAndDefaultSet;
		
			virtual void setDefaultValue( void * ) const = 0;
		
		protected:
			ParamInfo( ParamType t, const std::string & n, size_t c, 
					   size_t o, bool input, bool rangeType = false ) :
				type( t ), name( n ), count( c ), offset( o ), 
				input( input ), rangeAndDefaultSet( rangeType )
			{}
	};
	
	template<class T> 
	class TypedParamInfo : public ParamInfo	
	{
		public:
			TypedParamInfo( const std::string & n, size_t c = 1, size_t o = 0, bool input = true );
		
			TypedParamInfo( const std::string & n, T min, T max, T defaultValue, size_t c = 1, size_t o = 0, bool input = true );
		
			virtual ~TypedParamInfo() {}
		
			T minValue() const { return min; }
			T maxValue() const { return max; }
			T defaultValue() const { return defValue; }			
			virtual void setDefaultValue( void * ptr ) const { *( ( T* )ptr ) = defValue; }
		
			T min;
			T max;
			T defValue;
	};
	
#define X( TYPE, PTYPE ) \
	template<> inline TypedParamInfo<TYPE>::TypedParamInfo( const std::string & n, size_t c, size_t o, bool input ) : \
		ParamInfo( PTYPE, n, c, o, input ) {} \
	template<> inline TypedParamInfo<TYPE>::TypedParamInfo( const std::string & n, TYPE min, TYPE max, TYPE defaultValue, size_t c, size_t o, bool input ) : \
		ParamInfo( PTYPE, n, c, o, input, true ), min( min ), max( max ), defValue( defaultValue ) {}		
	#include <cvt/util/internal/ParamTypes.def>
#undef X	

	
}

#endif
