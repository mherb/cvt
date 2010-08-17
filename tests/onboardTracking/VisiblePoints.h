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

typedef std::vector<Eigen::Vector4d> PointsHom;

class VisiblePoints
{
public:
	VisiblePoints(std::string file);

	void visiblityInfoForViewAngles(double alpha,
									double beta,
									PointsHom & point,
									PointsHom & edges);


private:
	cvt::Range<double> xAngleRange;
	cvt::Range<double> yAngleRange;

	double xAngleStep;
	double yAngleStep;

	std::vector<std::vector<PointsHom> > visiblePointsForAngles;
	std::vector<std::vector<PointsHom> > linesForPoints;

	void parse(std::string fileName);
};

#endif
