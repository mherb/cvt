#include "FeatureAugmentation.h"

#include <set>

namespace cvt {

    FeatureAugmentation::FeatureAugmentation( const Matrix3f& intrinsics ) :
        _intrinsics( intrinsics ),
        _nextId( 0 ),
        _fastDetector(),
        _pyramid( 3, 0.5f ),
        _featureTracking( 3, 0.5f )
    {
        _fastDetector.setBorder( 16 );
        _fastDetector.setNonMaxSuppress( true );
        _fastDetector.setThreshold( 20 );
    }

    FeatureAugmentation::~FeatureAugmentation()
    {
    }

    void FeatureAugmentation::trackAndTriangulate( std::vector<Vector3f>& newPoints,
                                                   const Image& img,
                                                   const Image& depth,
                                                   const Matrix4f& pose3d )
    {
        // track current features of the tracks
        trackFeatures( img );

        // depth initialize tracks with sufficient length or confidence

        // detect new features and initialize new tracks
    }

    void FeatureAugmentation::trackFeatures( const Image& img, const Matrix4f& pose )
    {
        // create vector of current features for tracking:
        MapType::const_iterator it = _featureTracks.begin();
        const MapType::const_iterator end = _featureTracks.end();

        std::vector<size_t>     ids;
        std::vector<Vector2f>   positions;
        ids.reserve( _featureTracks.size() );
        positions.reserve( _featureTracks.size() );

        std::set<size_t> idSet;

        while( it != end ){
            ids.push_back( it->first );
            idSet.insert( it->first );
            positions.push_back( it->second.positions.back() );
            it++;
        }

        std::vector<size_t> trackedIds;
        PointSet2f  trackedPositions;
        _featureTracking.trackFeatures( trackedPositions, trackedIds, positions, ids, img );

        Matrix4f projMat;

        // update feature tracks of successfully tracked features
        for( size_t i = 0; i < trackedIds.size(); i++ ){
            const Vector2f& p = trackedPositions[ i ];

            MapType::iterator track = _featureTracks.find( trackedIds[ i ] );

            track->second.positions.push_back( p );
            track->second.projectionMat.push_back( projMat );
            idSet.erase( trackedIds[ i ] );
        }

        // remove tracks of lost features (remaining ids in idset)
        std::set<size_t>::const_iterator setIter;
        const std::set<size_t>::const_iterator setEnd;
        while( setIter != setEnd ){
            _featureTracks.erase( *setIter );
            setIter++;
        }

    }



}
