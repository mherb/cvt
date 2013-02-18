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
        T weight( T ){ return (T)1; }

        void setThreshold( T /*thresh*/ ){}
        void setScale( T /*sigma*/ ){}
    };

    template< typename T >
    struct IsRobustWeighting< NoWeighting<T> >
    {
        static const bool Value = false;
    };

    template <typename T>
    struct Huber
    {
        Huber( T _c ) : c( _c  ) {}

        T weight( T r ) const
        {
            T t = Math::abs( r );
            if ( t < c )
                return ( T )1;
            else
                return c / t;
        }

        void setThreshold( T thresh ){ c = thresh; }
        void setScale( T /*sigma*/ ){}

        T c;
    };

    template <typename T>
    struct Tukey
    {
        /**
         *	\brief	cut-off at threshold (0 influence of outliers!)
         */
        Tukey( T _c ) : c( _c ), s( 1 ) {}

        T weight( T r ) const
        {
            T rs = Math::abs( r / s );
            if ( rs > c )
                return (T)0;
            else
                return Math::sqr( 1 - Math::sqr( rs / c ) );
        }

        void setThreshold( T thresh ){ c = thresh; }
        void setScale( T sigma ){ s = sigma; }

        T c;
        T s;
    };

    /*
    template <typename T>
    struct Welsch
    {
        Welsch( T t ) : c( t ) {}

        T weight( T r ) const
        {
            return Math::exp( - Math::sqr( r / c ) );
        }

        void setSigma( T sigma ){ c = sigma; }

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

        void setSigma( T sigma ){ c = sigma; }

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

        void setSigma( T sigma ){}
    };

    template <typename T>
    struct Fair
    {
        Fair( T t ) : c( t ) {}

        T weight( T r ) const
        {
            return (T)1 / ( (T)1 + Math::abs( r ) / c );
        }

        void setSigma( T sigma ){ c = sigma; }

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

        void setSigma( T sigma ){}
    };

    template <typename T>
    struct L1
    {
        L1( T ) {}

        T weight( T r ) const
        {
            return (T)1 / Math::abs( r );
        }

        void setSigma( T sigma ){}
    };*/

}

#endif
