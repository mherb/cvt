#include <cvt/vision/Ferns.h>

#include <cvt/vision/FAST.h>

namespace cvt
{
	
	Ferns::Ferns( uint32_t patchSize, uint32_t numOverallTests, uint32_t numFerns ) : 
		_patchSize( patchSize ),
		_numFerns( numFerns ),
		_nTests( numOverallTests ),		
		_featureDetector( 0 )
	{		
		_ferns.reserve( numFerns );
		
		while( ( _nTests % _numFerns ) != 0 ){
			_nTests++;
		}
		_testsPerFern = _nTests / _numFerns;
		
		_featureDetector = new FAST( SEGMENT_10 );
		static_cast<FAST*>(_featureDetector)->setMinScore( 60 );
		static_cast<FAST*>(_featureDetector)->setThreshold( 40 );
	}
	
	Ferns::Ferns( const std::string & fileName ) : _featureDetector( 0 )
	{
		std::ifstream file;
		std::string line;
		std::istringstream strTok;
		
		file.open( fileName.c_str(), std::ifstream::in );
		
		getline( file, line );
		strTok.str( line );
		strTok >> _patchSize;
		
		getline( file, line );
		strTok.clear();
		strTok.str( line );
		strTok >> _numFerns;
		
		getline( file, line );
		strTok.clear();
		strTok.str( line );
		strTok >> _nTests;
		
		getline( file, line );
		strTok.clear();
		strTok.str( line );
		strTok >> _testsPerFern;
		
		uint32_t numFeatures;
		getline( file, line );
		strTok.clear();
		strTok.str( line );
		strTok >> numFeatures;
		
		Eigen::Vector2i p;
		std::string lineVal;
		for( size_t i = 0; i < numFeatures; i++ ){
			getline( file, line );
		
			strTok.clear();
			strTok.str( line );
			strTok >> p[ 0 ];
			strTok >> p[ 1 ];			
			
			_modelFeatures.push_back( p );			
		}
		
		uint32_t numProbs;
		float	 regPrior;
		for( size_t i = 0; i < _numFerns; i++ ){
			getline( file, line );
			strTok.clear();
			strTok.str( line );
			strTok >> numProbs;
			
			getline( file, line );
			strTok.clear();
			strTok.str( line );
			strTok >> regPrior;
						
			_ferns.push_back( Fern( _testsPerFern, _patchSize ) );
			Eigen::Vector2i p0, p1;
			for( uint32_t t = 0; t < _testsPerFern; t++ ){				
				getline( file, line );
				
				strTok.clear();
				strTok.str( line );
				
				strTok >> p0[ 0 ];
				strTok >> p0[ 1 ];
				strTok >> p1[ 0 ];
				strTok >> p1[ 1 ];
				_ferns.back().addTest( p0, p1 );				
			}
									
			for( size_t p = 0; p < numProbs; p++ ){				
				double prob;				
				getline( file, line );
				strTok.clear();
				strTok.str( line );
				for( size_t c = 0; c < _modelFeatures.size(); c++ ){
					strTok >> prob;
					_ferns.back().setProbabilityForTest( p, c, prob );
				}
				
			}
		}		
	}
	
	Ferns::~Ferns()
	{	
		if( _featureDetector )
			delete _featureDetector;
	}
	
	void Ferns::train( const Image & img )
	{
		RNG rng( time( NULL ) );
		
		Eigen::Vector2i x0, x1;		
		for( uint32_t i = 0; i < _numFerns; i++ ){
			_ferns.push_back( Fern( _testsPerFern, _patchSize ) );
			
			for( uint32_t t = 0; t < _testsPerFern; t++ ){
				x0[ 0 ] = rng.uniform( 0, _patchSize );
				x0[ 1 ] = rng.uniform( 0, _patchSize );
				x1[ 0 ] = rng.uniform( 0, _patchSize );
				x1[ 1 ] = rng.uniform( 0, _patchSize );

				_ferns.back().addTest( x0, x1 );
			}
		}	
		
		// detect features in the "model"-image	
		std::vector<Feature2D> features;
		_featureDetector->extractMultiScale( img, features, 3 );
		
		int32_t patchHalfSize = _patchSize >> 1;
		
		Eigen::Vector2i pCenter;
		int x, y;
		for( size_t i = 0; i < features.size(); i++ ){
			std::cout << "FEATURE " << i+1 << " / " << features.size() << std::endl;
			x = features[ i ][ 0 ];
			y = features[ i ][ 1 ];
			if( ( x - patchHalfSize ) < 0 ||
			 	( x + patchHalfSize ) >= ( int32_t )img.width() ||
				( y - patchHalfSize ) < 0 ||
				( y + patchHalfSize ) >= ( int32_t )img.height() )
				continue;
			
			pCenter[ 0 ] = features[ i ][ 0 ];
			pCenter[ 1 ] = features[ i ][ 1 ];
						
			_modelFeatures.push_back( pCenter );
			
			for( size_t f = 0; f < _ferns.size(); f++ ){
				_ferns[ f ].addClass( img, pCenter );
			}
		}
	}
	
	double Ferns::classify( Eigen::Vector2i & bestClass, const Image & img, const Eigen::Vector2i & p )
	{
		size_t imStride;
        
		
        const uint8_t * imP = img.map( &imStride );
		int32_t patchHalfSize = _patchSize >> 1;
        
        if( p[ 0 ] - patchHalfSize < 0 ||
            p[ 0 ] + patchHalfSize > ( int32_t )img.width() || 
            p[ 1 ] - patchHalfSize < 0 ||
            p[ 1 ] + patchHalfSize > ( int32_t )img.height() )
            return 0.0f;
        
		const uint8_t * uL = imP + imStride * ( p[ 1 ] - patchHalfSize ) + p[ 0 ] - patchHalfSize;

		std::vector<double> classProbs( _modelFeatures.size(), 0.0 );
		
		uint32_t testResult;

		// for each fern
		for( size_t i = 0; i < _ferns.size(); i++ ){
			testResult = _ferns[ i ].test( uL, imStride );
			const std::vector<double> & trainedProbs = _ferns[ i ].probsForResult( testResult );
			
			for( size_t f = 0; f < trainedProbs.size(); f++ ){
				classProbs[ f ] += trainedProbs[ f ];
			}
		}
		
		img.unmap( imP );
		
		uint32_t bestIdx = 0;
		for( size_t i = 0; i < classProbs.size(); i++ ){
			if( classProbs[ i ] > classProbs[ bestIdx ] )
				bestIdx = i;
		}
		
		bestClass[ 0 ] = _modelFeatures[ bestIdx ][ 0 ];
		bestClass[ 1 ] = _modelFeatures[ bestIdx ][ 1 ];
		
		return classProbs[ bestIdx ];						
	}
	
	void Ferns::save( const std::string & fileName )
	{
		std::ofstream out;
		
		out.open( fileName.c_str(), std::ios_base::out );
		
		out << _patchSize << std::endl;
		out << _numFerns << std::endl;
		out << _nTests << std::endl;
		out << _testsPerFern << std::endl;
		
		out << _modelFeatures.size() << std::endl;		
		for( size_t i = 0; i < _modelFeatures.size(); i++ ){
			out << _modelFeatures[ i ].x() << " " << _modelFeatures[ i ].y() << std::endl;
		}
		
		for( size_t i = 0; i < _ferns.size(); i++ ){
			_ferns[ i ].serialize( out );
		}
		
		out.close();
	}
		
}