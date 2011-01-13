#ifndef IFILTERPARAMETERSET_H
#define IFILTERPARAMETERSET_H
#include <cvt/gfx/IFilter.h>
#include <cvt/gfx/IFilterParameter.h>
#include <cvt/gfx/IFilterParameterInfo.h>
#include <map>


#include <iostream>

namespace cvt {

	class IFilterParameterSet
	{
		friend class IFilter;

		public:
			void setParameter( const std::string name, IFilterParameter* p )
			{
				std::map<const std::string, IFilterParameter*>::iterator it  = params.find( name );
				if( it == params.end() )
					return;
				it->second = p;
			}

			virtual ~IFilterParameterSet( ) {};

			IFilterParameter* getParameter( const std::string s ) const
			{
				std::map<const std::string, IFilterParameter*>::const_iterator it  = params.find( s );
				if( it == params.end() )
					return NULL;
				return it->second;
			};

			IFilterParameter* getParameter( size_t i ) const
			{
				if( i > pinfosize )
					return NULL;
				return getParameter( pinfo[ i ].getName() );
			};

			bool isValid() const
			{
				for( size_t i = 0; i < pinfosize; i++ ) {
					std::map<const std::string, IFilterParameter*>::const_iterator it  = params.find( pinfo[ i ].getName() );
					if( it == params.end() )
						return false;
					if( !it->second || (it->second)->getIFilterParameterType() != pinfo[ i ].getType() )
						return false;
				}
				return true;
			}

			void info( std::ostream& out ) const
			{
				std::map<const std::string, IFilterParameter*>::const_iterator it;
				for( it = params.begin(); it != params.end(); it++ )
					out << ( it->first ) << " : " << ( it->second ) << std::endl;
			}

		private:
			IFilterParameterSet( const IFilterParameterInfo* pi, size_t n ) : pinfo( pi ), pinfosize( n )
			{
				for( size_t i = 0; i < pinfosize; i++ )
					params[ pinfo[ i ].getName() ] = NULL;
			};
			IFilterParameterSet( );
			IFilterParameterSet( const IFilterParameterSet& pset );
			IFilterParameterSet& operator=( const IFilterParameterSet& pset );

			std::map<const std::string, IFilterParameter*> params;
			const IFilterParameterInfo* pinfo;
			size_t pinfosize;
	};
}
#endif
