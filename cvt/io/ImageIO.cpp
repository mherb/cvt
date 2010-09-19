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

			/* expand paletted or RGB images with transparency to full alpha channels
			 * so the data will be available as RGBA quartets */
			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
				png_set_expand(png_ptr);

			if (color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_expand(png_ptr);

			int channels = png_get_channels(png_ptr, info_ptr);
			switch (color_type) {
				case PNG_COLOR_TYPE_GRAY:
					if(bit_depth < 8)
						png_set_expand(png_ptr);
					img.reallocate( width, height, IOrder::GRAY, IType::UBYTE );		    
					break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:
					img.reallocate( width, height, IOrder::GRAYALPHA, IType::UBYTE );
					break;
				case PNG_COLOR_TYPE_RGB:
					/* expand paletted colors into true RGB triplets */		    
					png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
				case PNG_COLOR_TYPE_RGBA:
					png_set_bgr(png_ptr);
					img.reallocate( width, height, IOrder::BGRA, IType::UBYTE );
					break;
				default:
					break;
			}


			png_bytepp row_pointers = new png_bytep[height];
			uint8_t* base = img.map();
			for (unsigned y = 0; y < height; y++)
				row_pointers[y] = base + y * img.stride();

			png_read_image(png_ptr, row_pointers);
			png_read_end(png_ptr, info_ptr);

			delete [] row_pointers;
			img.unmap();

			fclose( fp );
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
			png_destroy_info_struct( png_ptr, (png_infopp) &info_ptr);
		}

		void savePNG( const Image& img, std::string path )
		{	    
			FILE *fp;	            
			fp = fopen(path.c_str(), "wb");
			if (fp == NULL){
				throw CVTException("Could not create file ...");
			}

			png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);	    
			if (png_ptr == NULL) {
				fclose(fp);
				throw CVTException("Could not create png_structp");
			}

			/* Allocate/initialize the image information data.  REQUIRED */
			png_infop info_ptr = png_create_info_struct(png_ptr);
			if (info_ptr == NULL){
				fclose(fp);
				png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
				throw CVTException("Could not create png info struct");
			}

			Image tmpImage( img.width(), img.height(), img.order(), IType::UBYTE );
			img.convert( tmpImage );

			/* Set error handling.  REQUIRED if you aren't supplying your own
			 * error handling functions in the png_create_write_struct() call.
			 */
			if (setjmp(png_jmpbuf(png_ptr))){		
				fclose(fp);
				png_destroy_write_struct(&png_ptr, &info_ptr);
				throw CVTException("Error while writing png");
			}

			/* Set up the output control if you are using standard C streams */
			png_init_io(png_ptr, fp);

			int channels;
			switch (tmpImage.order().id ) {
				case ICHANNELORDER_GRAY:
					channels = PNG_COLOR_TYPE_GRAY;
					break;
				case ICHANNELORDER_GRAYALPHA:
					channels = PNG_COLOR_TYPE_GA;
					break;
				case ICHANNELORDER_RGBA:
					channels = PNG_COLOR_TYPE_RGBA;
					break;
				case ICHANNELORDER_BGRA:
					channels = PNG_COLOR_TYPE_RGBA;
					png_set_bgr(png_ptr);
					break;
				default:
					throw CVTException("Input channel format not supported for writing");
					break;
			}

			png_set_IHDR(png_ptr, info_ptr, 
						 tmpImage.width(), 
						 tmpImage.height(), 
						 tmpImage.bpc(), 
						 channels,
						 PNG_INTERLACE_NONE, 
						 PNG_COMPRESSION_TYPE_BASE, 
						 PNG_FILTER_TYPE_BASE);

			/* 
			 *	Optional gamma chunk is strongly suggested if you have any guess
			 *	as to the correct gamma of the image.
			 */
			//double gamma = 2.2;
			//png_set_gAMA(png_ptr, info_ptr, gamma);   

			/* Write the file header information.  REQUIRED */
			png_write_info(png_ptr, info_ptr);

			png_bytep row_pointers[tmpImage.height()];

			if (tmpImage.height() > PNG_UINT_32_MAX/png_sizeof(png_bytep)){
				throw CVTException("Image is too tall to process in memory");
			}

			uint8_t* base = tmpImage.map();
			for (size_t k = 0; k < tmpImage.height(); k++)
				row_pointers[k] = base + tmpImage.stride() * k;

			png_write_image(png_ptr, row_pointers);

			/* It is REQUIRED to call this to finish writing the rest of the file */
			png_write_end(png_ptr, info_ptr);

			tmpImage.unmap();

			/* Clean up after the write, and free any memory allocated */
			png_destroy_write_struct(&png_ptr, &info_ptr);

			/* Close the file */
			fclose(fp);    
		}
	}
}

