#include "FloFile.h"
#include "util/CVTException.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define TAG_FLOAT 202021.25  // check for this when READING the file
#define TAG_STRING "PIEH"    // use this when WRITING the file
#define UNKNOWN_FLOW_THRESH 1e9
#define UNKNOWN_FLOW 1e10 // value to use to represent unknown flow

// ".flo" file format used for optical flow evaluation
//
// Stores 2-band float image for horizontal (u) and vertical (v) flow components.
// Floats are stored in little-endian order.
// A flow value is considered "unknown" if either |u| or |v| is greater than 1e9.
//
//  bytes  contents
//
//  0-3     tag: "PIEH" in ASCII, which in little endian happens to be the float 202021.25
//          (just a sanity check that floats are represented correctly)
//  4-7     width as an integer
//  8-11    height as an integer
//  12-end  data (width*height*2*4 bytes total)
//          the float values for u and v, interleaved, in row order, i.e.,
//          u[row0,col0], v[row0,col0], u[row0,col1], v[row0,col1], ...
//


namespace cvt {
    namespace FloFile {

	void FloReadFile( Image& flow, std::string const& filename )
	{

	    if( filename.compare( filename.length() -4, 4, ".flo" ) != 0 )
		throw CVTException("FloReadFile " + filename + ": extension .flo expected");

	    FILE *stream = fopen( filename.c_str(), "rb" );
	    if (stream == 0)
		throw CVTException("FloReadFile: could not open " + filename );

	    int32_t width, height;
	    float tag;

	    if ( ( int ) fread(&tag,    sizeof( float ), 1, stream) != 1 ||
		( int ) fread(&width,  sizeof( int32_t ),   1, stream) != 1 ||
		( int ) fread(&height, sizeof( int32_t ),   1, stream) != 1)
		throw CVTException("FloReadFile: problem reading file " + filename);

	    if (tag != TAG_FLOAT) // simple test for correct endian-ness
		throw CVTException("FloReadFile(" + filename + "): wrong tag (possibly due to big-endian machine?)" );

	    // another sanity check to see that integers were read correctly (99999 should do the trick...)
	    if (width < 1 || width > 99999)
		throw CVTException("ReadFlowFile(" + filename + "): illegal width" );

	    if (height < 1 || height > 99999)
		throw CVTException("ReadFlowFile(" + filename + "): illegal height" );


	    flow.reallocate( width, height, CVT_GRAYALPHA, CVT_FLOAT );
	    size_t h = flow.height();
	    size_t w2 = flow.width() * 2;
	    uint8_t* ptr = flow.data();
	    size_t stride = flow.stride();
	    while( h-- ) {
		if( fread( ptr, sizeof( float ), w2 , stream) != w2)
		    throw CVTException("ReadFlowFile(" + filename + "): file is too short" );
		ptr += stride;
	    }

	    if( fgetc(stream) != EOF)
		throw CVTException("ReadFlowFile(" + filename + "): file is too long" );

	    fclose(stream);
	}


	void FloWriteFile( Image& flow, std::string const & filename )
	{

	    if( filename.compare( filename.length() -4, 4, ".flo" ) != 0 )
		throw CVTException("FloWriteFile " + filename + ": extension .flo expected");

	    if( flow.order() != CVT_GRAYALPHA && flow.type() != CVT_FLOAT )
		throw CVTException("FloWriteFile " + filename + ": illeagal image format");

	    FILE *stream = fopen(filename.c_str(), "wb");
	    if (stream == 0)
		throw CVTException("FloWriteFile (" + filename + "): could not open file");

	    int32_t width, height;
	    width = ( int32_t ) flow.width();
	    height = ( int32_t ) flow.height();

	    // write the header
	    fprintf(stream, TAG_STRING);
	    if ( fwrite( &width,  sizeof( int32_t ), 1, stream) != 1 ||
		 fwrite( &height, sizeof( int32_t ), 1, stream) != 1)
		throw CVTException( "FloWriteFile (" + filename + "): problem writing header" );

	    size_t h = flow.height();
	    size_t w2 = flow.width() * 2;
	    uint8_t* ptr = flow.data();
	    size_t stride = flow.stride();
	    while( h-- ) {
		if( fwrite( ptr, sizeof( float ), w2 , stream ) != w2)
		    throw CVTException("ReadFlowFile(" + filename + "): problem writing data" );
		ptr += stride;
	    }
	    fclose(stream);
	}

	float FlowAEE( Image& flow, Image& gt )
	{
	    if( flow.width() != gt.width() ||
	        flow.height() != gt.height() ||
		flow.order() != CVT_GRAYALPHA || flow.type() != CVT_FLOAT ||
		gt.order() != CVT_GRAYALPHA || gt.type() != CVT_FLOAT )
		throw CVTException( "FlowAAE: illegal flow data" );

	    size_t stride1, stride2;
	    size_t w, h;
	    uint8_t* ptr1;
	    uint8_t* ptr2;
	    float ee1, ee2;
	    float aee = 0.0f;

	    stride1 = flow.stride();
	    stride2 = gt.stride();
	    w = flow.width();
	    h = flow.height();
	    ptr1 = flow.data();
	    ptr2 = gt.data();


	    while( h-- ) {
		float* d1 = ( float* ) ptr1;
		float* d2 = ( float* ) ptr2;

		for( size_t i = 0; i < w; i++ ) {
		    ee1 = ( d1[ w * 2 ] - d2[ w * 2 ] );
		    ee2 = ( d1[ w * 2 + 1 ] - d2[ w * 2 + 1 ] );
		    aee += sqrtf( ee1 * ee1 + ee2 * ee2 );
		}
		ptr1 += stride1;
		ptr2 += stride2;
	    }

	    return aee / ( ( float ) ( flow.width() * flow.height() ) );
	}

	float FlowAAE( Image& flow, Image& gt )
	{
	    if( flow.width() != gt.width() ||
	        flow.height() != gt.height() ||
		flow.order() != CVT_GRAYALPHA || flow.type() != CVT_FLOAT ||
		gt.order() != CVT_GRAYALPHA || gt.type() != CVT_FLOAT )
		throw CVTException( "FlowAAE: illegal flow data" );

	    size_t stride1, stride2;
	    size_t w, h;
	    uint8_t* ptr1;
	    uint8_t* ptr2;
	    float dot ,dot1, dot2;
	    float aae = 0.0f;

	    stride1 = flow.stride();
	    stride2 = gt.stride();
	    w = flow.width();
	    h = flow.height();
	    ptr1 = flow.data();
	    ptr2 = gt.data();


	    while( h-- ) {
		float* d1 = ( float* ) ptr1;
		float* d2 = ( float* ) ptr2;

		for( size_t i = 0; i < w; i++ ) {
		    dot = 1.0f + ( d1[ w * 2 ] * d2[ w * 2 ] + d1[ w * 2 + 1 ] * d2[ w * 2 + 1 ]);
		    dot1 = 1.0f + ( d1[ w * 2 ] * d1[ w * 2 ] + d1[ w * 2 + 1 ] * d1[ w * 2 + 1 ]);
		    dot2 = 1.0f + ( d2[ w * 2 ] * d2[ w * 2 ] + d2[ w * 2 + 1 ] * d2[ w * 2 + 1 ]);
		    aae += acosf( dot / ( sqrtf( dot1 ) * sqrtf( dot2 ) ) );
		}
		ptr1 += stride1;
		ptr2 += stride2;
	    }

	    return aae / ( ( float ) ( flow.width() * flow.height() ) );
	}

    }
}
