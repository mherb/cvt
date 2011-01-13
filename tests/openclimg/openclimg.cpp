#include <cvt/cl/CLContext.h>
#include <cvt/cl/CLKernel.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/ImageIO.h>
#include <cvt/math/Math.h>
#include <cvt/io/Resources.h>
#include <cvt/math/Vector.h>

#include <string>
#include <iostream>

#include "fill.h"
#include "gaussiir.h"
#include "gaussiir2.h"

using namespace cvt;


static const float _IIR[ 3 ][ 2 ][ 4 ] =
{
	 1.680f  ,  3.735f,  0.6318f, -1.783f,
	-0.6803f , -0.2598,  1.997f,  -1.723f,

	 -0.6472f, -4.531f,  0.6719f, -1.527f,
	  0.6494f,  0.9557f, 2.072f,  -1.516f,

	 -1.331f  ,  3.661f, 0.7480f, -1.240f,
	  0.3225f , -1.738f, 2.166f,  -1.314f
};

#define L1 ( _IIR[ order ][ 0 ][ 3 ] )
#define L2 ( _IIR[ order ][ 1 ][ 3 ] )
#define W1 ( _IIR[ order ][ 0 ][ 2 ] )
#define W2 ( _IIR[ order ][ 1 ][ 2 ] )
#define B1 ( _IIR[ order ][ 0 ][ 1 ] )
#define B2 ( _IIR[ order ][ 1 ][ 1 ] )
#define A1 ( _IIR[ order ][ 0 ][ 0 ] )
#define A2 ( _IIR[ order ][ 1 ][ 0 ] )

static void GaussIIRCoeff_D( float sigma, int order, Vector4f& d, float& SD )
{
	const float cw1 = Math::cos( W1 / sigma );
	const float cw2 = Math::cos( W2 / sigma );
	const float el1 = Math::exp( L1 / sigma );
	const float el2 = Math::exp( L2 / sigma );

	d[ 0 ] = -2.0f * ( el2 * cw2 + el1 * cw1 );
	d[ 1 ] = 4.0f * el1 * el2 * cw1 * cw2 + Math::sqr( el1 ) + Math::sqr( el2 );
	d[ 2 ] = -2.0f * el1 * Math::sqr( el2 ) * cw1 - 2.0f * Math::sqr( el1 ) * el2 * cw2;
	d[ 3 ] = Math::sqr( el1 * el2 );
	
	SD = 1.0f + d[ 0 ] + d[ 1 ] + d[ 2 ] + d[ 3 ];
}

static void GaussIIRCoeff_N( float sigma, int order, Vector4f& n, float& SN )
{
	const float cw1 = Math::cos( W1 / sigma );
	const float cw2 = Math::cos( W2 / sigma );
	const float sw1 = Math::sin( W1 / sigma );
	const float sw2 = Math::sin( W2 / sigma );
	const float el1 = Math::exp( L1 / sigma );
	const float el2 = Math::exp( L2 / sigma );

	n[ 0 ] = A1 + A2;
	n[ 1 ] = el2 * ( B2 * sw2 - ( A2 + 2.0f * A1 ) * cw2 ) + el1 * ( B1 * sw1 - ( A1 + 2.0f * A2 ) * cw1 );
	n[ 2 ] = 2.0f * el1 *el2 * ( ( A1 + A2 ) * cw2 * cw1 - B1 * cw2 * sw1 - B2 * cw1 * sw2  ) + A2 * Math::sqr( el1 ) + A1 * Math::sqr( el2 );
	n[ 3 ] = Math::sqr( el1 ) * el2 * ( B2 *sw2 - A2 * cw2 ) + el1 * Math::sqr( el2 ) * ( B1 * sw1 - A1 * cw1 );

	SN = n[ 0 ] + n[ 1 ] + n[ 2 ] + n[ 3 ];
}

