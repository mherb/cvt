#ifndef CLFILTER_H
#define CLFILTER_H
#include <cvt/cl/CLManager.h>
#include <cvt/cl/CLParameterInfo.h>
#include <cvt/cl/CLParameterSet.h>
#include <string>

namespace cvt {

		class CLManager;
		class CLParameterSet;

		class CLFilter
		{
			friend std::ostream& operator<<( std::ostream &os, const CLFilter &f );

			public:
			std::string getName( ) const { return name; };
			CLParameterSet* getParameterSet( void );

			virtual bool isExecuteable( ) = 0;
			virtual void apply( CLParameterSet* set ) = 0;
			void apply( CLParameterSet* set, double& t );

			virtual ~CLFilter( ) {};

			private:
			CLFilter( const CLFilter& ) {};
			CLFilter( ) {};
			std::ostream& print( std::ostream& os ) const;

			protected:
			CLFilter( CLManager* cl, std::string name, const CLParameterInfo* params, size_t paramssize );
			void buildKernel( ::cl::Kernel& kernel, const char* name, const char* src, size_t len, std::string& buildinfo );

			void runKernel( const ::cl::Kernel& kernel, const ::cl::NDRange& offset, const ::cl::NDRange& global, const ::cl::NDRange& local, std::vector< ::cl::Event>* events = NULL, ::cl::Event* event = NULL );

			CLManager* cl;
			std::string name;
			const CLParameterInfo* pinfo;
			size_t pinfosize;

		};

		struct CLFilterPlugin {
			size_t ID;
			CLFilter* ( * getFilter )( CLManager* cl );
		};

		std::ostream& operator<<( std::ostream &out, const CLFilter &f );
}
#endif
