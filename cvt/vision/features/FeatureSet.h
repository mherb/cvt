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
			FeatureSet();
			~FeatureSet();

			void			add( const Feature& feature );
			void			clear();

			size_t			size() const;
			Feature&		operator[]( size_t i );
			const Feature&	operator[]( size_t i ) const;

			void			toPointSet2f( PointSet2f& ptset ) const;
			void			setFeatures( const Feature* f, size_t n );

			void			filterNMS( int radius, bool sortPosition );
			void			filterANMS( int radius, float threshold, bool sortPosition );
			void			filterBest( size_t n, bool sortScore );

		private:
			class CmpScore
			{
				public:
					bool operator()( const Feature& f1, const Feature& f2 )
					{
						return f1.score < f2.score;
					}
			};

			class CmpPos
			{
				public:
					bool operator()( const Feature& f1, const Feature& f2 )
					{
						if( f1.pt.y == f2.pt.y )
							return f1.pt.x < f2.pt.x;
						return f1.pt.y < f2.pt.y;
					}
			};

			std::vector<Feature> _features;
	};


	inline FeatureSet::FeatureSet()
	{
	}

	inline FeatureSet::~FeatureSet()
	{
	}

	inline void FeatureSet::add( const Feature& feature )
	{
		_features.push_back( feature );
	}

	inline void FeatureSet::clear()
	{
		_features.clear();
	}

	inline size_t FeatureSet::size() const
	{
		return _features.size();
	}

	inline Feature& FeatureSet::operator[]( size_t i )
	{
		return _features[ i ];
	}

	inline const Feature& FeatureSet::operator[]( size_t i ) const
	{
		return _features[ i ];
	}

	inline void FeatureSet::toPointSet2f( PointSet2f& ptset ) const
	{
		ptset.clear();
		for( size_t i = 0; i < size(); i++ )
			ptset.add( ( *this )[ i ].pt );
	}

	inline void FeatureSet::setFeatures( const Feature* f, size_t n )
	{
		_features.assign( f, f + n );
	}

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
				_featureset.add( Feature( x * _scale, y * _scale, 0, _octave, score ) );
			}


		private:
			FeatureSet& _featureset;
			float		_scale;
			size_t		_octave;

			FeatureSetWrapper( const FeatureSet& );
	};

}

#endif
