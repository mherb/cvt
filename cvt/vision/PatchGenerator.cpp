#include <cvt/vision/PatchGenerator.h>

namespace cvt
{
	
	PatchGenerator::PatchGenerator( const Rangef & angleRange,
									const Rangef & scaleRange, 
									uint32_t patchSize,
									double whiteNoiseSigma ) :
		_patchSize( patchSize ), 
		_angleRange( angleRange ), 
		_scaleRange( scaleRange ),		
		_whiteNoiseSigma( whiteNoiseSigma ),
		_rng( time( NULL ) )
	{
	}
	
	PatchGenerator::~PatchGenerator()
	{
	}
	
	void PatchGenerator::randomizeAffine()
	{
		float phi, theta, s1, s2;
		Eigen::Matrix2f R_phi, R_theta, S;
		
		phi = _rng.uniform( _angleRange.min, _angleRange.max );
		theta = _rng.uniform( _angleRange.min, _angleRange.max );
		s1 = _rng.uniform( _scaleRange.min, _scaleRange.max );
		s2 = _rng.uniform( _scaleRange.min, _scaleRange.max );	
		
		//std::cout << s1 << ", " << s2 << std::endl;
		
		S << s1, 0.0f, 0.0f, s2;
		
		float c = Math::cos( phi );
		float s = Math::sin( phi );		
		R_phi << c, -s, s, c; 
		
		c = Math::cos( theta );
		s = Math::sin( theta );		
		R_theta << c, -s, s, c;
		
		_affine = R_theta * R_phi.transpose() * S * R_phi;
	}
	
	void PatchGenerator::next( Image & outputPatch, const cvt::Image & inputImage, const Eigen::Vector2i & patchCenter )
	{
		// generate new random affine Transform
		randomizeAffine();
		
		outputPatch.reallocate( _patchSize, _patchSize, inputImage.format() );
		
		size_t inStride, outStride;
		const uint8_t * in = inputImage.map( &inStride );
		uint8_t * out = outputPatch.map( &outStride );
		uint8_t * outSave = out;
		
		Eigen::Vector2f currentP, pPrime;
		
		int32_t x, y;	
		uint32_t numChannels = outputPatch.channels();
		
		currentP[ 1 ] = -(float)_patchSize / 2.0f;	
		double pixelNoise;
		int32_t tmp;
		for( uint32_t i = 0; i < _patchSize; i++ ){
			currentP[ 0 ] = -(float)_patchSize / 2.0f;
			for( uint32_t j = 0; j < _patchSize; j++ ){	
				pPrime = _affine * currentP;
												
				x = int32_t( pPrime[ 0 ] + float( patchCenter[ 0 ] ) );
				y = int32_t( pPrime[ 1 ] + float( patchCenter[ 1 ] ) );								
				
				if( x < 0 ||
				    y < 0 ||
				    ( uint32_t )x >= inputImage.width() ||
					( uint32_t )y >= inputImage.height() ){
					for( size_t c = 0; c < numChannels; c++ )
						out[ numChannels * j  + c ] = 0;
				} else {
					pixelNoise = Math::clamp( _rng.gaussian( _whiteNoiseSigma ), -20.0, 20.0 );
					for( size_t c = 0; c < numChannels; c++ ){						
						tmp = in[ y * inStride + x * numChannels + c ] + pixelNoise;
											
						out[ numChannels * j + c ] = (uint8_t)Math::clamp( tmp, 0, 255 );
					}
				}
				currentP[ 0 ] += 1.0f;
			}
			
			out += outStride;
			currentP[ 1 ] += 1.0f;
		}
		
		inputImage.unmap( in );
		outputPatch.unmap( outSave );
			
	}
	
}