#ifndef CVT_FERNS_H
#define CVT_FERNS_H

#include <cvt/gfx/Image.h>
#include <cvt/vision/PatchGenerator.h>
#include <cvt/vision/internal/Fern.h>
#include <cvt/vision/FAST.h>

#include <Eigen/Core>
#include <vector>
#include <string>
#include <utility>

namespace cvt 
{
	
	class Ferns 
	{
		
		public:
			Ferns( uint32_t patchSize = 21, uint32_t numOverallTests = 300, uint32_t numFerns = 30 );
			Ferns( const std::string & fileName );
			~Ferns();
			
			void train( const Image & img );
			
			double classify( Eigen::Vector2i & bestClass, const Image & img, const Eigen::Vector2i & p );
		
			void match( const::std::vector<Eigen::Vector2i> & features,
						const Image & img,
						std::vector<Eigen::Vector2d> & matchedModel,
					    std::vector<Eigen::Vector2d> & matchedFeatures );
			
			void save( const std::string & fileName );
			
		private:						
			uint32_t	_patchSize;
			uint32_t	_numFerns;
			uint32_t 	_nTests;
			uint32_t	_testsPerFern;
			uint32_t	_trainingSamples;
			
			std::vector<Fern>				_ferns;						
			FAST*                           _featureDetector;			
			std::vector<Feature2Df>	_modelFeatures;	

			void trainClass( size_t idx, PatchGenerator & patchGen, const Image & img );
	};
	
}
#endif