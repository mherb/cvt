/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURESET_H
#define CVT_FEATURESET_H

#include <cvt/vision/features/Feature.h>
#include <cvt/geom/PointSet.h>
#include <vector>

namespace cvt {

	class FeatureSet
	{
		public:
			virtual ~FeatureSet() {}

			virtual void add( const Feature& feature ) = 0;
			inline void operator()( const Feature& feature ) { add( feature ); }

			virtual size_t size() const = 0;
			virtual Feature& operator[]( size_t i ) = 0;
			virtual const Feature& operator[]( size_t i ) const = 0;

			void toPointSet2f( PointSet2f& ptset ) const {
				ptset.clear();
				for( size_t i = 0; i < size(); i++ )
					ptset.add( ( *this )[ i ].pt );
			}

			virtual void filter() {};
	};

    class FeatureSetWrapper
	{
		public:
			FeatureSetWrapper( FeatureSet& set, float scale = 1.0f, size_t octave = 0 ) :
				_featureset( set ),
				_scale( scale ),
				_octave( octave )
			{}

			void operator()( float x, float y, float score = 0.0f )
			{
				_featureset.add( Feature( x * _scale, y * _scale, score, _octave ) );
			}


		private:
			FeatureSet& _featureset;
			float		_scale;
			size_t		_octave;

			FeatureSetWrapper( const FeatureSet& );
	};

	class FeatureSetNoFilter : public FeatureSet
	{
		public:
			FeatureSetNoFilter() {}
			~FeatureSetNoFilter() {}

			void add( const Feature& feature ) { _features.push_back( feature ); }

			size_t size() const { return _features.size(); }
			Feature& operator[]( size_t i ) { return _features[ i ]; }
			const Feature& operator[]( size_t i ) const { return _features[ i ]; }

		private:
			std::vector<Feature> _features;
	};


}

#endif
