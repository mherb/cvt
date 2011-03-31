#include <cvt/util/ParamSet.h>

namespace cvt
{
	#define X( TYPE, PTYPE ) sizeof( TYPE ),
	static size_t _PTYPE2SIZE[] =
	{
		#include <cvt/util/internal/ParamTypes.def>
		X( Selection, PTYPE_SELECTION )
	};
	#undef X
	
	#define X( TYPE, PTYPE ) #PTYPE,
	static std::string _PTYPENAMES[] = 
	{
		#include <cvt/util/internal/ParamTypes.def>
		X( Selection, PTYPE_SELECTION )
	};

	ParamSet::ParamSet( ParamInfo** pInfos, size_t n, bool genOffsets ) :
		_parameterMem( 0 ),
		_pInfos( pInfos ),
		_numParameters( n )
	{
		size_t allSize = 0;

		ParamInfo * p;

		if( genOffsets ){			
			for( size_t i = 0; i < _numParameters; i++ ){
				p = pInfos[ i ];
				p->offset = allSize;
				allSize += _PTYPE2SIZE[ p->type ] * p->count;				
			}
		} else {
			// offsets initialized:
			p = pInfos[ n-1 ];
			allSize = p->offset + _PTYPE2SIZE[ p->type ] * p->count;
		}

		_parameterMem = new uint8_t[ allSize ];		
		
		for( size_t i = 0; i < _numParameters; i++ ){
			p = _pInfos[ i ];
			if( p->hasDefault )
				p->setDefaultValue( _parameterMem + p->offset );
		}
	}

	ParamSet::~ParamSet()
	{
		if( _parameterMem )
			delete[] _parameterMem;
	}

	size_t ParamSet::paramHandle( const std::string & name )
	{
		ParamInfo * p;
		for( size_t i = 0; i < _numParameters; i++ ){
			p = _pInfos[ i ];
			if( name == p->name )
				return i;			
		}

		throw CVTException( "Parameter \"" + name + "\" not in parameterset" );
	}

	std::ostream& operator<<( std::ostream& out, const ParamSet& pSet )
	{
		ParamInfo * p; 
		for( size_t i = 0; i < pSet._numParameters; i++ ){
			p = pSet._pInfos[ i ];
			out << "Name: " << p->name << " Type: " << _PTYPENAMES[ p->type ] << " numElements: " << p->count;
			if( p->hasRange ){
				out << " Range available"; 
			}
			if( p->hasDefault ){
				out << " Default value available"; 
			}

			out << std::endl;
		}
		return out;
	}
}
