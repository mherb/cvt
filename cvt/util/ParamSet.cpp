#include <cvt/util/ParamSet.h>

namespace cvt
{
	static size_t _PTYPE2SIZE[] =
	{
#define X( TYPE, PTYPE ) sizeof( TYPE ),
#include <cvt/util/internal/ParamTypes.def>
#undef X
	};

	ParamSet::ParamSet( ParamInfo * pInfos, size_t n, bool genOffsets ) :
		_parameterMem( 0 ),
		_pInfos( pInfos ),
		_numParameters( n )
	{
		size_t allSize = 0;

		ParamInfo * p;

		if( genOffsets ){
			p = pInfos;
			for( size_t i = 0; i < _numParameters; i++ ){
				p->offset = allSize;
				allSize += _PTYPE2SIZE[ p->type ] * p->count; ;

				p++;
			}
		} else {
			// offsets initialized:
			p = &pInfos[ n-1 ];
			allSize = p->offset + _PTYPE2SIZE[ p->type ] * p->count;
		}

		_parameterMem = new uint8_t[ allSize ];
	}

	ParamSet::~ParamSet()
	{
		if( _parameterMem )
			delete[] _parameterMem;
	}

	size_t ParamSet::paramHandle( const std::string & name )
	{
		ParamInfo * p = _pInfos;
		for( size_t i = 0; i < _numParameters; i++ ){
			if( name == p->name )
				return i;
			p++;
		}

		throw CVTException( "Parameter \"" + name + "\" not in parameterset" );
	}

	std::ostream& operator<<( std::ostream& out, const ParamSet& pSet )
	{
		ParamInfo * p = pSet._pInfos;
		for( size_t i = 0; i < pSet._numParameters; i++ ){
			out << "Name: " << p->name << " numElements: " << p->count << std::endl;
			p++;
		}
		return out;
	}
}
