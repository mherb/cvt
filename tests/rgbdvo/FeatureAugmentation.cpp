#include "FeatureAugmentation.h"
#include <cvt/vision/Vision.h>

#include <set>

namespace cvt {

    void setSpaceOccupied( uint8_t* ptr, size_t stride, size_t w=3, size_t h = 3, uint8_t val = 255 )
    {
        while( h-- ){
            for( size_t i = 0; i < w; i++ )
                ptr[ i ] = val;
            ptr += stride;
        }
    }

    class NoDepthFeatureInserter {
        public:
            NoDepthFeatureInserter( Image& mask ):
                _currentScale( 1.0f ),
                _occMap( mask )
            {
            }

            ~NoDepthFeatureInserter()
            {
            }

            void operator()( float x, float y )
            {
                int ix = x * _currentScale;
                int iy = y * _currentScale;

                // no feature occupying this position?
                uint8_t* occptr = _occMap.ptr() + iy * _occMap.stride() + ix;
                if( occptr[ 0 ] == 0 ){
                    //free
                    _features.push_back( Vector2f( _currentScale * x, _currentScale * y ) );

                    // mark the position as occupied
                    occptr--;
                    occptr -= _occMap.stride();
                    setSpaceOccupied( occptr, _occMap.stride(), _currentScale * 3, _currentScale* 3, 255 );
                }
            }

            const Vector2f* features() const    { return &_features[ 0 ]; }
            size_t          numFeatures() const { return _features.size(); }

            void setScale( float scale ) { _currentScale = scale; }

        private:
            float                   _currentScale;
            IMapScoped<uint8_t>     _occMap;

            std::vector<Vector2f>   _features;
    };

    FeatureAugmentation::FeatureAugmentation( const Matrix3f& intrinsics ) :
        _intrinsics( intrinsics ),
        _nextId( 0 ),
        _fastDetector()
    {
        _fastDetector.setBorder( 16 );
        _fastDetector.setNonMaxSuppress( true );
        _fastDetector.setThreshold( 20 );
    }

    FeatureAugmentation::~FeatureAugmentation()
    {
    }

    void FeatureAugmentation::trackAndTriangulate( std::vector<Vector3f>& newPoints,
                                                   const ImagePyramid& pyramidU8,
                                                   const ImagePyramid& pyramidF,
                                                   const Image& depth,
                                                   const Matrix4f& pose3d )
    {
        depth.convert( _mask, IFormat::GRAY_UINT8 );

        Matrix4f projMat = pose3d * Matrix4f( _intrinsics );

        // track current features of the tracks
        //trackFeatures( img );

        // depth initialize tracks with sufficient length or confidence
        initializeNewFeatures( newPoints );

        // detect new features and initialize new tracks
        detectNewFeatures( projMat, pyramidU8 );

        _mask.save( "mask.png" );

    }

    void FeatureAugmentation::trackFeatures( const Image& img, const Matrix4f& projMat )
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
        PointSet2d  trackedPositions;
        _featureTracking.trackFeatures( trackedPositions, trackedIds, positions, ids, img );

        IMapScoped<uint8_t> maskMap( _mask );

        // update feature tracks of successfully tracked features
        for( size_t i = 0; i < trackedIds.size(); i++ ){
            const Vector2d& p = trackedPositions[ i ];

            MapType::iterator track = _featureTracks.find( trackedIds[ i ] );

            uint8_t* maskPtr = maskMap.ptr() + ( int )( p.y - 1 ) * maskMap.stride() + ( int )( p.x - 1 );
            setSpaceOccupied( maskPtr, maskMap.stride(), 3, 3, 255 );

            track->second.positions.push_back( Vector2f( p.x, p.y ) );
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

    void FeatureAugmentation::initializeNewFeatures( std::vector<Vector3f>& newPoints )
    {
        // check the current feature tracks
        MapType::iterator trackIter = _featureTracks.begin();
        const MapType::iterator trackEnd = _featureTracks.end();

        while( trackIter != trackEnd ){
            bool erase = false;
            if( trackIter->second.positions.size() > _minTrackLength ){
                Vector4f p3dHom;
                Vision::triangulate( p3dHom,
                                     &( trackIter->second.projectionMat[ 0 ] ),
                                     &( trackIter->second.positions[ 0 ] ),
                                     trackIter->second.positions.size() );

                if( Math::abs( p3dHom.w ) > 0.0000001 ){
                    newPoints.push_back( p3dHom );
                    erase = true;
                }
            }

            if( erase ){
                // remove this track, as we initialized a depth value!
                _featureTracks.erase( trackIter++ );
            } else {
                ++trackIter;
            }
        }
    }

    void FeatureAugmentation::detectNewFeatures( const Matrix4f& projMat, const ImagePyramid& pyr )
    {
        float scale = 1.0f;
        NoDepthFeatureInserter inserter( _mask );
        for( size_t i = 0; i < pyr.octaves(); i++ ){
            _fastDetector.extract( pyr[ i ], inserter );
            scale /= pyr.scaleFactor();
            inserter.setScale( scale );
        }

        // initialize the new tracks using those features:
        for( size_t i = 0; i < inserter.numFeatures(); i++ ){
            const Vector2f& f = inserter.features()[ i ];

            _featureTracking.addFeatureToDatabase( f, _nextId );
            _featureTracks.insert( std::make_pair( _nextId, FeatureTrack( f, projMat ) ) );
            _nextId++;
        }
    }

}
