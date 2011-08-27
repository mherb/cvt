#ifndef CVT_EDGE_MATCHER_H
#define CVT_EDGE_MATCHER_H

#include <Eigen/Core>

#include <cvt/gfx/Image.h>
#include <cvt/gfx/Color.h>

namespace cvt {

	class EdgeMatcher
	{
		public:
			EdgeMatcher();
			virtual ~EdgeMatcher();

			/*
			 *	Find matching image coordinates for incoming projections
			 *	@param	screenCoordsH	2d homogeneous image coords to find matches for
			 *	@param	screenNormals	the normals at the resp. screenpoint, defining the search dir.
			 *	@param	matchedPoints	the found points
			 *	@param	validityFlags	indicating if the resp. entry in matchedPoints is a valid match
			 */
			void findMatchingPoints(cvt::Image const & edgeImage,
									Eigen::Matrix<double, 3, Eigen::Dynamic> const& screenCoordsH,
									Eigen::Matrix<double, 2, Eigen::Dynamic> const& screenNormals,
									Eigen::Matrix<double, 2, Eigen::Dynamic> & matchedPoints,
									std::vector<bool> & validityFlags);

		private:
			unsigned int normalSearchLength;

			bool normalSearch(cvt::Image const& img,
							  Eigen::Vector2f const & sn,
							  Eigen::Vector2f const& sc,
							  Eigen::Vector2f & match);

		bool matchFunc(cvt::Color & color);
	};

}

#endif
