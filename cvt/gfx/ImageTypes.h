#ifndef IMAGETYPES_H
#define IMAGETYPES_H
#include <stdlib.h>
#include <iostream>

namespace cvt {

	enum IChannelOrder_t {
		ICHANNELORDER_GRAY = 0,
		ICHANNELORDER_GRAYALPHA,
		ICHANNELORDER_RGBA,
		ICHANNELORDER_BGRA
	};

	enum IChannelType_t {
		ICHANNELTYPE_UBYTE = 0,
		ICHANNELTYPE_FLOAT,
	};

	enum IBayerPattern_t {
		IBAYER_RGGB = 0
	};

	struct IOrder {
		IChannelOrder_t id;
		size_t channels;

		bool operator==( const IOrder& x ) const { return id == x.id; };
		bool operator!=( const IOrder& x ) const { return id != x.id; };

		static const IOrder GRAY;
		static const IOrder GRAYALPHA;
		static const IOrder RGBA;
		static const IOrder BGRA;
	};

	struct IType {
		IChannelType_t id;
		size_t size;

		bool operator==( const IType& x ) const { return id == x.id; };
		bool operator!=( const IType& x ) const { return id != x.id; };

		static const IType UBYTE;
		static const IType FLOAT;
	};


}

#endif
