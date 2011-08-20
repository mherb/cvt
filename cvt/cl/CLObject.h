#ifndef CVT_CLOBJECT_H
#define CVT_CLOBJECT_H

#include <cvt/cl/OpenCL.h>

namespace cvt {
	template<typename T>
	class CLObject
	{
		public:
			CLObject();
			CLObject( const T& other );
			CLObject( const CLObject<T>& other );
			~CLObject();

			CLObject<T>& operator=( const CLObject<T>& other );
			operator T () const { return _object; }

		protected:
			cl_int retain() const;
			cl_int release() const;
			T _object;
	};

	template<typename T>
	inline CLObject<T>::CLObject() : _object( NULL )
	{
	}

	template<typename T>
	inline CLObject<T>::CLObject( const T& other ) : _object( NULL )
	{
		_object = other;
		retain();
	}

	template<typename T>
	inline CLObject<T>::CLObject( const CLObject<T>& other ) : _object( NULL )
	{
		_object = other._object;
		retain();
	}

	template<typename T>
    CLObject<T>& CLObject<T>::operator=( const CLObject<T>& other )
	{
		if( &other == this )
			return *this;
		release();
		_object = other._object;
		retain();
		return *this;
	}


	template<typename T>
	inline CLObject<T>::~CLObject()
	{
		release();
	}

#define X( t, ref, unref ) \
	template<> \
	inline cl_int CLObject<t>::retain() const\
	{ \
		if( _object == NULL ) \
			return CL_SUCCESS; \
		return ref( _object ); \
	} \
	\
	template<> \
	inline cl_int CLObject<t>::release() const\
	{ \
		if( _object == NULL ) \
			return CL_SUCCESS; \
		return unref( _object ); \
	}
#include "CLObject.inl"
#undef X

	typedef CLObject<cl_mem> CLMemory;
};

#endif
