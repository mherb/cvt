#include <cvt/io/ImageSequence.h>
#include <cvt/io/FileSystem.h>

namespace cvt {
    
    ImageSequence::ImageSequence( const String& basename,
                                  const String& ext ) :
	   _index( 0 )	
    {       
		std::vector<String> filenames;

		// get the folder:
		String folder;
		if( !extractFolder( folder, basename ) ){
			throw CVTException( "Could not extract folder from basename" );
		}

		FileSystem::filesWithExtension( folder, filenames, ext );
		for( size_t i = 0; i < filenames.size(); i++ ){
			if( filenames[ i ].hasPrefix( basename ) ){
				_files.push_back( filenames[ i ] );
			}
		}
		
		nextFrame();
    }
    
    bool ImageSequence::nextFrame( size_t )
    {
        // build the string and load the frame
		if( _index < _files.size() ){
			_current.load( _files[ _index ] );
			_index++;
			return true;
		} else {
			return false;
		}
    }

	bool ImageSequence::extractFolder( String& folder, const String& basename ) const
	{
		size_t pos = basename.length();
		
		while( pos-- ){
			if( basename[ pos ] == '/' ){
				break;
			}
		}

		if( pos == 0 )
			return false;

		folder = basename.substring( 0, pos+1 );
		return true;	
	}


	bool ImageSequence::hasNext() const
	{
		if( _index < ( _files.size() - 1 ) )
			return true;
		return false;
	}
    
}
