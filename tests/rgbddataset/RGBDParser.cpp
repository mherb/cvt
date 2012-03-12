#include "RGBDParser.h"

namespace cvt
{

	RGBDParser::RGBDParser( const String& folder ) :
	    _maxStampDiff( 0.01 ), // this is 10ms	
		_folder( folder ),
		_idx( 0 )
	{
		if( _folder[ _folder.length() - 1 ] != '/' )
			_folder += "/";

		std::vector<double> rgbStamps;
		std::vector<double> depthStamps;
		loadRGBFilenames( rgbStamps );
		loadDepthFilenames( depthStamps );	
		loadGroundTruth();
		sortOutData( rgbStamps, depthStamps );
	}

	void RGBDParser::loadNext()
	{
		if( _idx >= _stamps.size() ){
			std::cout << "End of data !" << std::endl;
			return;
		}
		_sample.stamp	= _stamps[ _idx ];
		_sample.rgb.load( _rgbFiles[ _idx ] );
		_sample.depth.load( _depthFiles[ _idx ] );
		_sample.pose = _groundTruthPoses[ _idx ];
		_idx++;
	}

	void RGBDParser::loadGroundTruth()
	{
		String file;
		file.sprintf( "%sgroundtruth.txt", _folder.c_str() );

		Data gtFile;
		FileSystem::load( gtFile, file );
		
		DataIterator iter( gtFile );

		Matrix4d p; double stamp;
		while( readNext( p, stamp, iter ) ){
			_groundTruthPoses.push_back( p );	
			_stamps.push_back( stamp );	
		}
	}

	void RGBDParser::loadRGBFilenames( std::vector<double> & rgbStamps )
	{
		String file;
		file.sprintf( "%srgb.txt", _folder.c_str() );

		Data rgbFile;
		FileSystem::load( rgbFile, file );
		DataIterator iter( rgbFile );

		String name;
		double stamp;
		while( readNextFilename( name, stamp, iter ) ){
			rgbStamps.push_back( stamp );
			_rgbFiles.push_back( name );
		}
	}
	
	void RGBDParser::loadDepthFilenames( std::vector<double>& depthStamps )
	{
		String file;
		file.sprintf( "%sdepth.txt", _folder.c_str() );

		Data depthFile;
		FileSystem::load( depthFile, file );
		DataIterator iter( depthFile );

		String name;
		double stamp;
		while( readNextFilename( name, stamp, iter ) ){
			depthStamps.push_back( stamp );
			_depthFiles.push_back( name );
		}
	}

	// read next line from the gtfile	
	bool RGBDParser::readNext( Matrix4d& pose, double& stamp, DataIterator& iter )
	{
		if( !iter.hasNext() )
			return false;
		
		std::vector<String> tokens;
		iter.tokenizeNextLine( tokens, " " );

		while( tokens.size() == 0 || tokens[ 0 ] == "#" ){
			if( !iter.hasNext() )
				return false;
			tokens.clear();
			iter.tokenizeNextLine( tokens, " " );
		}

		// now we should have a valid line with 8 tokens!
		if( tokens.size() != 8 ) std::cout << "File corrupt?!" << std::endl;

		stamp = tokens[ 0 ].toDouble();
		double tx = tokens[ 1 ].toDouble();
		double ty = tokens[ 2 ].toDouble();
		double tz = tokens[ 3 ].toDouble();
		double qx = tokens[ 4 ].toDouble();
		double qy = tokens[ 5 ].toDouble();
		double qz = tokens[ 6 ].toDouble();
		double qw = tokens[ 7 ].toDouble();
		Quaterniond q( qx, qy, qz, qw );

		pose = q.toMatrix4();
		pose[ 0 ][ 3 ] = tx;
		pose[ 1 ][ 3 ] = ty;
		pose[ 2 ][ 3 ] = tz;
		pose[ 3 ][ 3 ] = 1.0;

		return true;
	}

