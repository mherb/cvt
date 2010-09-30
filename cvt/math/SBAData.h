#ifndef CVT_SBADATA
#define CVT_SBADATA

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace cvt {
	
	struct Measurements
	{		
		Measurements(Eigen::Vector3d const& point,
					 std::vector<Eigen::Vector2d> & measurements,
					 std::vector<size_t> & views):
			pointParameters( point(0), point(1), point(2), 1.0 ),
			viewIds( views ),
			observations( measurements )
		{
			// TODO: init 
			// screen projections with current parameters
			// residuals
			// inverse covariances (set to identity)
		}
		
		~Measurements()
		{}
		
		/* 3d data for this point */
		Eigen::Vector4d pointParameters;
		
		/* ids for the views, corresponding to the observations */
		std::vector<size_t> viewIds;
		
		/* 2d image observations for the respective view */
		std::vector<Eigen::Vector2d> observations;
		
		/* 2d homogeneous point predictions, calculated using the camera model */
		std::vector<Eigen::Vector3d> screenPredictions;
		
		/* residuals for the respective view */
		std::vector<Eigen::Vector2d> residuals;
		Eigen::Vector3d accumulatedResiduals;
		
		/* Inverse Covariance Matrices for the projections in the different frames */
		std::vector<Eigen::Matrix2d> inverseCovariances;
		
		/* screenJacobians wrt to camera parameters */
		std::vector<Eigen::Matrix<double, 2, 6> > jacWRTCamParams;
		
		/* screenJacobians wrt to point parameters */
		std::vector<Eigen::Matrix<double, 2, 3> > jacWRTPointParams;
		
		void updateResidualForMeasurement( unsigned int idx )
		{
			residuals[ idx ]( 0 ) = observations[ idx ]( 0 ) - screenPredictions[ idx ]( 0 ) / screenPredictions[ idx ]( 2 );
			residuals[ idx ]( 1 ) = observations[ idx ]( 1 ) - screenPredictions[ idx ]( 1 ) / screenPredictions[ idx ]( 2 );
		}
	};
	
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
		virtual void project(const Eigen::Matrix<double, 4, 1> & p4, 
							 Eigen::Matrix<double, 3, 1> & p3) const
		{			
			p3 = projMat * p4;
		}
		
		const Eigen::Matrix<double, 3, 4> & projectionMatrix()
		{
			return projMat;
		}
		
		void screenJacobians( const Eigen::Vector4d & pointParams,
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
		}
		
	private:
		size_t id;
		
		Eigen::Matrix3d rotation;
		Eigen::Vector3d translation;
		
		Eigen::Matrix<double, 6, 1> params;
		
		Eigen::Matrix<double, 3, 3> intrinsic;
		Eigen::Matrix<double, 3, 4> projMat;		
	};
	
	
	struct SBAMeasurement
	{
		SBAMeasurement( ) :
			point3d( NULL )
		{			
		}
		
		Eigen::Vector4d * point3d;
		Eigen::Vector2d imagePoint;
		Eigen::Vector2d projection;
		Eigen::Vector2d residual;
		Eigen::Matrix2d inverseCovariance;
		Eigen::Matrix<double, 2, 6> screenJacobiansWRTCam;
		Eigen::Matrix<double, 2, 3> screenJacobiansWRTPoint;
	};
	
	struct KeyFrame
	{
		KeyFrame( const Eigen::Matrix3d & intrinsics, size_t id ) :
			camera( intrinsics, id )
		{			
		}
		
		// parameters for resp. the camera (location)
		CameraModel camera;
		std::vector<SBAMeasurement> measurements;
	};	
	
	
	class SBAData {
	public:			
		SBAData():
			currentCosts( 0.0 )
		{
		}
		
		~SBAData()
		{				
		}
		
		CameraModel & cameraWithId( size_t viewId )
		{
			return cameras[ viewId ];
		}
		
		CameraModel& addCamera( Eigen::Matrix<double, 3, 3> & intrinsic )
		{
			cameras.push_back( CameraModel( intrinsic, cameras.size() ) );
			
			return cameras.back();
		}
		
		Measurements & addMeasurements( Eigen::Vector3d const& point,
										std::vector<Eigen::Vector2d> & measurements,
									    std::vector<size_t> & views )
		{
			measurementData.push_back( Measurements( point, measurements, views ) );
			return measurementData.back();
		}
		
		std::vector<Measurements>& measurements()
		{
			return measurementData;
		}
		
		size_t numCams()
		{
			return cameras.size();
		}
		
		size_t numPoints()
		{
			return measurementData.size();
		}
		
		/**
		 *	apply computed delta parameters
		 *	@return the new calculated costs
		 */
		double applyDelta( Eigen::VectorXd & deltaCam, Eigen::VectorXd & deltaStruct )
		{
			currentCosts = 0.0;
			numObservations = 0;
			// apply the camera parameters:
			for( unsigned int i = 0; i < cameras.size(); i++ ){
				Eigen::Matrix<double, 6, 1> tmp = deltaCam.block( i * 6, 0, 6, 1 );
				cameras[ i ].update( tmp );
				cameras[ i ].updateProjectionMatrix();
			}
			
			for( unsigned int i = 0; i < measurementData.size(); i++ ){
				// apply the parameter
				measurementData[ i ].pointParameters.block( 0, 0, 3, 1 ) += deltaStruct.block( i*3, 0, 3, 1 );
				measurementData[ i ].pointParameters[ 3 ] = 1.0;
				
				Measurements & current = measurementData[ i ];
				// compute residuals
				for( unsigned int p = 0; p < current.observations.size(); p++ ){
					cameras[ current.viewIds[ p ] ].project( current.pointParameters, current.screenPredictions[ p ] );
					current.updateResidualForMeasurement( p );
					
					// Mahalanobis distance
					currentCosts += current.residuals[ p ].dot( current.inverseCovariances[ p ] * current.residuals[ p ] );
					numObservations++;
				}
			}
			
			currentCosts /= numObservations;
			return currentCosts;
		}
		
		void updateCurrentCosts()
		{	
			currentCosts = 0.0;
			numObservations = 0;
			for( unsigned int i = 0; i < measurementData.size(); i++ ){								
				Measurements & current = measurementData[ i ];
				// compute residuals
				for( unsigned int p = 0; p < current.observations.size(); p++ ){
					// Mahalanobis distance
					double tmp = current.residuals[ p ].dot( current.inverseCovariances[ p ] * current.residuals[ p ] );
					
					currentCosts += tmp;
					numObservations++;
				}
			}
			currentCosts /= numObservations;
		}
		
		void revertDelta( Eigen::VectorXd & deltaCam, Eigen::VectorXd & deltaStruct )
		{	
			// apply the camera parameters:
			Eigen::Matrix<double, 6, 1> tmp;
			for( unsigned int i = 0; i < cameras.size(); i++ ){
				tmp = deltaCam.block( i * 6, 0, 6, 1 );
				
				cameras[ i ].removeDelta( tmp );				
				cameras[ i ].updateProjectionMatrix();
			}
			
			for( unsigned int i = 0; i < measurementData.size(); i++ ){
				// apply the parameter
				measurementData[ i ].pointParameters.block( 0, 0, 3, 1 ) -= deltaStruct.block( i*3, 0, 3, 1 );
				measurementData[ i ].pointParameters[ 3 ] = 1.0;
				
				Measurements & current = measurementData[ i ];
				
				// compute residuals
				for( unsigned int p = 0; p < current.observations.size(); p++ ){
					cameras[ current.viewIds[ p ] ].project( current.pointParameters, current.screenPredictions[ p ] );
					current.updateResidualForMeasurement( p );					
				}
			}
		}
		
		double& costs()
		{
			return currentCosts;
		}
		
	private:		
		/* Motion parameters (per camera view) */
		std::vector<CameraModel> cameras;			
		
		/* Structure: 3d point, together with observation data */			
		std::vector<Measurements> measurementData;
		
		double currentCosts;
		size_t numObservations;
	};
}

#endif