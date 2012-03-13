#include <cvt/vision/ORB.h>
#include <cvt/vision/FeatureMatch.h>
#include <cvt/vision/FeatureFilter.h>
#include <cvt/vision/slam/stereo/ORBStereoMatching.h>
#include <cvt/io/Resources.h>
#include <cvt/gfx/ifilter/IWarp.h>
#include <cvt/gfx/GFXEngineImage.h>

using namespace cvt;

void createStereoImage( Image& out, const Image& i0, const Image& i1 )
{
	Image outTmp;
	outTmp.reallocate( i0.width() + i1.width(), Math::max( i0.height(), i1.height() ), IFormat::GRAY_UINT8 );

	outTmp.copyRect( 0, 0, i0, i0.rect() );
	outTmp.copyRect( i0.width(), 0, i1, i1.rect() );

	outTmp.convert( out, IFormat::RGBA_UINT8 );
}

void drawMatches( Image& out, const std::vector<FeatureMatch>& matches )
{
	{
		GFXEngineImage ge( out );
		GFX g( &ge );

		float xoff = out.width() / 2.0f;

		Recti r;
		size_t pSize = 3;
		size_t phSize = pSize >> 1;
		r.width = pSize;
		r.height = pSize;
		Vector2f p0, p1;
		for( size_t i = 0; i < matches.size(); i++ ){
			g.color() = Color::GREEN;
			p0 = matches[ i ].feature0->pt;
			p1 = matches[ i ].feature1->pt;
			p1.x += xoff;

			r.x = ( int )p0[ 0 ] - phSize;
			r.y = ( int )p0[ 1 ] - phSize;
			g.drawRect( r );

			r.x = ( int )p1[ 0 ] - phSize;
			r.y = ( int )p1[ 1 ] - phSize;
			g.drawRect( r );

			// line between the two points
			g.color() = Color::RED;
			g.drawLine( p0, p1 );
		}
	}
}

template <class Container>
void drawFeatures( Image& out, const Container& container )
{
	{
		GFXEngineImage ge( out );
		GFX g( &ge );

		Recti r;
		size_t pSize = 3;
		size_t phSize = pSize >> 1;
		r.width = pSize;
		r.height = pSize;
		g.color() = Color::GREEN;
		for( size_t i = 0; i < container.size(); i++ ){
			r.x = ( int )container[ i ].pt[ 0 ] - phSize;
			r.y = ( int )container[ i ].pt[ 1 ] - phSize;
			g.drawRect( r );
		}
	}
}

template <class PointerContainer>
struct FeaturePointerTrait
{
	FeaturePointerTrait( PointerContainer& cont ) : _cont( cont )
	{
	}

	size_t size() const { return _cont.size(); }
	const Feature2Df& operator[]( size_t idx ) const { return *_cont[ idx ]; }

	PointerContainer& _cont;
};

int main()
{
	Resources r;
	String calib0 = r.find( "calib_stereo/ueye_4002738788.xml" );
	String calib1 = r.find( "calib_stereo/ueye_4002738790.xml" );

	// load the calibration data:
	CameraCalibration camCalib0, camCalib1;
	camCalib0.load( calib0 );
	camCalib1.load( calib1 );

	// load the images:
	Image img0, img1;
	img0.load( r.find( "calib_stereo/ueye_4002738788.png" ) );
	img1.load( r.find( "calib_stereo/ueye_4002738790.png" ) );

	// undistort the images
	Image warp0( img0.width(), img0.height(), IFormat::GRAYALPHA_FLOAT );
	Image warp1( img1.width(), img1.height(), IFormat::GRAYALPHA_FLOAT );

	Image undist0, undist1;

	Matrix3f knew0, knew1;
	IWarp::warpUndistort( warp0, knew0, camCalib0, img0.width(), img0.height() );
	IWarp::warpUndistort( warp1, knew1, camCalib1, img1.width(), img1.height() );
	IWarp::apply( undist0, img0, warp0 );
	IWarp::apply( undist1, img1, warp1 );

	Image gray0, gray1;

	undist0.convert( gray0, IFormat::GRAY_UINT8 );
	undist1.convert( gray1, IFormat::GRAY_UINT8 );

	// compute the orbs
	ORB orb0( gray0, 3, 0.5f, 25, 0, true );
	ORB orb1( gray1, 3, 0.5f, 25, 0, false );

	Image outColor;
	gray0.convert( outColor, IFormat::RGBA_UINT8 );
	drawFeatures( outColor, orb0 );
	outColor.save( "orb0.png" );
	gray1.convert( outColor, IFormat::RGBA_UINT8 );
	drawFeatures( outColor, orb1 );
	outColor.save( "orb1.png" );

	// filter the features with a grid
	FeatureFilter filter( 20, gray0.width(), gray0.height() );
	for( size_t i = 0; i < orb0.size(); i++ ){
		filter.addFeature( &orb0[ i ] );
	}
	std::vector<const Feature2Df*> filteredFeatures0;
	filter.gridFilteredFeatures( filteredFeatures0, 1000 );

	FeaturePointerTrait<std::vector<const Feature2Df*> > fpcont( filteredFeatures0 );
	gray0.convert( outColor, IFormat::RGBA_UINT8 );
	drawFeatures( outColor, fpcont );
	outColor.save( "filtered0.png" );

	// compute stereo matches
	float maxLineDist = 5.0f;
	float maxDescDist = 50;
	ORBStereoMatching stereoMatcher( maxLineDist, maxDescDist, camCalib0, camCalib1 );
	std::vector<FeatureMatch> matches;
	std::set<size_t> usedIndexes;
	
	FeatureMatch match;
	std::set<size_t> orb1Assigned;

	for( size_t i = 0; i < filteredFeatures0.size(); i++ ){
	//for( size_t i = 0; i < orb0.size(); i++ ){
	//	match.feature0 = &orb0[ i ];
		match.feature0 = filteredFeatures0[ i ];
		size_t id = stereoMatcher.matchEpipolar( match, orb1, orb1Assigned );

		if( match.distance < maxDescDist ) { 
			orb1Assigned.insert( id );
			matches.push_back( match );
		} 
	} 

	Image out;
	createStereoImage( out, undist0, undist1 );
	drawMatches( out, matches );

	out.save( "orbstereomatching.png" );
}
