#ifndef CVTDELEGATE_H
#define CVTDELEGATE_H

/*
    Mediocre Delegate implementation:
    - needs heap memory
    - additional indirection
    + but readable code
 */

namespace cvt {
    template<typename> class Delegate;
    template<typename> class DelegateImpl;
    template<typename> class DelegateFunction;
    template<typename, typename> class DelegateMember;

    /* General delegate interface */
    template <typename T0, typename T1>
	class DelegateImpl<T0 (T1)>
	{
	    public:
		virtual ~DelegateImpl() {}
		virtual T0 invoke( T1 ) const = 0;
	};

    /* Delegate implementation for pointers to members */
    template <class T, typename T0, typename T1 >
	class DelegateMember<T, T0 (T1)> : public DelegateImpl<T0 (T1)>
	{
	    typedef T0 (T::*MemberPtr)(T1);

	    public:
		DelegateMember( T* obj, MemberPtr mptr ) : _obj( obj ), _mptr( mptr ) {}

		virtual T0 invoke( T1 arg ) const
		{
		    return (_obj->*_mptr)( arg );
		}

	    private:
		T* _obj;
		MemberPtr _mptr;
	};

    /* Delegate implementation for function pointers */
    template <typename T0, typename T1 >
	class DelegateFunction<T0 (T1)> : public DelegateImpl<T0 (T1)>
	{
	    typedef T0 (*FuncPtr)(T1);

	    public:
		DelegateFunction( FuncPtr fptr ) : _fptr( fptr ) {}

		virtual T0 invoke( T1 arg ) const
		{
		    return _fptr( arg );
		}

	    private:
		FuncPtr _fptr;
	};

    /* The real delegate */
    template <typename T0, typename T1>
	class Delegate<T0 (T1)>
	{
	    public:
		template <class T>
		Delegate( T* t, T0 ( T::*ptr )( T1 ) )
		{
		    _impl = new DelegateMember<T, T0 ( T1 )>( t, ptr );
		}

		Delegate( T0 ( *func )( T1 ) )
		{
		    _impl = new DelegateFunction<T0 ( T1 )>( func );
		}

		virtual ~Delegate()
		{
		    delete _impl;
		}

		virtual T0 operator()( T1 arg ) const
		{
		    return _impl->invoke( arg );
		}

	    private:
		DelegateImpl<T0 (T1)>* _impl;
	};



}

#endif
