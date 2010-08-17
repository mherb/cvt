/*
 *  VisiblePoints.cpp
 *  CVTools
 *
 *  Created by Sebastian Klose on 12.08.10.
 *  Copyright 2010. All rights reserved.
 *
 */

#include "VisiblePoints.h"

#include <fstream>
#include <sstream>

VisiblePoints::VisiblePoints(std::string fileName)
{	
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
	
	visiblePointsForAngles.resize(numXAngles);
	linesForPoints.resize(numXAngles);
	for (unsigned int i = 0; i < numXAngles; i++) {
		visiblePointsForAngles[i].resize(numYAngles);
		linesForPoints[i].resize(numYAngles);
	}
	
	xAngleStep = (xAngleRange.max - xAngleRange.min) / numXAngles;
	yAngleStep = (yAngleRange.max - yAngleRange.min) / numYAngles;
	
	std::string line;
	
	unsigned int currentAlphaIdx, currentBetaIdx;
	unsigned int lineInFile = 0;
	while (!in.eof()) {
		std::getline(in, line);
		lineInFile++;
		
		std::istringstream lineParsing(line);
		
		lineParsing >> currentAlphaIdx;
		lineParsing >> currentBetaIdx;
		
		Eigen::Vector4d currentP;
		currentP[3] = 1.0;
		
		while (!lineParsing.eof()) {			
			lineParsing >> currentP[0];
			lineParsing >> currentP[1];
			lineParsing >> currentP[2];
			
			visiblePointsForAngles[currentAlphaIdx][currentBetaIdx].push_back(currentP);
			
			lineParsing >> currentP[0];
			lineParsing >> currentP[1];
			lineParsing >> currentP[2];
			
			linesForPoints[currentAlphaIdx][currentBetaIdx].push_back(currentP);
			
			lineParsing >> currentP[0];
			lineParsing >> currentP[1];
			lineParsing >> currentP[2];
			
			linesForPoints[currentAlphaIdx][currentBetaIdx].push_back(currentP);

		}				
	}	
}

void VisiblePoints::visiblityInfoForViewAngles(double alpha, double beta,
											   PointsHom & points,
											   PointsHom & edges)
{
	if(alpha > xAngleRange.max)
		alpha = xAngleRange.max;
	else if( alpha < xAngleRange.min)
		alpha = xAngleRange.min;
	
	if(beta > yAngleRange.max)
		beta = yAngleRange.max;
	else if( beta < yAngleRange.min)
		beta = yAngleRange.min;
		
	unsigned int aIdx = static_cast<unsigned int>(alpha / xAngleStep);
	unsigned int bIdx = static_cast<unsigned int>(beta / yAngleStep);
	
	points = visiblePointsForAngles[aIdx][bIdx];
	edges = linesForPoints[aIdx][bIdx];	
}
