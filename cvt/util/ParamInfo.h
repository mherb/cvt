#ifndef CVT_PARAMINFO_H
#define CVT_PARAMINFO_H

#include <stdint.h>
#include <string>
#include <cvt/gfx/Image.h>
#include <cvt/math/Vector.h>
#include <cvt/math/Matrix.h>
#include <cvt/gfx/Color.h>

namespace cvt
{
	/* Special handled Type */
	struct Selection {
		Selection( size_t s ) : value( s ){}
		Selection( const Selection & s ) : value( s.value ){}
		bool operator<( const Selection & other ) const { return value < other.value; }
		bool operator>( const Selection & other ) const { return value > other.value; }
		size_t value;		
	};

#define X( TYPE, PTYPE ) PTYPE,
	enum ParamType
	{
		#include <cvt/util/internal/ParamTypes.def>
		PTYPE_SELECTION // param types that need special handling need manual editing
	};
#undef X

	template<class T> struct PTypeCheck;

	#define X( TYPE, PTYPE ) template<> struct PTypeCheck<TYPE>{ static const ParamType paramType = PTYPE; static bool check( ParamType other ) { if( other != paramType ) return false; return true; } };
		#include <cvt/util/internal/ParamTypes.def>
		X( Selection, PTYPE_SELECTION )
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
	
	template<class T> class ParamInfoTyped;

#define X( TYPE, PTYPE ) \
	template<> class ParamInfoTyped<TYPE> : public ParamInfo \
	{\
		public:\
			ParamInfoTyped( const std::string & n, bool input = true, size_t c = 1, size_t o = 0 ) :\
			ParamInfo( PTYPE, n, c, o, input ) {}\
			ParamInfoTyped( const std::string & n, TYPE min, TYPE max, TYPE defaultValue, bool input = true, size_t c = 1, size_t o = 0 ) :\
			ParamInfo( PTYPE, n, c, o, input, true ), min( min ), max( max ), defValue( defaultValue ) {}\
			virtual ~ParamInfoTyped() {}\
			TYPE minValue() const { return min; }\
			TYPE maxValue() const { return max; }\
			TYPE defaultValue() const { return defValue; }\
			virtual void setDefaultValue( void * ptr ) const { *( ( TYPE* )ptr ) = defValue; }\
			TYPE min; TYPE max; TYPE defValue;\
	};
	
	#include <cvt/util/internal/ParamTypes.def>
#undef X	
	template<> class ParamInfoTyped<Selection> : public ParamInfo
	{
		public:
		ParamInfoTyped( const std::string & n, size_t numVals, std::string* desc, bool input = true, size_t o = 0 ) :
				ParamInfo( PTYPE_SELECTION, n, 1, o, input, true ),
				min( 0 ), max( numVals - 1 ), defValue( 0 ), values( desc )
			{}
		
		virtual ~ParamInfoTyped() {}
		Selection minValue() const { return min; }
		Selection maxValue() const { return max; }
		Selection defaultValue() const { return defValue; }
		virtual void setDefaultValue( void * ptr ) const { *( ( Selection* )ptr ) = defValue; }
		Selection min; 
		Selection max; 
		Selection defValue;
		
		const std::string & description( const Selection & sel ){
			if( sel < min || sel > max )
				throw CVTException( "Selection out of bounds" );
			
			return values[ sel.value ];
		}
				
		private:
			std::string* values;
	};
	
}

#endif