	bool RGBDParser::readNextFilename( String& filename, double& stamp, DataIterator& iter )
	{
		if( !iter.hasNext() )
			return false;
		
		std::vector<String> tokens;
		iter.tokenizeNextLine( tokens, " " );


		while( tokens.size() == 0 || tokens[ 0 ] == "#" ){
			if( !iter.hasNext() ){
				return false;
			} 
			tokens.clear();
			iter.tokenizeNextLine( tokens, " " );
		}

		// now we should have a valid line with 8 tokens!
		if( tokens.size() != 2 ) std::cout << "File corrupt?!" << std::endl;
		
		stamp = tokens[ 0 ].toDouble();
		filename = tokens[ 1 ];

		return true;
	}


	size_t RGBDParser::findClosestMatchInGTStamps( double val, size_t startIdx )
	{
		size_t bestIdx = startIdx;
		double best = Math::abs( _stamps[ startIdx ] - val );
		double last = best;
		for( size_t i = startIdx + 1; i < _stamps.size(); i++ ){
			double cur = Math::abs( _stamps[ i ] - val );
			if( cur < best ){
				best = cur;
				bestIdx = i;
			}
			if( cur > last )
				break;
			last = cur;
		}
		return bestIdx;
	}
	void RGBDParser::sortOutData( const std::vector<double>& rgbStamps, 
								  const std::vector<double>& depthStamps )
	{
	
		size_t gtIdx	= 0;
		size_t depthIdx = 0;
		size_t rgbIdx	= 0;
		size_t savePos	= 0;	

		while( rgbIdx < rgbStamps.size() && 
			   depthIdx < depthStamps.size() && 
			   gtIdx < _stamps.size() ){

			// find best gt pose for this rgb
			size_t bestGtIdx = findClosestMatchInGTStamps( rgbStamps[ rgbIdx ], gtIdx );
			if( Math::abs( _stamps[ bestGtIdx ] - rgbStamps[ rgbIdx ] ) > _maxStampDiff ){
				break; // there is no more good pose for this rgb stamp!
			}

			// find closest depth stamp to the gt stamp
			size_t bestDepthIdx = findClosestMatchInDepthStamps( _stamps[ bestGtIdx ], depthIdx, depthStamps );
			double depthDist = Math::abs( _stamps[ bestGtIdx ] - depthStamps[ bestDepthIdx ] );
			if( depthDist < _maxStampDiff ){
				// take it
				_stamps[ savePos ] = _stamps[ bestGtIdx ];
				_groundTruthPoses[ savePos ] = _groundTruthPoses[ bestGtIdx ];
				_depthFiles[ savePos ] = _depthFiles[ bestDepthIdx ];
				_rgbFiles[ savePos ] = _rgbFiles[ rgbIdx ];
				savePos++;

				gtIdx = bestGtIdx + 1;
				depthIdx = bestDepthIdx + 1;
			} else {
				gtIdx++;
				depthIdx++;
			}

			rgbIdx++;
		}

		// erase the rest: 
		_stamps.erase( _stamps.begin() + gtIdx, _stamps.end() );
		_groundTruthPoses.erase( _groundTruthPoses.begin() + gtIdx, _groundTruthPoses.end() );
		_rgbFiles.erase( _rgbFiles.begin() + rgbIdx, _rgbFiles.end() );
		_depthFiles.erase( _depthFiles.begin() + depthIdx, _depthFiles.end() );
	}


	size_t RGBDParser::findClosestMatchInDepthStamps( double stamp,
													  size_t iter,	
													  const std::vector<double>& dStamps )
	{
		size_t bestIdx = iter;
		double best = Math::abs( dStamps[ iter ] - stamp );
		double last = best;
		for( size_t i = iter + 1; i < dStamps.size(); i++ ){
			double cur = Math::abs( dStamps[ i ] - stamp );
			if( cur < best ){
				best = cur;
				bestIdx = i;
			}
			if( cur > last )
				break;
			last = cur;
		}
		return bestIdx;
	}
}
