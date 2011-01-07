#include <cvt/util/ParamSet.h>

namespace cvt
{
	static size_t _PTYPE2SIZE[] = 
	{
		sizeof( uint8_t  ),
		sizeof( int8_t  ),
		sizeof( uint16_t  ),
		sizeof( int16_t  ),
		sizeof( uint32_t  ),
		sizeof( int32_t  ),
		sizeof( uint64_t  ),
		sizeof( int64_t  ),
		sizeof( float  ),
		sizeof( double  )
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