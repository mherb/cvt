#include <cvt/gfx/Image.h>
#include <cvt/gfx/IMapScoped.h>
#include <cvt/util/Time.h>
#include <cvt/gfx/ifilter/BoxFilter.h>
#include <cvt/gfx/ifilter/AdaptiveThreshold.h>
#include <cvt/gfx/IComponents.h>
#include <cvt/cl/OpenCL.h>
#include <cvt/cl/CLPlatform.h>

using namespace cvt;


int main( int argc, char** argv )
{
	Image tmp( argv[ 1 ] );
	Image src, dst, isrc, dst2;

	std::vector<CLPlatform> platforms;
	CLPlatform::get( platforms );
	CL::setDefaultDevice( platforms[ 0 ].defaultDevice() );

	tmp.convert( src, IFormat::GRAY_FLOAT );
	isrc.reallocate( src.width(), src.height(), IFormat::GRAY_FLOAT );
	src.integralImage( isrc );

	BoxFilter box;
	dst.reallocate( src.width(), src.height(), IFormat::GRAY_FLOAT );
	Time t;
	box.apply( dst, isrc, 2 );
	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;
	dst.save( "outbox2.png" );


	dst2.reallocate( src.width(), src.height(), IFormat::GRAY_UINT8 );
	AdaptiveThreshold ath;

	ath.apply( dst2, src, dst, 0.01f );
	dst2.save( "outath.png" );

	t.reset();
	IComponents<float> comp( dst2 );

	std::cout << t.elapsedMilliSeconds() << " ms" << std::endl;

	{
		for( size_t i = 0; i < comp.size(); i++ ) {
			Image icomps( src.width(), src.height(), IFormat::GRAY_UINT8 );
			icomps.fill( Color::BLACK );
			{
				PointSet<2,float>& ptset = comp[ i ];
				if( ptset.size() < 20 )
					continue;

				IMapScoped<uint8_t> map( icomps );
				for( size_t k = 0; k < ptset.size(); k++ ) {
					Vector2f pt = ptset[ k ];
					*( map.base() + map.stride() * ( int ) pt.y + ( int ) pt.x ) = 0xff;
				}
			}
			icomps.save( "outcomponents.png" );
			getchar();
		}
	}


}
