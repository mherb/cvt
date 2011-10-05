#ifndef METAIO_DATA_H
#define METAIO_DATA_H

#include <cvt/util/String.h>
#include <cvt/io/VideoReader.h>
#include <cvt/util/Data.h>
#include <cvt/util/DataIterator.h>
#include <cvt/io/FileSystem.h>

namespace cvt
{
	class MetaioData
	{
		public:
			MetaioData( const String& datasetFolder, size_t seqNr );
			~MetaioData();

			const Image&	templateImage() const { return _template; }
			bool			finished() const;

			const Image&	processNext( Matrix3d& h );

			bool			hasGroundTruthForFrame( std::vector<Vector2d>& pts ) const;

		private:
			VideoReader*	_video;
			int32_t			_frameIndex;
			Image			_template;

			struct GTData
			{
				size_t		index;
				Vector2d	p0, p1, p2, p3;
			};

			std::vector<GTData>		_gtData;

			void loadGroundTruthInfo( const String& gtFile );
	};

	inline MetaioData::MetaioData( const String& dataFolder, size_t seqNr ) :
		_video( 0 ),
		_frameIndex( 0 )
	{
		// construct the path
		String file = dataFolder;
		if( file[ file.length() - 1 ] != '/' )
			file += "/";

		String sequence;
		sequence.sprintf( "gtSeq%02d.avi", seqNr );

		String videoFile = file;
		videoFile += sequence;
		_video = new VideoReader( videoFile, false );

		String gtFile = file;
		sequence.sprintf( "gtInit/gtSeq%02dinit.txt", seqNr );
		gtFile += sequence;
		loadGroundTruthInfo( gtFile );

		// load the template
		String templateFile = file;
		templateFile += "template.png";
		_template.load( templateFile );
	}

	inline MetaioData::~MetaioData()
	{
		if( _video )
			delete _video;
	}

	inline void MetaioData::loadGroundTruthInfo( const String& file )
	{
		Data d;
		FileSystem::load( d, file );

		DataIterator iter( d );
		String delims( " =" );

		while( iter.hasNext() ){
			std::vector<String> tokens;
			iter.tokenizeNextLine( tokens, delims );

			// no empty line
			if( tokens.size() > 0 ){
				if( tokens[ 0 ] == "Initialization" ){
					// new block:
					if( tokens.size() != 5 ){
						throw CVTException( "Ground Truth File Corrupt" );
					}
					
					GTData gtData;
					gtData.index = (size_t)tokens[ 4 ].toInteger();

					if( !iter.hasNext() )
						throw CVTException( "Ground Truth File Corrupt" );

					tokens.clear();
					iter.tokenizeNextLine( tokens, delims );
					if( tokens.size() != 4 )
						throw CVTException( "Ground Truth File Corrupt" );
					gtData.p0.x = tokens[ 0 ].toDouble();
					gtData.p0.y = tokens[ 1 ].toDouble();


					if( !iter.hasNext() )
						throw CVTException( "Ground Truth File Corrupt" );

					tokens.clear();
					iter.tokenizeNextLine( tokens, delims );
					if( tokens.size() != 4 )
						throw CVTException( "Ground Truth File Corrupt" );
					gtData.p1.x = tokens[ 0 ].toDouble();
					gtData.p1.y = tokens[ 1 ].toDouble();
					
					if( !iter.hasNext() )
						throw CVTException( "Ground Truth File Corrupt" );

					tokens.clear();
					iter.tokenizeNextLine( tokens, delims );
					if( tokens.size() != 4 )
						throw CVTException( "Ground Truth File Corrupt" );
					gtData.p2.x = tokens[ 0 ].toDouble();
					gtData.p2.y = tokens[ 1 ].toDouble();

					if( !iter.hasNext() )
						throw CVTException( "Ground Truth File Corrupt" );

					tokens.clear();
					iter.tokenizeNextLine( tokens, delims );
					if( tokens.size() != 4 )
						throw CVTException( "Ground Truth File Corrupt" );
					gtData.p3.x = tokens[ 0 ].toDouble();
					gtData.p3.y = tokens[ 1 ].toDouble();

					_gtData.push_back( gtData );
				}
			}
		}
	}

	inline bool MetaioData::finished() const 
	{
		return _frameIndex == (int32_t)_video->numFrames();
	}

	inline bool MetaioData::hasGroundTruthForFrame( std::vector<Vector2d>& pts ) const
	{
		// TODO: check if _frameIndex is one of the ground truth frames
		for( size_t i = 0; i < _gtData.size(); i++ ){
			if( _gtData[ i ].index == ( size_t )_frameIndex ){
				pts.push_back( _gtData[ i ].p0 );
				pts.push_back( _gtData[ i ].p1 );
				pts.push_back( _gtData[ i ].p2 );
				pts.push_back( _gtData[ i ].p3 );
				return true;
			}
		}
		return false;
	}
	
	inline const Image& MetaioData::processNext( Matrix3d& h ) 
	{
		if( !finished() ){
			_frameIndex++;
			_video->nextFrame();
		}

		return _video->frame();
	}

}

#endif
