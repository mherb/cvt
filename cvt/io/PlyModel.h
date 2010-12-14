#ifndef CVT_PLYMODEL_H
#define CVT_PLYMODEL_H

#include <cvt/geom/Model.h>

namespace cvt {
	class PlyModel {
		public:
			static void load( Model& model, const char* file );
		private:
			PlyModel();
			PlyModel( const PlyModel& );
			~PlyModel();
	};
}

#endif
