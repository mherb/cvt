/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURE2D_H
#define CVT_FEATURE2D_H

#include <cvt/math/Vector.h>
#include <vector>

namespace cvt {
	template<typename T>
	struct Feature2D {
		Feature2D( T x = 0, T y = 0, float angle = 0.0f, size_t octave = 0, float score = 0.0f );

		Vector2<T> pt;
		float angle;
		size_t octave;
        float score;
	};

    template <typename T>
	inline Feature2D<T>::Feature2D( T x, T y, float a, size_t o, float sc ) : pt( x, y ), angle( a ), octave( o ), score( sc )
	{
	}

	typedef Feature2D<int> Feature2Di;
	typedef Feature2D<float> Feature2Df;

    template<typename T>
    struct VectorFeature2DInserter
    {
        VectorFeature2DInserter( std::vector<Feature2D<T> >  & vec ) : _vec( vec )
        {
        }

        void operator()( T x, T y )
        {
            _vec.push_back( Feature2D<T>( x, y ) );
        }


    private:
        std::vector<Feature2D<T> >  & _vec;

        VectorFeature2DInserter( const std::vector<Feature2D<T> > & );
    };


    template<typename T>
    struct VectorVector2Inserter
    {
        VectorVector2Inserter( std::vector<Vector2<T> > & vec ) : _vec( vec )
        {
        }

        void operator()( T x, T y )
        {
            _vec.push_back( Vector2<T>( x, y ) );
        }

    private:
        std::vector<Vector2<T> >  & _vec;

        VectorVector2Inserter( const std::vector<Vector2<T> > & );
    };

}

#endif
