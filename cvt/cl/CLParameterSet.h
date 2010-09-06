#ifndef CLPARAMETERSET_H
#define CLPARAMETERSET_H
#include <cvt/cl/CLParameter.h>
#include <cvt/cl/CLParameterInfo.h>
#include <map>

namespace cvt {
	namespace cl {

		class CLParameterSet
		{
			friend class CLFilter;

			public:
			void setParameter( const std::string name, CLParameter* p );

			~CLParameterSet( );
			CLParameter* getParameter( const std::string s )
			{
				return params[ s ];
			};

			private:
			CLParameterSet( const CLParameterInfo* pi, size_t n );
			CLParameterSet( ) {};
			CLParameterSet( const CLParameterSet& pset ) {};

			std::map<std::string, CLParameter*> params;
			const CLParameterInfo* pinfo;
			size_t pinfosize;
		};
	}
}
#endif
