#include <cvt/io/CameraModeSet.h>
#include <cvt/math/Math.h>

namespace cvt {

	CameraModeSet::CameraModeSet()
	{
	}

	CameraModeSet::~CameraModeSet()
	{
	}

	void CameraModeSet::add( const CameraMode & m )
	{
		_modes.push_back( m );
	}


	CameraModeSet CameraModeSet::filterFormat( const IFormat & format ) const
	{
		CameraModeSet ret;
		for( size_t i = 0; i < _modes.size(); i++ ){
			const CameraMode & current = _modes[ i ];

			if( current.format == format )
				ret.add( current );
		}

		return ret;

	}

	CameraModeSet CameraModeSet::filterSize( size_t minWidth, size_t minHeight ) const
	{
		CameraModeSet ret;
		for( size_t i = 0; i < _modes.size(); i++ ){
			const CameraMode & current = _modes[ i ];

			if( current.width >= minWidth &&
			   current.height >= minHeight )
				ret.add( current );
		}

		return ret;

	}

	CameraModeSet CameraModeSet::filterFPS( size_t minFPS ) const
	{
		CameraModeSet ret;
		for( size_t i = 0; i < _modes.size(); i++ ){
			const CameraMode & current = _modes[ i ];

			if( current.fps >= minFPS )
				ret.add( current );
		}

		return ret;
	}

	CameraModeSet CameraModeSet::filter( const IFormat & format, size_t minWidth, size_t minHeight, size_t minFPS ) const
	{
		CameraModeSet ret;
		for( size_t i = 0; i < _modes.size(); i++ ){
			const CameraMode & current = _modes[ i ];

			if( current.format == format &&
			   current.width >= minWidth &&
			   current.height >= minHeight &&
			   current.fps >= minFPS )
				ret.add( current );
		}

		return ret;
	}

	size_t CameraModeSet::size() const
	{
		return _modes.size();
	}

	const CameraMode & CameraModeSet::mode( size_t index ) const
	{
		if( index > _modes.size() ){
			throw CVTException( "Mode index out of bounds" );
		}

		return _modes[ index ];
	}

	CameraMode CameraModeSet::bestMatching( const IFormat & format, size_t width, size_t height, size_t fps  ) const
	{
		size_t dist, bestDist = 100000, bestIdx = 0;
		for( size_t i = 0; i < this->size(); i++ ) {
			const CameraMode & mode = this->mode( i );
			if( mode.format == format )
				dist = 0;
			else
				dist = 1;
			dist += Math::sqr( mode.width - width ) +
					Math::sqr( mode.height - height ) +
					Math::sqr( mode.fps - fps );

			if( dist < bestDist ){
				bestIdx = i;
				bestDist = dist;
			}
		}

		return this->mode( bestIdx );
	}

	std::ostream& operator<<( std::ostream &out, const CameraModeSet & info )
	{
		out << "\tModes:\n\t\t";
		for( size_t i = 0; i < info.size(); i++ ){
			out << info.mode( i ) << ", ";
			if( i != 0 && ( (i+1) % 2 ) == 0 && ( i + 1 ) < info.size() ){
				out << "\n\t\t";
			}
		}

		return out;
	}
}
