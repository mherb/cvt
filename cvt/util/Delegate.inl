#ifdef CVTDELEGATE_H

/*
   Mediocre Delegate implementation:
   - needs heap memory
   - additional indirection
   + but readable code
 */

/* General delegate interface */
template<typename T0 TYPENAMELIST>
class DelegateImpl<T0 ( TYPELIST )>
{
	public:
		virtual ~DelegateImpl() {}
		virtual T0 invoke( TYPELIST ) const = 0;
};

/* Delegate implementation for pointer to member */
template<class T, typename T0 TYPENAMELIST >
class DelegateMember<T, T0 ( TYPELIST )> : public DelegateImpl<T0 ( TYPELIST )>
{
	typedef T0 (T::*MemberPtr)( TYPELIST );

	public:
		DelegateMember( T* obj, MemberPtr mptr ) : _obj( obj ), _mptr( mptr ) {}

		virtual T0 invoke( TYPEARGLIST ) const
		{
			return (_obj->*_mptr)( ARGLIST );
		}

	private:
		T* _obj;
		MemberPtr _mptr;
};

/* Delegate implementation for pointer to const member */
template<class T, typename T0 TYPENAMELIST >
class DelegateMemberConst<T, T0 ( TYPELIST )> : public DelegateImpl<T0 ( TYPELIST )>
{
	typedef T0 (T::*MemberPtr)( TYPELIST ) const;

	public:
		DelegateMemberConst( T* obj, MemberPtr mptr ) : _obj( obj ), _mptr( mptr ) {}

		virtual T0 invoke( TYPEARGLIST ) const
		{
			return (_obj->*_mptr)( ARGLIST );
		}

	private:
		T* _obj;
		MemberPtr _mptr;
};

/* Delegate implementation for function pointers */
template<typename T0 TYPENAMELIST >
class DelegateFunction<T0 ( TYPELIST )> : public DelegateImpl<T0 ( TYPELIST )>
{
	typedef T0 (*FuncPtr)( TYPELIST );

	public:
		DelegateFunction( FuncPtr fptr ) : _fptr( fptr ) {}

		virtual T0 invoke( TYPEARGLIST ) const
		{
			return _fptr( ARGLIST );
		}

	private:
		FuncPtr _fptr;
};

/* The real delegate */
template<typename T0 TYPENAMELIST >
class Delegate<T0 ( TYPELIST )>
{
	public:
		template<class T>
			Delegate( T* t, T0 ( T::*ptr )( TYPELIST ) )
			{
				_impl = new DelegateMember<T, T0 ( TYPELIST )>( t, ptr );
			}

		template<class T>
			Delegate( T* t, T0 ( T::*ptr )( TYPELIST ) const )
			{
				_impl = new DelegateMemberConst<T, T0 ( TYPELIST )>( t, ptr );
			}

		Delegate( T0 ( *func )( TYPELIST ) )
		{
			_impl = new DelegateFunction<T0 ( TYPELIST )>( func );
		}

		~Delegate()
		{
			delete _impl;
		}

		T0 operator()( TYPEARGLIST ) const
		{
			return _impl->invoke( ARGLIST );
		}

	private:
		DelegateImpl<T0 ( TYPELIST )>* _impl;
};


#endif
