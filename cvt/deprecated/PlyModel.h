/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_PLYMODEL_H
#define CVT_PLYMODEL_H

#include <cvt/geom/Model.h>

namespace cvt {
	class PlyModel {
		public:
			static void load( Model& model, const char* file );
			static void save( const char* file, const Model& model );
		private:
			PlyModel();
			PlyModel( const PlyModel& );
			~PlyModel();
	};
}

#endif
