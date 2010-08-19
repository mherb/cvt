#include "io/ImageIO.h"
#include "util/Exception.h"

#include <png.h>

namespace cvt {
    namespace ImageIO {

	/* FIXME ... */
	void loadPNG( Image& img, std::string path )
	{
	    FILE *fp = fopen( path.c_str(), "rb");
	    if (!fp)
		throw CVTException( "Cannot open PNG image file: " + path );

	    unsigned char header[8];
	    png_uint_32 width, height;
	    int bit_depth, color_type, interlace_type;

	    fread( header, 1, 8, fp);

	    if( png_sig_cmp(header, 0, 8) != 0 )
		throw CVTException( "Invalid PNG image header: " + path );

	    png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );

	    png_infop info_ptr = png_create_info_struct( png_ptr );

	    if( setjmp( png_jmpbuf( png_ptr ) ) ) {
		fclose( fp );
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		png_destroy_info_struct( png_ptr, (png_infopp) &info_ptr);
		throw CVTException( "IO error during PNG loading: " + path );
	    }
	    png_init_io(png_ptr, fp);
	    png_set_sig_bytes(png_ptr, 8);

	    png_read_info(png_ptr, info_ptr);

	    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			 &interlace_type, NULL, NULL);

	    /* tell libpng to strip 16 bit/color files down to 8 bits/color */
	    png_set_strip_16(png_ptr);

	    /* expand paletted colors into true RGB triplets */
	    if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);

	    /* expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
	    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand(png_ptr);

	    /* expand paletted or RGB images with transparency to full alpha channels
	     * so the data will be available as RGBA quartets */
	    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);

	    if (color_type == PNG_COLOR_TYPE_RGB )
		png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);

	    if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(png_ptr);


	    int channels = png_get_channels(png_ptr, info_ptr);
	    img.reallocate( width, height, channels!=1?CVT_BGRA:CVT_GRAY, CVT_UBYTE );

	    png_bytepp row_pointers = new png_bytep[height];
	    for (unsigned y = 0; y < height; y++)
		row_pointers[y] = img.data() + y * img.stride();

	    png_read_image(png_ptr, row_pointers);
	    png_read_end(png_ptr, info_ptr);

	    delete [] row_pointers;
	    fclose( fp );
	    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	    png_destroy_info_struct( png_ptr, (png_infopp) &info_ptr);
	}

	void savePNG( const Image& img, std::string path )
	{

	}
    }
}

