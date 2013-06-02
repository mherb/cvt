#ifndef CVT_RDFTEST_H
#define CVT_RDFTEST_H

namespace cvt {

	template<typename DATA>
	class RDFTest
	{
		public:
			RDFTest() {}
			virtual ~RDFTest() {}

			virtual bool operator()( const DATA& d ) = 0;
	};
}

#endif
