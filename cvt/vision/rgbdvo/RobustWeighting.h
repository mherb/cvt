#ifndef CVT_ROBUST_WEIGHTING_H
#define CVT_ROBUST_WEIGHTING_H

namespace cvt
{
	struct Huber
	{
		Huber( float t ) : c( t ) {}

		float weight( float r ) const
		{
			float t = Math::abs( r );
			if ( t < c ) 
				return 1.0f;
			else 
				return c / t;
		}

		float c;
	};

	struct Tukey
	{
		/**
		 *	\brief	cut-off at threshold (0 influence of outliers!)
		 */
		Tukey( float t ) : c( t ) {}
		
		float weight( float r ) const
		{
			float t = Math::abs( r );
			if ( t > c ) 
				return 0.0f;
			else 
				return Math::sqr( 1 - Math::sqr( r / c ) );
		}

		float c;
	};

	struct Welsch 
	{
		Welsch( float t ) : c( t ) {}
		
		float weight( float r ) const
		{
			return Math::exp( - Math::sqr( r / c ) );
		}

		float c;
	};
	
	struct Cauchy 
	{
		Cauchy( float t ) : c( t ) {}
		
		float weight( float r ) const
		{
			return 1.0f / ( 1.0f + Math::sqr( r / c ) );
		}

		float c;
	};
	
	struct GemanMcClure 
	{
		GemanMcClure( float ) {}
		
		float weight( float r ) const
		{
			return 1.0f / Math::sqr( 1.0f + Math::sqr( r ) );
		}

	};
	
	struct Fair 
	{
		Fair( float t ) : c( t ) {}
		
		float weight( float r ) const
		{
			return 1.0f / ( 1.0f + Math::abs( r ) / c );
		}

		float c;
	};

	struct L1L2Diff 
	{
		L1L2Diff( float ) {}
		
		float weight( float r ) const
		{
			return 1.0f / Math::sqrt( 1.0f + Math::sqr( r ) / 2 );
		}
	};
	
	struct L1
	{
		L1( float ) {}
		
		float weight( float r ) const
		{
			return 1.0f / Math::abs( r );
		}
	};

};

#endif
