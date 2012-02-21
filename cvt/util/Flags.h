/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_FLAGS_H
#define CVT_FLAGS_H

namespace cvt
{	
	template<typename ENUM>
	class Flags
	{
		public:
			inline Flags( ) : _flags( 0 ) {}
			inline Flags( const Flags<ENUM> &f ) : _flags( f._flags ) {}
			inline Flags( ENUM f ) : _flags( f ) {}

			inline Flags<ENUM>& operator=( const Flags<ENUM> &f ) { _flags = f._flags; return *this; }
			inline Flags<ENUM>& operator|=( Flags<ENUM> f ) { _flags |= f._flags; return *this; }
			inline Flags<ENUM>& operator^=( Flags<ENUM> f ) { _flags ^= f._flags; return *this; }
			inline Flags<ENUM> operator|( Flags<ENUM> f ) const { Flags<ENUM> g; g._flags = _flags | f._flags; return g; }
			inline Flags<ENUM> operator^( Flags<ENUM> f ) const { Flags<ENUM> g; g._flags = _flags ^ f._flags; return g; }

			inline Flags<ENUM>& operator&=( size_t mask ) { _flags &= mask; return *this; }
			inline Flags<ENUM>& operator|=( ENUM f ) { _flags |= f; return *this; }			
			inline Flags<ENUM> operator&( size_t mask ) const { Flags<ENUM> g; g._flags = _flags & mask; return g; }
			inline Flags<ENUM> operator&( ENUM f ) const { Flags<ENUM> g; g._flags = _flags & f; return g; }
			inline Flags<ENUM> operator|( ENUM f ) const { Flags<ENUM> g; g._flags = _flags | f; return g; }
			inline Flags<ENUM> operator^( ENUM f ) const { Flags<ENUM> g; g._flags = _flags ^ f; return g; }
			inline Flags<ENUM>& operator^=( ENUM f ) { _flags ^= f; return *this; }

			inline operator size_t( ) const { return _flags; }

		private:
			size_t _flags;
	};
	
#define CVT_ENUM_TO_FLAGS( ENUM, NAME ) \
	typedef Flags<ENUM> NAME; \
	inline Flags<ENUM> operator|( ENUM flag1, ENUM flag2 ) \
	{ \
		return Flags<ENUM>( flag1 ) | flag2; \
	}

}

#endif
