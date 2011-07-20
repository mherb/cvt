#ifndef CVT_SBADATA
#define CVT_SBADATA

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <iostream>

namespace cvt {
	
	class CameraModel 
	{
	public:
		CameraModel( const Eigen::Matrix<double, 3, 3> & intr, size_t id ):
			id( id ),
			params( Eigen::Matrix<double, 6, 1>::Zero() ),
			intrinsic( intr )
		{			
		}
		
		virtual ~CameraModel()
		{			
		}		
		
		void setIntrinsic( Eigen::Matrix<double, 3, 3> & K )
		{
			intrinsic = K;
		}
		
		void set( Eigen::Quaterniond & rot, Eigen::Vector3d & trans )
		{
			rotation = rot.toRotationMatrix();
			translation = trans;
		}
		
		void set( Eigen::Matrix3d & rot, Eigen::Vector3d & trans )
		{
			rotation = rot;
			translation = trans;
		}
		
		void update( Eigen::Matrix<double, 6, 1> & delta )
		{	
			
			// apply delta rotation
			rotation *= (Eigen::AngleAxisd( delta(0), Eigen::Vector3d::UnitX() ) *
						Eigen::AngleAxisd( delta(1), Eigen::Vector3d::UnitY() ) *
						Eigen::AngleAxisd( delta(2), Eigen::Vector3d::UnitZ() ) ).toRotationMatrix();
			
			translation += delta.block( 3, 0, 3, 1); 
		}
		
		void removeDelta( Eigen::Matrix<double, 6, 1> & delta )
		{
			// apply inverse delta rotation
			rotation *= (Eigen::AngleAxisd( delta(0), Eigen::Vector3d::UnitX() ) *
						 Eigen::AngleAxisd( delta(1), Eigen::Vector3d::UnitY() ) *
						 Eigen::AngleAxisd( delta(2), Eigen::Vector3d::UnitZ() ) ).toRotationMatrix().transpose();
			
			translation -= delta.block( 3, 0, 3, 1); 
		}
		
		void updateProjectionMatrix()
		{			
			projMat.block( 0, 0, 3, 3 ) = intrinsic * rotation;
			projMat.block( 0, 3, 3, 1 ) = intrinsic * translation;
		}
		
		/* project a 3d Homogeneous point to 2d homogeneous point, according to  */
		virtual void project(const Eigen::Vector4d & p4, 
							 Eigen::Vector3d & p3) const
		{			
			p3 = projMat * p4;
		}
		
		virtual void project(const Eigen::Vector4d & p4, 
							 Eigen::Vector2d & p) const
		{			
			Eigen::Vector3d p3;
			p3 = projMat * p4;
			
			double dehom = 1.0 / p3( 2 );
			
			p( 0 ) = dehom * p3( 0 );
			p( 1 ) = dehom * p3( 1 );
		}
		
		const Eigen::Matrix<double, 3, 4> & projectionMatrix()
		{
			return projMat;
		}
		
		const Eigen::Matrix3d & K() const
		{
			return intrinsic;
		}
		
		const Eigen::Matrix3d & R() const
		{
			return rotation;
		}
		
		const Eigen::Vector3d & t() const
		{
			return translation;
		}
		
		/*void screenJacobians( const Eigen::Vector4d & pointParams,
							  const Eigen::Vector3d & p3,
							  Eigen::Matrix<double, 2, 6> & jacobians )
		{
			Eigen::Matrix<double, 6, 1> currParams( Eigen::Matrix<double, 6, 1>::Zero() );
			Eigen::Vector3d currProjection;
			
			const double currentDelta = 0.001;
			
			for( unsigned int i = 0; i < 6; i++ ){
				currParams( i ) = currentDelta;
				
				this->update( currParams );
				this->updateProjectionMatrix( );
				
				this->project( pointParams, currProjection );
				
				jacobians( 0, i ) = ( currProjection( 0 ) / currProjection( 2 ) - p3( 0 ) / p3( 2 ) ) / currentDelta;
				jacobians( 1, i ) = ( currProjection( 1 ) / currProjection( 2 ) - p3( 1 ) / p3( 2 ) ) / currentDelta;
				
				this->removeDelta( currParams );
				currParams( i ) = 0.0;
			}
			this->updateProjectionMatrix();
		}*/
		
		size_t id;
	private:
		
		Eigen::Matrix3d rotation;
		Eigen::Vector3d translation;
		
		Eigen::Matrix<double, 6, 1> params;
		
		Eigen::Matrix3d intrinsic;
		Eigen::Matrix<double, 3, 4> projMat;		
	};
	
			
	template <class T>
	struct Feature
	{
		Feature( T *& f, size_t id ):
		 data( f ),
		 id( id )
		{		
		}
		
