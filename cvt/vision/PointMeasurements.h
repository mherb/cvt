/*
 *  PointMeasurements.h
 *  CVTools
 *
 *  Created by Sebastian Klose on 25.01.11.
 *  Copyright 2011. All rights reserved.
 *
 */

#ifndef CVT_POINTMEASUREMENTS_H
#define CVT_POINTMEASUREMENTS_H

#include<vector>

#define EIGEN_USE_NEW_STDVECTOR
#include <Eigen/StdVector>

namespace cvt {	
	
	template<typename T>
	class Point2dMeasurements
	{
		public:
			typedef Eigen::Matrix<T, 2, 1>	POINT2D;
			typedef POINT2D*				MeasIterator;
			typedef const POINT2D*			ConstMeasIterator;
			typedef size_t*					CamIterator;
			typedef const size_t*			ConstCamIterator;
			
			Point2dMeasurements(){}
			~Point2dMeasurements(){}
		
			void add( size_t camIndex, const POINT2D & p ) {
				_camIndex.push_back( camIndex );
				_points.push_back( p );
			}
		
			size_t size() const { return _camIndex.size(); }
		
			MeasIterator		measurementBegin() { return &_points[ 0 ]; }
			MeasIterator		measurementEnd() { return &_points[ _points.size() - 1 ]; }
			ConstMeasIterator	measurementBegin() const { return &_points[ 0 ]; }
			ConstMeasIterator	measurementEnd() const { return &_points[ _points.size() - 1 ]; }
			CamIterator			camIdsBegin() { return &_camIndex[ 0 ]; }
			CamIterator			camIdsEnd() { return &_camIndex[ _camIndex.size() - 1 ]; }
			ConstCamIterator	camIdsBegin() const { return &_camIndex[ 0 ]; }
			ConstCamIterator	camIdsEnd() const { return &_camIndex[ _camIndex.size() - 1 ]; }
					
		private:
			std::vector<size_t> _camIndex;
			std::vector<POINT2D> _points;
	};
	
	template<typename T> class PointMeasurements
	{		
		public:
			typedef Eigen::Matrix<T, 3, 1>	TYPE3D;
			typedef TYPE3D*					PointIterator;
			typedef const TYPE3D*			ConstPointIterator;
		
			PointMeasurements(){}
		
			~PointMeasurements()
			{}
		
			size_t add( const TYPE3D & p );
		
			Point2dMeasurements<T> & measurementsForId( size_t index );
			size_t size() const { return _points3d.size(); }
		
			TYPE3D & operator[]( size_t i ) { return _points3d[ i ]; }
			const TYPE3D & operator[]( size_t i ) const { return _points3d[ i ]; }
		
		private:
			std::vector<TYPE3D>						_points3d;
			std::vector<Point2dMeasurements<T> >	_measurements;
	};
	
	template<typename T>
	size_t PointMeasurements<T>::add( const TYPE3D & p )
	{
		_points3d.push_back( p );
		_measurements.push_back( Point2dMeasurements<T>() );
		return _measurements.size() - 1;
	}
	
	template<typename T>
	Point2dMeasurements<T> & PointMeasurements<T>::measurementsForId( size_t index )
	{
		return _measurements[ index ];
	}
	
	template<typename T>
	class PointCorrespondences3d2d
	{
		public:
			typedef Eigen::Matrix<T, 3, 1>	POINT3D;
			typedef Eigen::Matrix<T, 2, 1>	POINT2D;
		
			typedef const POINT3D*	ConstDataPtr;
			typedef const POINT2D*	ConstMeasPtr;
			typedef POINT3D*		DataPtr;
			typedef POINT2D*		MeasPtr;
		
			PointCorrespondences3d2d( size_t cameraIndex ) : _cam( cameraIndex )
			{
			}
		
			void add( const POINT3D & P, const POINT2D & p )
			{
				_points3d.push_back( P );
				_points2d.push_back( p );
			}
		
			size_t size() const { return _points3d.size(); }		
			size_t	cameraId() const { return _cam; }
		
			DataPtr			data() { return &_points3d[ 0 ]; }			
			MeasPtr			meas() { return &_points2d[ 0 ]; }
			ConstDataPtr	data() const { return &_points3d[ 0 ]; }
			ConstMeasPtr	meas() const { return &_points2d[ 0 ]; }
			
			const POINT3D & point3d( size_t index ) const { return _points3d[ index ]; }
			const POINT2D & point2d( size_t index ) const { return _points2d[ index ]; }
		
		private:
			size_t					_cam;
			std::vector<POINT3D>	_points3d;
			std::vector<POINT2D>	_points2d;
	};
	
	template<typename T>
	class PointCorrespondences3d3d
	{
	public:
		typedef Eigen::Matrix<T, 3, 1>	POINT3D;
		
		typedef const POINT3D*	ConstDataPtr;
		typedef const POINT3D*	ConstMeasPtr;
		typedef POINT3D*		DataPtr;
		typedef POINT3D*		MeasPtr;
		
		PointCorrespondences3d3d( size_t cameraIndex ) : _cam( cameraIndex )
		{
		}
		
		void add( const POINT3D & P, const POINT3D & meas )
		{
			_points3d.push_back( P );
			_meas.push_back( meas );
		}
		
		size_t size() const { return _points3d.size(); }		
		size_t	cameraId() const { return _cam; }
		
		DataPtr			data() { return &_points3d[ 0 ]; }			
		MeasPtr			meas() { return &_meas[ 0 ]; }
		ConstDataPtr	data() const { return &_points3d[ 0 ]; }
		ConstMeasPtr	meas() const { return &_meas[ 0 ]; }
		
		const POINT3D & point3d( size_t index ) const { return _points3d[ index ]; }
		const POINT3D & meas( size_t index ) const { return _meas[ index ]; }
		
	private:
		size_t					_cam;
		std::vector<POINT3D>	_points3d;
		std::vector<POINT3D>	_meas;
	};
}

#endif

