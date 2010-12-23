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
		static_cast<FAST*>(_featureDetector)->setMinScore( 80 );
		static_cast<FAST*>(_featureDetector)->setThreshold( 40 );
	}
	
	Ferns::~Ferns()
	{		
		delete _featureDetector;
	}
	
	void Ferns::train( const Image & img )
	{
		RNG rng( time( NULL ) );
		
		// generate the Ferns for the tests
		uint32_t maxTestIdx = ( _patchSize * _patchSize ) * ( _patchSize * _patchSize - 1 );
		
		for( uint32_t i = 0; i < _numFerns; i++ ){
			_ferns.push_back( Fern( _testsPerFern, _patchSize ) );
			
			uint32_t testIdx;
			for( uint32_t t = 0; t < _testsPerFern; t++ ){
				testIdx = rng.uniform( 0, maxTestIdx );
				_ferns.back().addTest( testIdx );
			}
		}	
		
		// detect features in the "model"-image	
		std::vector<Feature2D> features;
		_featureDetector->extractMultiScale( img, features, 3 );
		
		std::cout << "NUMBER OF MODEL FEATURES: " << features.size() << std::endl;
		
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
	
	float Ferns::classify( Eigen::Vector2i & bestClass, const Image & img, const Eigen::Vector2i & p )
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

		std::vector<float> classProbs( _modelFeatures.size(), 0.0f );
		
		uint32_t testResult;

		// for each fern
		for( size_t i = 0; i < _ferns.size(); i++ ){
			// get the test result index of this fern: 
			testResult = _ferns[ i ].test( uL, imStride );
			
			// get the trained probabilities for the classes:
			const std::vector<float> & trainedProbs = _ferns[ i ].probsForResult( testResult );
			
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
	
		
}