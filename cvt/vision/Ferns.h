#ifndef CVT_FERNS_H
#define CVT_FERNS_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/PatchGenerator.h>
#include <cvt/vision/FeatureExtractor.h>

#include <Eigen/Core>
#include <vector>
#include <utility>

namespace cvt 
{
	
	class Ferns 
	{
		
		public:
			Ferns( uint32_t patchSize = 21, uint32_t numOverallTests = 300, uint32_t numFerns = 30 );
			~Ferns();
			
			void train( const Image & img );
			double classify( Eigen::Vector2i & bestClass, const Image & img, const Eigen::Vector2i & p );
			
		private:
			/* a fern is a set of pixel tests */
			struct Fern
			{
				Fern( uint32_t numTests, uint32_t patchSize ) : 
					_regPrior( 1.0 ),
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
				
				void addTest( const Eigen::Vector2i & p0, const Eigen::Vector2i & p1 )
				{
					_tests.push_back( std::make_pair( p0, p1 ) );
				}
				
				/* imP must point to the top left corner of the patch */
				uint32_t test( const uint8_t * imP, size_t stride )
				{										
					uint32_t idx = 0;
					for( size_t i = 0; i < _tests.size(); i++ ){						
						Eigen::Vector2i & x0 = _tests[ i ].first;
						Eigen::Vector2i & x1 = _tests[ i ].second;
						
						if( imP[ x0[ 1 ] * stride + x0[ 0 ] ] < imP[ x1[ 1 ] * stride + x1[ 0 ] ] )
							idx++;
						idx <<= 1;
					}
					idx >>= 1;
					
					return idx;
				}
			
				void normalize( uint32_t numSamples, size_t classIndex )
				{
					double normalizer = 0.0;
					double tmp;
					for( uint32_t i = 0; i < _numProbs; i++ ){
						tmp = ( _probsForResult[ i ][ classIndex ] + _regPrior ) / ( ( double )numSamples * _regPrior * _numProbs );
						_probsForResult[ i ][ classIndex ] = tmp;						
						normalizer += tmp;
					}
					
					for( uint32_t i = 0; i < _numProbs; i++ ){
						tmp = _probsForResult[ i ][ classIndex ] / normalizer;
						//_probsForResult[ i ][ classIndex ] = log( tmp );
						_probsForResult[ i ][ classIndex ] = tmp;
					}
				}
				
				void addClass( const Image & img, const Eigen::Vector2i & pos )
				{	
					size_t classIndex = _probsForResult[ 0 ].size();				
					for( size_t i = 0; i < _probsForResult.size(); i++ ){
						_probsForResult[ i ].push_back( 0.0 );
					}
					
					/* train the class */
					PatchGenerator patchGen( Rangef( 0.0f, Math::TWO_PI ), Rangef( 0.6f, 1.5f ), _patchSize );
					
					Image patch( _patchSize, _patchSize, img.format() );
					
					uint32_t testResult;
					size_t pStride;
					
					
					uint32_t _numTrainingPatches = 1000;
					
					for( size_t i = 0; i < _numTrainingPatches; i++ ){
						patchGen.next( patch, img, pos );
						
						uint8_t * p = patch.map( &pStride );
						testResult = test( p, pStride );
						patch.unmap( p );						
						
						_probsForResult[ testResult ][ classIndex ] += 1.0;
					}					
					
					/* normalize the results for this class */
					normalize( _numTrainingPatches, classIndex );
				}
				
				const std::vector<double> & probsForResult( uint32_t result )
				{
					return _probsForResult[ result ];					
				}
				
				private:				
					// tests in a fern: two points w.r.t. upper left corner of patch
					typedef std::pair<Eigen::Vector2i, Eigen::Vector2i> PixelPair;
					std::vector<PixelPair>	_tests;
					uint32_t				_numProbs;
					float					_regPrior;
					uint32_t				_patchSize;
					
					/* trained probabilities for each possible result and classes (2^S per class) */
					std::vector< std::vector<double> > _probsForResult;					
					
			};
			
			uint32_t 			_patchSize;
			uint32_t			_numFerns;
			uint32_t 			_nTests;
			uint32_t			_testsPerFern;
			
			std::vector<Fern>				_ferns;
						
			FeatureExtractor<int32_t>*		_featureDetector;
			
			std::vector<Eigen::Vector2i>	_modelFeatures;	
	};
	
}
#endif