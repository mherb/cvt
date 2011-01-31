#include <cvt/gfx/GFX.h>
#include <cvt/util/CVTAssert.h>
#include <cvt/gfx/GFXEngine.h>

namespace cvt {

	GFX::GFX() : _engine( NULL ), _active( false )
	{
		setDefault();
	}

	GFX::GFX( const GFX& g ) :  _engine( NULL ), _active( false ), _color( g._color ), _linewidth( g._linewidth )
	{
	}

	GFX::GFX( GFXEngine* engine ) : _engine( engine ), _active( false )
	{
		setDefault();
		begin();
	}

	GFX::~GFX()
	{
		if( _active )
			end();
	}

	GFX::GFX( Drawable* drawable )
	{
		setDefault();
		_engine = drawable->gfxEngine();
		begin();
	}

	void GFX::begin()
	{
		CVT_ASSERT( _active == false, "GFX engine already active" );
		_active = true;
		_engine->begin();
	}

	void GFX::begin( GFXEngine* engine )
	{
		CVT_ASSERT( _active == false && _engine == NULL && engine, "GFX engine already active or NULL"  );
		_engine = engine;
		begin();
	}

	void GFX::begin( Drawable* draw )
	{
		CVT_ASSERT( _active == false && _engine == NULL && draw, "GFX engine already active or NULL"   );
		_engine = draw->gfxEngine();
		CVT_ASSERT( _engine, "GFX engine is NULL" );
		begin();
	}

	void GFX::end()
	{
		CVT_ASSERT( _active, "GFX engine is not active"  );
		_active = false;
		_engine->end();
	}

	void GFX::setDefault()
	{
		_color.set( 1.0f, 1.0f, 1.0f, 1.0f );
		_linewidth = 1.0f;
	}

	void GFX::drawLine( int x1, int y1, int x2, int y2 )
	{
		Vector2f pts[ 2 ];
		pts[ 0 ].set( ( float ) x1, ( float ) y1 );
		pts[ 1 ].set( ( float ) x2, ( float ) y2 );
		_engine->drawLines( pts, 2, _linewidth, _color );
	}

	void GFX::drawLines( const Vector2f* pts, size_t n )
	{
		_engine->drawLines( pts, n, _linewidth, _color );
	}

	void GFX::fillRect( const Recti& rect )
	{
		_engine->fillRect( rect, _color );
	}

	void GFX::fillRect( int x, int y, int width, int height )
	{
		Recti rect( x, y, width, height );
		_engine->fillRect( rect, _color );
	}

	void GFX::fillRoundRect( const Recti& rect, float radius )
	{
		_engine->fillRoundRect( rect, radius, _color );
	}

	void GFX::fillRoundRect( int x, int y, int width, int height, float radius )
	{
		Recti rect( x, y, width, height );
		_engine->fillRoundRect( rect, radius, _color );
	}

	// FIXME: remove and add Font class
	int  GFX::textWidth( const char* text )
	{
		return 0;
	}

	void GFX::drawText( int x, int y, const char* text )
	{
		_engine->drawText( x, y, text, _color );
	}

	void GFX::drawImage( int x, int y, const Image& img )
	{
		_engine->drawImage( x, y, img, _color.alpha() );
	}

	void GFX::drawImage( int x, int y, int width, int height, const Image& img )
	{
		Recti rect( x, y, width, height );
		_engine->drawImage( rect , img, _color.alpha() );
	}

	void GFX::drawIcon( int x, int y, Icon i )
	{
		_engine->drawIcon( x, y, i, _color );
	}

	void GFX::drawIcons( const Vector2i* pts, size_t npts, Icon i )
	{
		_engine->drawIcons( pts, npts, i, _color );
	}

	void GFX::drawModel( GLModel& mdl, const Matrix4f& modelview, float near, float far  )
	{

	}

	void GFX::setChildrect( const Recti& r )
	{
		_engine->setChildrect( r );
	}

	const Recti& GFX::childrect() const
	{
		return _engine->childrect();
	}
}
