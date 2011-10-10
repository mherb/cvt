#ifndef FEATURE_TRACKING_H
#define FEATURE_TRACKING_H

namespace cvt
{

	/**
	 *	Feature Tracking using ORB Matching	
	 */
	class FeatureTracking 
	{
		public:
			FeatureTracking( size_t maxDescDistance, float windowRadius );
			~FeatureTracking();

            void setMaxDescDist( size_t v ) { _maxDescDistance = v; }
            void setMatchRadius( size_t v ) { _windowRadius = v; }

            void trackFeatures( std::vector<FeatureMatch> & matches, 
							    const std::vector<ORBFeature*> & predicted,
							    const ORB& orb ) const;

		private:
            size_t      _maxDescDistance;
            float       _windowRadius;
            
			void matchInWindow( FeatureMatch& match, const ORBFeature& f, const ORB & orb ) const;
	};

	inline FeatureTracking::FeatureTracking( size_t maxDD, float wR ) :
		_maxDescDistance( maxDD ),
		_windowRadius( wR )
	{
	}

	inline FeatureTracking::~FeatureTracking()
	{
	}

    inline void FeatureTracking::trackFeatures( std::vector<FeatureMatch> & matches, 
											    const std::vector<ORBFeature*> & predicted,
                                                const ORB& orb ) const
    {
        // we want to find the best matching orb feature from current, that lies
        // within a certain distance from the "predicted" position
        std::vector<ORBFeature*>::const_iterator current = predicted.begin();
        std::vector<ORBFeature*>::const_iterator tEnd = predicted.end();

        while( current != tEnd ){
			FeatureMatch m;
			m.feature0 = *current;
            matchInWindow( m, **current, orb );
			matches.push_back( m );
			++current;
        }
    }

    inline void FeatureTracking::matchInWindow( FeatureMatch& match, const ORBFeature & f, const ORB & orb ) const
    {
        int idx = -1;
        match.distance = _maxDescDistance;
        size_t currDist;
        for( size_t i = 0; i < orb.size(); i++ ){
            if( ( f.pt - orb[ i ].pt ).length() < _windowRadius ){
                // try to match
                currDist = f.distance( orb[ i ] );
                if( currDist < match.distance ){
                    idx = i;
					match.feature1 = &orb[ i ];
                    match.distance = currDist;
                }
            }
        }
    }
}

#endif
