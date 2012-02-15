#ifndef CVT_IMAPSCOPED_H
#define CVT_IMAPSCOPED_H

#include <cvt/gfx/Image.h>

namespace cvt {

	template<typename T>
		class IMapScoped {
			private:
				template<typename T2>
					struct ITypeSelector;

				template<typename T2>
					struct ITypeSelector
					{
						typedef Image& IType;
						typedef uint8_t* IPtrType;
					};

				template<typename T2>
					struct ITypeSelector<const T2>
					{
						typedef const Image& IType;
						typedef const uint8_t* IPtrType;
					};

				typedef typename ITypeSelector<T>::IType IType;
				typedef typename ITypeSelector<T>::IPtrType IPtrType;

			public:
				IMapScoped( IType img );
				~IMapScoped();

				void		reset();
				T*			ptr();
				void		prevLine();
				void		nextLine();
				void		operator++( int );
				void		operator--( int );
				size_t		stride() const;

			private:
				IType	    _img;
				IPtrType	_base;
				IPtrType	_line;
				size_t		_stride;
		};

	template<typename T>
	inline IMapScoped<T>::IMapScoped( IMapScoped<T>::IType img ) : _img( img )
	{
		_base = _img.map( &_stride );
		_line = _base;
	}

	template<typename T>
	inline IMapScoped<T>::~IMapScoped( )
	{
		_img.unmap( _base );
	}

	template<typename T>
	inline void IMapScoped<T>::reset()
	{
		_line = _base;
	}

	template<typename T>
	inline T* IMapScoped<T>::ptr()
	{
		return ( T* ) _line;
	}

	template<typename T>
	inline void IMapScoped<T>::nextLine()
	{
		_line += _stride;
	}

	template<typename T>
	inline void IMapScoped<T>::prevLine()
	{
		_line -= _stride;
	}

	template<typename T>
	inline void IMapScoped<T>::operator++( int )
	{
		_line += _stride;
	}

	template<typename T>
	inline void IMapScoped<T>::operator--( int )
	{
		_line -= _stride;
	}

	template<typename T>
	inline size_t IMapScoped<T>::stride() const
	{
		return _stride;
	}

}

#endif
