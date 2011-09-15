#include "EdgeMatcher.h"

#include <cv.h>

namespace cvt {

	EdgeMatcher::EdgeMatcher():
		normalSearchLength(20)
	{
	}

	EdgeMatcher::~EdgeMatcher()
	{
	}

	void EdgeMatcher::findMatchingPoints(cvt::Image const& img,
										 Eigen::Matrix<double, 3, Eigen::Dynamic> const& screenCoordsH,
										 Eigen::Matrix<double, 2, Eigen::Dynamic> const& screenNormals,
										 Eigen::Matrix<double, 2, Eigen::Dynamic> & matchedPoints,
										 std::vector<bool> & validityFlags)
	{
		Eigen::Vector2f sc, sn, match;
		
		validityFlags.clear();
		validityFlags.resize(screenCoordsH.cols());

		for(int i = 0; i < screenCoordsH.cols(); i++){
			sc[0] = (float)screenCoordsH( 0, i ) / screenCoordsH( 2, i );
			sc[1] = (float)screenCoordsH( 1, i ) / screenCoordsH( 2, i );

			sn[0] = (float)screenNormals(0, i);
			sn[1] = (float)screenNormals(1, i);

			if( this->normalSearch(img, sn, sc, match) ){
				validityFlags[i] = true;
				matchedPoints(0, i) = match[0];
				matchedPoints(1, i) = match[1];
			} else {
				validityFlags[i] = false;
			}
		}
	}

	bool EdgeMatcher::normalSearch(cvt::Image const& img,
								   Eigen::Vector2f const & sn,
								   Eigen::Vector2f const& sc,
								   Eigen::Vector2f & match)
	{
		Eigen::Vector2f left = sc;
		Eigen::Vector2f right = sc;

		unsigned int currIter = 0;

		while(currIter <= normalSearchLength){
			right-=sn;
			
			cvt::Color c = img( right[0], right[1] );
			if( matchFunc( c ) ){
				match = right;
				return true;
			}

			left+=sn;
			c = img( left[0], left[1] );
			if( matchFunc( c ) ){
				match = left;
				return true;
			}

			currIter++;
		}

		return false;
	}

	bool EdgeMatcher::matchFunc(cvt::Color & color)
	{
		// for Canny: value > 0.5 -> match
		return (color.gray() > 0.5 );
	}

}