		~Feature()
		{	
		}
		
		T * data;
		size_t id;
	};
	
	struct SBAMeasurement
	{
		SBAMeasurement( ) :
			point3d( NULL ),
			costs( 0.0 ) 
		{		
			inverseCovariance.setIdentity();
		}
		
		void updateResidual()
		{
			residual = imagePoint - projection;
		}
		
		void updateCosts()
		{	
			costs = residual.dot( inverseCovariance * residual );
		}
		
		Feature<Eigen::Vector4d> * point3d;
		Eigen::Vector2d imagePoint;
		Eigen::Vector2d projection;
		Eigen::Vector2d residual;
		Eigen::Matrix2d inverseCovariance;
		double costs;
		
		Eigen::Matrix<double, 2, 6> screenJacobiansWRTCam;
		Eigen::Matrix<double, 2, 3> screenJacobiansWRTPoint;
	};
	
	struct KeyFrame
	{
		KeyFrame( const Eigen::Matrix3d & intrinsics, size_t id ) :
			camera( intrinsics, id ),
			costs(0.0)
		{			
		}
		
		// update the measurement structure (reproject, ...)
		void update()
		{
			costs = 0.0;
			for( unsigned int i = 0; i < measurements.size(); i++ ){
				camera.project( *(measurements[ i ].point3d->data), measurements[ i ].projection );
				
				// residual update
				measurements[ i ].updateResidual();
				
				// update the costs
				measurements[ i ].updateCosts();
				
				costs += measurements[ i ].costs; 
			}
		}
		
		void cameraJacobians( )
		{
			Eigen::Matrix<double, 6, 1> currParams( Eigen::Matrix<double, 6, 1>::Zero() );
			Eigen::Vector2d currProjection;
			Eigen::Vector2d tmp;
			
			const double delta = 0.00001;
			const double deltaInv = 1.0 / delta;
			
			std::vector<SBAMeasurement>::iterator m;
			
			// screen jacobians wrt to camera parameters for each measurement of this camera
			for( unsigned int i = 0; i < 6; i++ ){							
				currParams( i ) = delta;			
				
				camera.update( currParams );
				camera.updateProjectionMatrix( );
				
				for( m = measurements.begin(); m != measurements.end(); ++m ){
					//std::cout << "Keyframe " << camera.id << std::endl;
					//std::cout << "3D Point:\n" << *(m->point3d->data) << std::endl;
					camera.project( *(m->point3d->data), currProjection );
					//std::cout << "Proj.:\n" << currProjection << std::endl;

					m->screenJacobiansWRTCam.block( 0, i, 2, 1 ) =  deltaInv * ( currProjection - m->projection );
					//std::cout << "Jac wrt param " << i << ": " << m->screenJacobiansWRTCam( 0, i ) << ", " << m->screenJacobiansWRTCam( 1, i ) << std::endl;
				}
				
				camera.removeDelta( currParams );
				currParams( i ) = 0.0;
			}
			camera.updateProjectionMatrix();
		}
		
		/* screen jacobians wrt point parameters (analytical) */
		void pointJacobians( const Eigen::Matrix3d & K, 
							 const Eigen::Matrix3d & rot, 
							 const Eigen::Vector3d & t, 
							 const Eigen::Vector4d & , 
							 Eigen::Matrix<double, 2, 3> & J )
		{
			Eigen::Vector3d tmp = t.block( 0, 0, 3, 1 );
			
			Eigen::Vector3d transformed = rot * tmp + t;
			
			double denom = transformed.z();
			if( fabs(denom) < 1e-9 )
				denom += 1e-9;
			
			double denomInv = 1.0 / denom;
			double denomSqrInv = denomInv*denomInv;
			
			double fx = K( 0, 0 );
			double fy = K( 1, 1 );
			
			double txDenom = transformed.x() * denomSqrInv;
			double tyDenom = transformed.y() * denomSqrInv;
			
			// d/dX
			J( 0, 0 ) = fx * ( rot( 0, 0 ) * denomInv -  rot( 2, 0 ) * txDenom );
			J( 1, 0 ) = fy * ( rot( 1, 0 ) * denomInv -  rot( 2, 0 ) * tyDenom );
			
			// d/dY
			J( 0, 1 ) = fx * ( rot( 0, 1 ) * denomInv -  rot( 2, 1 ) * txDenom );
			J( 1, 1 ) = fy * ( rot( 1, 1 ) * denomInv -  rot( 2, 1 ) * tyDenom );
			
			// d/dZ
			J( 0, 2 ) = fx * ( rot( 0, 2 ) * denomInv -  rot( 2, 2 ) * txDenom );
			J( 1, 2 ) = fy * ( rot( 1, 2 ) * denomInv -  rot( 2, 2 ) * tyDenom );
			
			if( std::isnan( J( 0, 0) ) || std::isnan( J( 0, 1) ) || std::isnan( J( 0, 2) ) ||
			   std::isnan( J( 1, 0) ) || std::isnan( J( 1, 1) ) || std::isnan( J( 1, 2) ) ){
				std::cout << "Jacobian: \n" << J << "\n" << std::endl;
				std::cout << "Denominator: " << denom << std::endl;
			}
		}
		
