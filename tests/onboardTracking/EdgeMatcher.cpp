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
		Eigen::Vector2d sc, sn, match;

		for(int i = 0; i < screenCoordsH.cols(); i++){
			sc[0] = screenCoordsH( 0, i ) / screenCoordsH( 2, i );
			sc[1] = screenCoordsH( 1, i ) / screenCoordsH( 2, i );

			sn[0] = screenNormals(0, i);
			sn[1] = screenNormals(1, i);

			if( this->normalSearch(img, sn, sc, match) ){

			}

		}
	}

	bool EdgeMatcher::normalSearch(cvt::Image const& img,
								   Eigen::Vector2d const & sn,
								   Eigen::Vector2d const& sc,
								   Eigen::Vector2d & match)
	{
		Eigen::Vector2d left = sc;
		Eigen::Vector2d right = sc;

		float value = 0.0;

		unsigned int currIter = 0;
		while(currIter <= normalSearchLength){
			right-=sn;

			if(matchFunc(&value)){
				match = right;
				return true;
			}

			left+=sn;
			if(matchFunc(&value)){
				match = left;
				return true;
			}

			currIter++;
		}

		return false;
	}

	bool EdgeMatcher::matchFunc(float* value)
	{
		// for Canny: value > 0 -> match
		return (*value > 0.0 );
	}

}