void GaussIIRCoeff( float sigma, int order, Vector4f& n, Vector4f& m, Vector4f& d )
{
	order = Math::clamp( order, 0, 2 );
	sigma = Math::max( sigma, 0.05f );
	float SN, SD;

	GaussIIRCoeff_D( sigma, order, d, SD );
	GaussIIRCoeff_N( sigma, order, n, SN );

	if( order == 0 ) {
		float alpha = 2.0f * SN / SD - n[ 0 ];
		n[ 0 ] = n[ 0 ] / alpha;
		n[ 1 ] = n[ 1 ] / alpha;
		n[ 2 ] = n[ 2 ] / alpha;
		n[ 3 ] = n[ 3 ] / alpha;
	} else if( order == 1 ) {
		float DN = n[ 1 ] + 2.0f * n[ 2 ] + 3.0f * n[ 3 ];
		float DD = d[ 0 ] + 2.0f * d[ 1 ] + 3.0f * d[ 2 ] + 4.0f * d[ 3 ];
		float alpha =  2.0f * ( SN * DD - DN * SD ) / Math::sqr( SD );
		n[ 0 ] = n[ 0 ] / alpha;
		n[ 1 ] = n[ 1 ] / alpha;
		n[ 2 ] = n[ 2 ] / alpha;
		n[ 3 ] = n[ 3 ] / alpha;
	} else {
		Vector4f n0, d0;
		float SN0, SD0, DN0;
		GaussIIRCoeff_D( sigma, 0, d0, SD0 );
		GaussIIRCoeff_N( sigma, 0, n0, SN0 );
		float DN = n[ 1 ] + 2.0f * n[ 2 ] + 3.0f * n[ 3 ];
		float DD = d[ 0 ] + 2.0f * d[ 1 ] + 3.0f * d[ 2 ] + 4.0f * d[ 3 ];
		float beta = -( 2.0f * SN - SD * n[ 0 ] ) / ( 2.0f * SN0 - SD * n0[ 0 ] );
		float EN = n[ 1 ] + 4.0f * n[ 2 ] + 9.0f * n[ 3 ];
		float EN0 = n0[ 1 ] + 4.0f * n0[ 2 ] + 9.0f * n0[ 3 ];
		float ED = d[ 0 ] + 4.0f * d[ 1 ] + 9.0f * d[ 3 ] + 16.0f * d[ 4 ];
	    float SN2 = SN + beta * SN0;
		float DN2 = DN + beta * DN0;
		float EN2 = EN + beta * EN0;
		float alpha = EN2 * Math::sqr( SD ) - ED * SN2 * SD - 2.0f * DN2 * DD * SD + 2.0f * Math::sqr( DD ) * SN;
		alpha /= SD * SD * SD;
		n[ 0 ] = ( n[ 0 ] + beta * n0[ 0 ] ) / alpha;
		n[ 1 ] = ( n[ 1 ] + beta * n0[ 1 ] ) / alpha;
		n[ 2 ] = ( n[ 2 ] + beta * n0[ 2 ] ) / alpha;
		n[ 3 ] = ( n[ 3 ] + beta * n0[ 3 ] ) / alpha;
	}

	float n0 = n[ 0 ];
	m[ 0 ] = n[ 1 ] - d[ 0 ] * n0;
	m[ 1 ] = n[ 2 ] - d[ 1 ] * n0;
	m[ 2 ] = n[ 3 ] - d[ 2 ] * n0;
	m[ 3 ] =		- d[ 3 ] * n0;

	if( order == 1 ) {
		m[ 0 ] = -m[ 0 ];
		m[ 1 ] = -m[ 1 ];
		m[ 2 ] = -m[ 2 ];
		m[ 3 ] = -m[ 3 ];
	}
}

int main( int argc, char** argv )
{
	CLContext cl;
	std::string log;
	
	Resources resources;

	try {

		cl.makeCurrent();

		CLKernel kernel, kerneliir, kerneliir2;
		kernel.build("fill", _fill_source, strlen( _fill_source ), log );
		kerneliir.build("gaussiir", _gaussiir_source, strlen( _gaussiir_source ), log );
		kerneliir2.build("gaussiir2", _gaussiir2_source, strlen( _gaussiir2_source ), log );

		Image* tmp = new Image();
		ImageIO::loadPNG( *tmp, resources.find( "lena.png" ) );
		Image x( *tmp, IALLOCATOR_CL );
		delete tmp;
		Image x2;
		x2.reallocate( x, IALLOCATOR_CL );
		cl::Buffer buf( cl.getCLContext(), CL_MEM_READ_WRITE, sizeof( float ) * 4 * 512 * 512 );
		cl::Buffer buf2( cl.getCLContext(), CL_MEM_READ_WRITE, sizeof( float ) * 4 * 512 * 512 );

		//	kernel.setArg( 0, &x );
		//	kernel.run( x, cl::NullRange );

		Vector4f n, m ,d ;
		GaussIIRCoeff( 3.5f, 0, n, m , d );

		kerneliir.setArg( 0, buf );
		kerneliir.setArg( 1, x );
		int32_t w = x.width();
		kerneliir.setArg( 2, sizeof( int32_t ), &w );
		kerneliir.setArg( 3, n );
		kerneliir.setArg( 4, m );
		kerneliir.setArg( 5, d );
		kerneliir.run( cl::NDRange( x.height() ), cl::NDRange( 8 ) );

		kerneliir2.setArg( 0, x2 );
		kerneliir2.setArg( 1, buf );
		kerneliir2.setArg( 2, buf2 );
		int32_t h = x.height();
		kerneliir2.setArg( 3, sizeof( int32_t ), &h );
		kerneliir2.setArg( 4, n );
		kerneliir2.setArg( 5, m );
		kerneliir2.setArg( 6, d );
		kerneliir2.run( cl::NDRange( x.width() ), cl::NDRange( 8 ) );
		cl.getCLQueue().finish();

		ImageIO::savePNG( x2, "bla.png" );
	} catch( CLException e ) {
		std::cout << e.what() << std::endl;
	} catch( Exception e ) {
		std::cout << e.what() << std::endl;
	}


}