		void pointJacobians( const Eigen::Vector4d & point, 
							 const Eigen::Vector2d & proj,
							 Eigen::Matrix<double, 2, 3> & J )
		{
			const double delta = 0.0001;
			const double deltaInv = 1.0 / delta;
			
			Eigen::Vector4d currentParams = point;
			Eigen::Vector2d currentProj;
			
			currentParams( 0 ) += delta;
			camera.project( currentParams, currentProj );
			J.block( 0, 0, 2, 1 ) =  deltaInv * ( currentProj - proj );
			currentParams( 0 ) -= delta;
			
			currentParams( 1 ) += delta;
			camera.project( currentParams, currentProj );
			J.block( 0, 1, 2, 1 ) =  deltaInv * ( currentProj - proj );
			currentParams( 1 ) -= delta;
			
			currentParams( 2 ) += delta;
			camera.project( currentParams, currentProj );
			J.block( 0, 2, 2, 1 ) =  deltaInv * ( currentProj - proj );
			
		}
		
		void addMeasurement( Eigen::Vector4d *& point, size_t id, Eigen::Vector2d & observation)
		{
			measurements.push_back( SBAMeasurement() );
			
			measurements.back().point3d = new Feature<Eigen::Vector4d>( point, id );
			
			measurements.back().imagePoint = observation;
			
			camera.project( *point, measurements.back().projection );
			
			measurements.back().updateResidual();
			measurements.back().updateCosts();
			
			/*std::cout << "Image Point: " << measurements.back().imagePoint << std::endl;
			std::cout << "3D Point: " << measurements.back().point3d->data << std::endl;			
			std::cout << "Project: " << measurements.back().projection << std::endl;			
			std::cout << "Project: " << measurements.back().residual << std::endl;			
			std::cout << "Costs: " << measurements.back().costs << std::endl;*/
			costs += measurements.back().costs;
		}
		
		// parameters for resp. the camera (location)
		CameraModel camera;
		std::vector<SBAMeasurement> measurements;		
		double costs;
	};	
	
	
	class SBAData {
	public:			
		SBAData():
			currentCosts( 0.0 ),
			summedCosts( 0.0 ),
			numObservations( 0 ),
			fillSize( 0 )
		{
		}
		
		~SBAData()
		{				
			for( unsigned int i = 0; i < keyFrames.size(); i++ ){
				delete keyFrames[ i ];
			}
		}
		
		CameraModel & cameraWithId( size_t viewId )
		{
			return keyFrames[ viewId ]->camera;
		}
		
		const Eigen::Vector4d & pointWithId( size_t pId )
		{
			return *points3dHom[ pId ];
		}
	
		void normalizeCosts()
		{
			currentCosts = summedCosts/numObservations;
		}
				
		std::vector<KeyFrame*>& frames()
		{
			return keyFrames;
		}
		
		size_t numCams()
		{
			return keyFrames.size();
		}
		
		size_t numPoints()
		{
			return points3dHom.size();
		}
		
		/**
		 *	apply computed delta parameters
		 *	@return the new calculated costs
		 */
		double applyDelta( Eigen::VectorXd & deltaCam, Eigen::VectorXd & deltaStruct )
		{
			currentCosts = 0.0;
			numObservations = 0;
			
			// add the delta to the points:
			for( unsigned int p = 0; p < points3dHom.size(); p++ ){
				(*points3dHom[ p ])( 0 ) += deltaStruct( p*3 );
				(*points3dHom[ p ])( 1 ) += deltaStruct( p*3 +1 );
				(*points3dHom[ p ])( 2 ) += deltaStruct( p*3 +2 );
			}
			
			// for each key frame: apply delta, and reproject the points
			Eigen::Matrix<double, 6, 1> tmp;
			for( unsigned int k = 0; k < keyFrames.size(); k++){
				tmp = deltaCam.block( k * 6, 0, 6, 1 );
				keyFrames[ k ]->camera.update( tmp );
				keyFrames[ k ]->camera.updateProjectionMatrix();
				
				keyFrames[ k ]->update();
				
				currentCosts += keyFrames[ k ]->costs;
				numObservations += keyFrames[ k ]->measurements.size();
			}			
			summedCosts = currentCosts;
			currentCosts /= numObservations;
			return currentCosts;
		}
		
