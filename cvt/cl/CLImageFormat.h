#ifndef CVT_CLIMAGEFORMAT_H
#define CVT_CLIMAGEFORMAT_H

#include <CL/opencl.h>
#include <iostream>

namespace cvt {
	class CLImageFormat
	{
		friend std::ostream& operator<<( std::ostream& out, const CLImageFormat& format );
		public:
			CLImageFormat( cl_image_format format );
			CLImageFormat( cl_channel_order order, cl_channel_type type );
			CLImageFormat( const CLImageFormat& format );

			operator cl_image_format () const { return _format; }

			static const char* orderToString( cl_channel_order order );
			static const char* typeToString( cl_channel_type type  );
		private:
			cl_image_format _format;
	};


	inline CLImageFormat::CLImageFormat( cl_image_format format ) : _format( format )
	{
	}

	inline CLImageFormat::CLImageFormat( cl_channel_order order, cl_channel_type type )
	{
		 _format.image_channel_order = order;
		 _format.image_channel_data_type = type;
	}

	inline CLImageFormat::CLImageFormat( const CLImageFormat& format ) : _format( format._format )
	{
	}

	inline std::ostream& operator<<( std::ostream& out, const CLImageFormat& format )
	{
		out << "CLImageFormat:" << CLImageFormat::orderToString( format._format.image_channel_order )
			<< " " << CLImageFormat::typeToString( format._format.image_channel_data_type );
		return out;
	}
}


#endif
