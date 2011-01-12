#ifndef CVT_PARAMSET_H
#define CVT_PARAMSET_H

#include <cvt/util/ParamInfo.h>
#include <cvt/util/Exception.h>
#include <iostream>

namespace cvt
{
	class ParamSet;
	std::ostream & operator<<( std::ostream & out, const ParamSet& pSet );

	class ParamSet
	{
		public:
			/*
			 * @param pInfos		array of ParamInfo
			 * @param n				size of array
			 * @param genOffsets	if true, the offsets will be generated and set during construction
			 */
			ParamSet( ParamInfo** pInfos, size_t n, bool genOffsets = true );

			~ParamSet();

			/* get handle to a parameter with the given name */
			size_t paramHandle( const std::string & name );

			/* set parameter */
			template <class T>
			void setArg( size_t handle, T value, size_t localIndex = 0 );

			template <class T>
			T arg( size_t handle, size_t localIndex = 0 );

			template <typename T>
			T * ptr();

		private:
			uint8_t *	_parameterMem;
			ParamInfo**	_pInfos;
			size_t		_numParameters;

			friend std::ostream& operator<< ( std::ostream &, const ParamSet& );
	};

	template<class T>
	inline void ParamSet::setArg( size_t handle, T value, size_t localIndex )
	{
		ParamInfo * pInfo = _pInfos[ handle ];
		
		if( localIndex > pInfo->count )
			throw CVTException( "Parameter \"" + pInfo->name + "\" local index out of bounds!" );

		if( !PTypeCheck<T>::check( pInfo->type ) ){
			throw CVTException( "Parameter \"" + pInfo->name + "\" types do not match!" );
		}
		
		// Type T is ensured here!
		TypedParamInfo<T>* pInfoT =  ( TypedParamInfo<T>* )pInfo;
		if( pInfo->rangeAndDefaultSet ){
			if( value < pInfoT->minValue() ){
				value = pInfoT->minValue();
			} else if( value > pInfoT->maxValue() ) {
				value = pInfoT->maxValue();
			}			
		}

		*( T* )( _parameterMem + pInfo->offset + localIndex * sizeof( T ) ) = value;
	}

	template <class T>
	inline T ParamSet::arg( size_t handle, size_t localIndex )
	{
		ParamInfo * pInfo = _pInfos[ handle ];

		if( localIndex > pInfo->count )
			throw CVTException( "Parameter \"" + pInfo->name + "\" local index out of bounds!" );

		if( !PTypeCheck<T>::check( pInfo->type ) ){
			throw CVTException( "Parameter \"" + pInfo->name + "\" types do not match!" );
		}

		return *( T* )( _parameterMem + pInfo->offset + localIndex * sizeof( T ) );
	}

	template <typename T>
	inline T * ParamSet::ptr()
	{
		return ( T* )_parameterMem;
	}

}

#endif
