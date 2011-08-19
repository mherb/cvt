/*
 *  VisiblePoints.cpp
 *  CVTools
 *
 *  Created by Sebastian Klose on 12.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */

#include "VisiblePoints.h"

#include <cvt/io/FileSystem.h>
#include <cvt/util/Exception.h>

#include <Eigen/Geometry>

#include <fstream>
#include <sstream>

VisiblePoints::VisiblePoints(std::string fileName):
	xAngleRange(0, 0),
	yAngleRange(0, 0)
{
	if(!cvt::FileSystem::exists(fileName)){
		throw CVTException("File not found: \"" + fileName + "\"");
	}

	this->parse(fileName);
}

void VisiblePoints::parse(std::string fileName)
{
	std::ifstream in(fileName.c_str());

	in >> xAngleRange.min;
	in >> xAngleRange.max;
	in >> yAngleRange.min;
	in >> yAngleRange.max;


	unsigned int numXAngles;
	unsigned int numYAngles;
	in >> numXAngles;
	in >> numYAngles;

	std::cout << "XRange: " << xAngleRange;
	std::cout << "YRange: " << yAngleRange;
	std::cout << "NumXAngles: " << numXAngles << std::endl;
	std::cout << "NumYAngles: " << numYAngles << std::endl;

	viewInformations.resize(numXAngles);

	xAngleStep = (xAngleRange.max - xAngleRange.min) / numXAngles;
	yAngleStep = (yAngleRange.max - yAngleRange.min) / numYAngles;

	Eigen::Transform3d T(Eigen::AngleAxisd(-M_PI/2.0, Eigen::Vector3d::UnitX()));
	std::cout << T.linear() << std::endl;

	std::string line;

	unsigned int currentAlphaIdx, currentBetaIdx;
	unsigned int lineInFile = 1;
	std::getline(in, line);
	while (!in.eof()) {
		std::getline(in, line);
		lineInFile++;

		// skip empty line
		if(line.size() == 0)
			continue;

		std::istringstream lineParsing(line);

		lineParsing >> currentAlphaIdx;
		lineParsing >> currentBetaIdx;

		Eigen::Vector4d currentP;
		currentP[3] = 1.0;
		std::vector<Eigen::Vector4d> points;
		std::vector<Eigen::Vector4d> edges;

		while (!lineParsing.eof()) {
			// One 3D Point ...
			lineParsing >> currentP[0];
			lineParsing >> currentP[1];
			lineParsing >> currentP[2];
			points.push_back(currentP);

			// Corresponding 3D line  (start and endpoint)
			lineParsing >> currentP[0];
			lineParsing >> currentP[1];
			lineParsing >> currentP[2];
			edges.push_back(currentP);

			lineParsing >> currentP[0];
			lineParsing >> currentP[1];
			lineParsing >> currentP[2];
			edges.push_back(currentP);
		}

		unsigned int numPoints = (unsigned int)points.size();
		viewInformations[currentAlphaIdx].push_back(ViewInformation(numPoints));
		for(unsigned int p = 0; p < numPoints; p++){
			viewInformations[currentAlphaIdx][currentBetaIdx].points.block(0, p, 4, 1) = T.matrix()*points[p];
			viewInformations[currentAlphaIdx][currentBetaIdx].edges.block(0, 2*p, 4, 1) = T.matrix()*edges[2*p];
			viewInformations[currentAlphaIdx][currentBetaIdx].edges.block(0, 2*p+1, 4, 1) = T.matrix()*edges[2*p+1];
		}

	}
}

ViewInformation const& VisiblePoints::get( double alpha, double beta )
{
	alpha = (alpha*180.0/M_PI);
	beta = (beta*180.0/M_PI);

	if(alpha > xAngleRange.max)
		alpha = xAngleRange.max;
	else if( alpha < xAngleRange.min)
		alpha = xAngleRange.min;

	if(beta > yAngleRange.max)
		beta = yAngleRange.max;
	else if( beta < yAngleRange.min)
		beta = yAngleRange.min;

	unsigned int aIdx = viewInformations.size() * (alpha - xAngleRange.min) / xAngleRange.size();
	unsigned int bIdx = viewInformations[0].size() * (beta - yAngleRange.min) / yAngleRange.size();

	return viewInformations[aIdx][bIdx];
}
