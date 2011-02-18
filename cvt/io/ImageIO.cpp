#include "io/ImageIO.h"
#include "util/Exception.h"

#include <iostream>
#include <fstream>

#include <png.h>

namespace cvt {
	namespace ImageIO {

		/* FIXME ... */
		void loadPNG( Image& img, std::string path )
		{
			size_t rdlen;
			FILE *fp = fopen( path.c_str(), "rb");
			if (!fp)
				throw CVTException( "Cannot open PNG image file: " + path );

			unsigned char header[8];
			png_uint_32 width, height;
			int bit_depth, color_type, interlace_type;

			rdlen = fread( header, 1, 8, fp);
			if( rdlen != 8 )
				throw CVTException( "Invalid PNG image header: " + path );

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

			switch (color_type) {
				case PNG_COLOR_TYPE_GRAY:
					if(bit_depth < 8)
						png_set_expand(png_ptr);
					img.reallocate( width, height, IFormat::GRAY_UINT8 );
					break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:
					img.reallocate( width, height, IFormat::GRAYALPHA_UINT8 );
					break;
				case PNG_COLOR_TYPE_RGB:
				case PNG_COLOR_TYPE_PALETTE:
					/* expand paletted colors into true RGB triplets */		    
					png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
				case PNG_COLOR_TYPE_RGBA:
					png_set_bgr(png_ptr);
					img.reallocate( width, height, IFormat::BGRA_UINT8 );
					break;
				default:
					std::cout << color_type << std::endl;
					throw CVTException("Unsupported PNG format");
					break;
			}


			png_bytepp row_pointers = new png_bytep[height];
			size_t stride;
			uint8_t* base = img.map( &stride );
			for (unsigned y = 0; y < height; y++)
				row_pointers[y] = base + y * stride;

			png_read_image(png_ptr, row_pointers);
			png_read_end(png_ptr, info_ptr);

			delete [] row_pointers;
			img.unmap( base );

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
				png_destroy_write_struct(&png_ptr,  ( png_infopp )NULL);
				throw CVTException("Could not create png info struct");
			}

			// convert image to UINT8 equivalent!
			Image tmpImage( img.width(), img.height(), IFormat::uint8Equivalent( img.format() ) );
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
			switch( tmpImage.format().formatID ) {
				case IFORMAT_GRAY_UINT8:
					channels = PNG_COLOR_TYPE_GRAY;
					break;
				case IFORMAT_GRAYALPHA_UINT8:
					channels = PNG_COLOR_TYPE_GA;
					break;
				case IFORMAT_RGBA_UINT8:
					channels = PNG_COLOR_TYPE_RGBA;
					break;
				case IFORMAT_BGRA_UINT8:
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
						 tmpImage.bpc() * 8, 
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

			png_bytep row_pointers[ tmpImage.height() ];

			if (tmpImage.height() > PNG_UINT_32_MAX/png_sizeof(png_bytep)){
				throw CVTException("Image is too tall to process in memory");
			}

			size_t stride;
			uint8_t* base = tmpImage.map( &stride );
			for(size_t k = 0; k < tmpImage.height(); k++)
				row_pointers[k] = base + stride * k;

			png_write_image(png_ptr, row_pointers);

			/* It is REQUIRED to call this to finish writing the rest of the file */
			png_write_end(png_ptr, info_ptr);

			tmpImage.unmap( base );

			/* Clean up after the write, and free any memory allocated */
			png_destroy_write_struct(&png_ptr, &info_ptr);

			/* Close the file */
			fclose(fp);    
		}
        
        void saveRAW( const Image & img, const std::string & path )
        {
            std::fstream file( path.c_str(), 
                               std::fstream::out | 
                               std::fstream::binary | 
                               std::fstream::trunc );
            
            // header: width, height, stride, IFormatID
            size_t stride;
            const char *p; 
            p = img.map<char>( &stride );
            
            size_t formatId = ( size_t )img.format().formatID;
            size_t width = img.width();
            size_t height = img.height();
            
            file.write( ( char* )&width, sizeof( size_t ) );
            file.write( ( char* )&height, sizeof( size_t ) );
            file.write( ( char* )&stride, sizeof( size_t ) );
            file.write( ( char* )&formatId, sizeof( size_t ) );            
            file.write( p, img.height() * stride );
            
            img.unmap( p );
            
            file.close();
        }
        
        void loadRAW( Image & img, const std::string & path )
        {
            std::fstream file( path.c_str(), 
                               std::fstream::in | 
                               std::fstream::binary );
            
            // header: width, height, stride, IFormat
            size_t savedStride, width, height, formatId;
            
            file.read( ( char* )&width, sizeof( size_t ) );
            file.read( ( char* )&height, sizeof( size_t ) );
            file.read( ( char* )&savedStride, sizeof( size_t ) );
            file.read( ( char* )&formatId, sizeof( size_t ) );            
            
            img.reallocate( width, height, IFormat::formatForId( ( IFormatID ) formatId ) );

            char *p, *punmap; 
            size_t stride;
            p = punmap = img.map<char>( &stride );
            
            if( savedStride == stride ){
                file.read( p, height * stride );
            } else {
                size_t offset = savedStride - width;
                while( height-- ){
                    file.read( p, width * img.bpp() );
                    file.seekg( offset, std::ios::end );
                    p += stride;
                }
            } 
            
            img.unmap( punmap );
            
            file.close();
        }
	}
}

