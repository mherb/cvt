#ifndef CVT_ROBUST_WEIGHTING_H
#define CVT_ROBUST_WEIGHTING_H

namespace cvt
{
    template <class W>
    struct IsRobustWeighting
    {
        static const bool Value = true;
    };


    template <typename T>
    struct NoWeighting {
        NoWeighting( T ){}
        T weight( T r ){ return (T)1; }
    };

    template< typename T >
    struct IsRobustWeighting< NoWeighting<T> >
    {
        static const bool Value = false;
    };

    template <typename T>
	struct Huber
	{
        Huber( T t ) : c( t ) {}

        T weight( T r ) const
		{
            T t = Math::abs( r );
			if ( t < c ) 
                return 1;
			else 
				return c / t;
		}

        T c;
	};

    template <typename T>
	struct Tukey
	{
		/**
		 *	\brief	cut-off at threshold (0 influence of outliers!)
		 */
        Tukey( T t ) : c( t ) {}
		
        T weight( T r ) const
		{
            T t = Math::abs( r );
			if ( t > c ) 
                return (T)0;
			else 
				return Math::sqr( 1 - Math::sqr( r / c ) );
		}

        T c;
	};

    template <typename T>
	struct Welsch 
	{
        Welsch( T t ) : c( t ) {}
		
        T weight( T r ) const
		{
			return Math::exp( - Math::sqr( r / c ) );
		}

        T c;
	};
	
    template <typename T>
	struct Cauchy 
	{
        Cauchy( T t ) : c( t ) {}
		
        T weight( T r ) const
		{
            return (T)1.0 / ( (T)1.0 + Math::sqr( r / c ) );
		}

        T c;
	};

    template <typename T>
	struct GemanMcClure 
	{
        GemanMcClure( T ) {}
		
        T weight( T r ) const
		{
            return (T)1 / Math::sqr( (T)1 + Math::sqr( r ) );
		}

	};
	
    template <typename T>
	struct Fair 
	{
        Fair( T t ) : c( t ) {}
		
        T weight( T r ) const
		{
            return (T)1 / ( (T)1 + Math::abs( r ) / c );
		}

        T c;
	};

    template <typename T>
	struct L1L2Diff 
	{
        L1L2Diff( T ) {}
		
        T weight( T r ) const
		{
            return (T)1 / Math::sqrt( (T)1 + Math::sqr( r ) / 2 );
		}
	};
	
    template <typename T>
	struct L1
	{
        L1( T ) {}
		
        T weight( T r ) const
		{
            return (T)1 / Math::abs( r );
		}
	};

}

#endif
