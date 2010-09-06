#ifndef IFILTERPARAMETERSET_H
#define IFILTERPARAMETERSET_H
#include <cvt/gfx/IFilter.h>
#include <cvt/gfx/IFilterParameter.h>
#include <cvt/gfx/IFilterParameterInfo.h>
#include <map>

namespace cvt {

	class IFilterParameterSet
	{
		friend class IFilter;

		public:
			void setParameter( const std::string name, IFilterParameter* p );

			~IFilterParameterSet( );
			IFilterParameter* getParameter( const std::string s )
			{
				return params[ s ];
			};

		private:
			IFilterParameterSet( const IFilterParameterInfo* pi, size_t n ) {};
			IFilterParameterSet( ) {};
			IFilterParameterSet( const IFilterParameterSet& pset ) {};

			std::map<std::string, IFilterParameter*> params;
			const IFilterParameterInfo* pinfo;
			size_t pinfosize;
	};
}
#endif