		void revertDelta( Eigen::VectorXd & deltaCam, Eigen::VectorXd & deltaStruct )
		{	
			currentCosts = 0.0;
			numObservations = 0;
			
			// add the delta to the points:
			for( unsigned int p = 0; p < points3dHom.size(); p++ ){
				(*points3dHom[ p ])( 0 ) -= deltaStruct( p*3 );
				(*points3dHom[ p ])( 1 ) -= deltaStruct( p*3 +1 );
				(*points3dHom[ p ])( 2 ) -= deltaStruct( p*3 +2 );
			}
			
			// for each key frame: apply delta, and reproject the points
			Eigen::Matrix<double, 6, 1> tmp;
			for( unsigned int k = 0; k < keyFrames.size(); k++){
				tmp = deltaCam.block( k * 6, 0, 6, 1 );
				keyFrames[ k ]->camera.removeDelta( tmp );
				keyFrames[ k ]->camera.updateProjectionMatrix();
				
				keyFrames[ k ]->update();				
			}			
		}
		
		double & costs()
		{
			return currentCosts;
		}
		
		void updateCosts()
		{
			summedCosts = 0.0;
			numObservations = 0;
			for (unsigned int i = 0; i < keyFrames.size(); i++) {
				summedCosts += keyFrames[ i ]->costs;
				numObservations += keyFrames[ i ]->measurements.size();
			}
			currentCosts = summedCosts / numObservations;
		}
		
		void addKeyFrame( KeyFrame * kf )
		{
			keyFrames.push_back( kf );
			summedCosts += kf->costs;
			numObservations += kf->measurements.size();
			
			// keep track in here of common point tracks:
			viewCorrespondences.push_back( std::set<size_t>() );
			viewCorrespondences.back().insert( kf->camera.id );
			fillSize++;
		}
		
		/* add a new point, return a pointer to it, and store its identifier in id */
		Eigen::Vector4d * addPoint( const Eigen::Vector4d & p, size_t & identifier )
		{
			identifier = points3dHom.size();
			points3dHom.push_back( new Eigen::Vector4d( p ) );
			
			// initialize viewIds
			viewIdsForPoint.push_back( std::set<size_t>() );
			
			return points3dHom[ identifier ];
		}
		
		/* observation of pointId in viewId has been made */
		void addFrameIdForPoint( size_t pointId, size_t viewId ) 
		{
			// add the view id to the set if frames observing the point
			viewIdsForPoint[ pointId ].insert( viewId );
			
			updateCorrespondences( pointId, viewId );
		}
		
		/* when a point is observed in another view, the point tracks change:
		 * all other views, observing this 3d point need to be updated
		 */
		void updateCorrespondences( size_t pointId, size_t viewId )
		{
			std::set<size_t>::const_iterator view2 = viewIdsForPoint[ pointId ].begin();
			
			std::pair<std::set<size_t>::iterator, bool> ret;
			
			while ( view2 != viewIdsForPoint[ pointId ].end() ) {
								
				ret = (viewCorrespondences[ viewId ].insert( *view2 ));
				
				if( ret.second )
					fillSize++;
	
				ret = (viewCorrespondences[ *view2 ].insert( viewId ));
				
				if( ret.second )
					fillSize++;
				view2++;
			}
		}
		
		std::set<size_t> const & correspondenceSet( size_t view ) const
		{
			return viewCorrespondences[ view ];
		}
		
		size_t numNonZero()
		{
			return fillSize;
		}
		
		void printPoints()
		{
			for( unsigned int i = 0; i < points3dHom.size(); i++ ){
				std::cout << "Point " << i
						  << ": [ " 
						  << (*points3dHom[ i ])( 0 ) << ", "
						  << (*points3dHom[ i ])( 1 ) << ", "
						  << (*points3dHom[ i ])( 2 ) << ", "
						  << (*points3dHom[ i ])( 3 ) << "]" << std::endl;
			}
		}
		
	private:		
		/* Motion parameters (per camera view) */
		std::vector<Eigen::Vector4d*> points3dHom;
		/* for each point, store the camera ids of the cameras, observing it */
		std::vector< std::set<size_t> > viewIdsForPoint;
		
		/* Common point tracks: for each frame, those frames, which see at least one common point */
		std::vector< std::set<size_t> > viewCorrespondences;
		
		std::vector<KeyFrame *> keyFrames;
		
		double currentCosts;
		double summedCosts;
		size_t numObservations;
		size_t fillSize;
	};
	
}

#endif
