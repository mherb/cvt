#ifndef CVT_FERNS_H
#define CVT_FERNS_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/PatchGenerator.h>
#include <cvt/vision/FeatureExtractor.h>

#include <Eigen/Core>
#include <vector>

namespace cvt 
{
	
	class Ferns 
	{
		
		public:
			Ferns( uint32_t patchSize = 21, uint32_t numOverallTests = 300, uint32_t numFerns = 30 );
			~Ferns();
			
			void train( const Image & img );
			float classify( Eigen::Vector2i & bestClass, const Image & img, const Eigen::Vector2i & p );
			
		private:
			/* a fern is a set of pixel tests */
			struct Fern
			{
				Fern( uint32_t numTests, uint32_t patchSize ) : 
					_regPrior( 1.0f ),
					_patchSize( patchSize )
				{					
					_tests.reserve( numTests );
					_numProbs = ( 1 << numTests );
					
					// one result per class
					_probsForResult.resize( _numProbs );
				}
				
				~Fern()
				{
				}			
				
				void addTest( uint32_t idx )
				{
					_tests.push_back( idx );
				}
				
				/* imP must point to the top left corner of the patch */
				uint32_t test( const uint8_t * imP, size_t stride )
				{										
					uint32_t idx = 0;
					Eigen::Vector2i x0, x1;
					for( size_t i = 0; i < _tests.size(); i++ ){						
						id2pos( _tests[ i ], x0, x1 );
						
						if( imP[ x0[ 1 ] * stride + x0[ 0 ] ] < imP[ x1[ 1 ] * stride + x1[ 0 ] ] )
							idx++;
						idx <<= 1;
					}
					idx >>= 1;
					
					return idx;
				}
			
				void normalize( uint32_t numSamples, size_t classIndex )
				{
					float normalizer = 0.0f;
					float tmp;
					for( uint32_t i = 0; i < _numProbs; i++ ){
						tmp = ( _probsForResult[ i ][ classIndex ] + _regPrior ) / ( ( float )numSamples * _regPrior * _numProbs );
						_probsForResult[ i ][ classIndex ] = tmp;						
						normalizer += tmp;
					}
					
					for( uint32_t i = 0; i < _numProbs; i++ ){
						tmp = _probsForResult[ i ][ classIndex ] / normalizer;
						_probsForResult[ i ][ classIndex ] = log( tmp );
					}
				}
				
				void addClass( const Image & img, const Eigen::Vector2i & pos )
				{	
					size_t classIndex = _probsForResult[ 0 ].size();				
					for( size_t i = 0; i < _probsForResult.size(); i++ ){
						_probsForResult[ i ].push_back( 0.0 );
					}
					
					/* train the class */
					PatchGenerator patchGen( Rangef( 0.0f, Math::TWO_PI ), Rangef( 0.8f, 1.2f ), _patchSize );
					Image patch( _patchSize, _patchSize, img.format() );
					
					uint32_t testResult;
					size_t pStride;
					uint8_t * p = patch.map( &pStride );
					
					uint32_t _numTrainingPatches = 2000;
					
					for( size_t i = 0; i < _numTrainingPatches; i++ ){
						patchGen.next( patch, img, pos );
						
						testResult = test( p, pStride );
						
						_probsForResult[ testResult ][ classIndex ] += 1.0;
					}
					
					patch.unmap( p );
					
					/* normalize the results for this class */
					normalize( _numTrainingPatches, classIndex );
				}
				
				const std::vector<float> & probsForResult( uint32_t result )
				{
					return _probsForResult[ result ];					
				}
				
				private:				
					// tests in a fern are accessible by ids
					std::vector<uint32_t> 	_tests;
					uint32_t	_numProbs;
					float		_regPrior;
					uint32_t 	_patchSize;
					
					/* trained probabilities for each possible result and classes (2^S per class) */
					std::vector< std::vector<float> > _probsForResult;
					
					/* every test has a unique id within the n*(n-1) overall possible tests */
					void id2pos( uint32_t id, Eigen::Vector2i & x0, Eigen::Vector2i & x1 )
					{
						uint32_t numPositions = Math::sqr( _patchSize );
						
						div_t decode0 = div( id, numPositions );		
						div_t point = div( decode0.quot, numPositions );
						
						x0[ 0 ] = point.rem;
						x0[ 1 ] = point.quot;

						uint32_t localIdx = decode0.rem;
						if( localIdx >= ( uint32_t )decode0.quot )
							localIdx++;

						point = div( localIdx, numPositions );		
						x1[ 0 ] = point.rem;
						x1[ 1 ] = point.quot;
					}
			};
			
			uint32_t 			_patchSize;
			uint32_t			_numFerns;
			uint32_t 			_nTests;
			uint32_t			_testsPerFern;
			
			std::vector<Fern>			_ferns;
						
			FeatureExtractor<int32_t>* 	_featureDetector;
			
			std::vector<Eigen::Vector2i>	_modelFeatures;	
	};
	
}
#endif