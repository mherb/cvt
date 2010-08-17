/*
 *  VisiblePoints.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 12.08.10.
 *  Copyright 2010. All rights reserved.
 */

#ifndef VISIBLE_POINTS_H
#define VISIBLE_POINTS_H

#include <cvt/util/Range.h>

#include <Eigen/Core>

#include <vector>

struct ViewInformation
{
	ViewInformation(unsigned int numPoints)
	{
		points.setZero(4, numPoints);
		edges.setZero(4, 2*numPoints);
	}

	Eigen::Matrix<double, 4, Eigen::Dynamic> points;
	Eigen::Matrix<double, 4, Eigen::Dynamic> edges;
};

class VisiblePoints
{
public:
	VisiblePoints(std::string file);

	/**
	  *	get the visible points and corresponding edges
	  *	for a specific viewing angle in camera coordinates
	  *
	  *	@param alpha	rotation around x axis in camera frame in RAD!
	  *	@param beta		rotation around y axis in camera frame in RAD!
	  *	@return			ViewInformation for given angles (stored in matrices column wise)
	  *
	  *	@note	The results still have to be transformed according
	  *			to the z-Rotation + translations
	  */
	ViewInformation const& get(double alpha, double beta);

private:
	cvt::Range<double> xAngleRange;
	cvt::Range<double> yAngleRange;

	double xAngleStep;
	double yAngleStep;

	std::vector<std::vector<ViewInformation> > viewInformations;

	void parse(std::string fileName);
};

#endif
