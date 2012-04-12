/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_FEATURE_FILTER_H
#define CVT_FEATURE_FILTER_H

#include <cvt/vision/Feature2D.h>
#include <vector>

namespace cvt
{
    class FeatureFilter
    {
        public:
            FeatureFilter( size_t gridsize, size_t w, size_t h ) :
                _grid( gridsize, w, h )
            {
            }

            void addFeature( const Feature2Df* f )
            {
                _grid.addFeature( f );
            }

            void gridFilteredFeatures( std::vector<const Feature2Df*>& features, size_t maxnum )
            {
                features.clear();
                features.reserve( maxnum );
                size_t numPerCell = maxnum / Math::sqr( _grid.n );

                if( numPerCell == 0 )
                    numPerCell = 1;

                Cell* cell = _grid.grid;
                for( size_t y = 0; y < _grid.n; y++ ){
                    for( size_t x = 0; x < _grid.n; x++ ){
                        size_t num = Math::min( numPerCell, cell->features.size() );
                        if( num ){
                            features.insert( features.end(), cell->features.begin(), cell->features.begin() + num );
                        }
                        cell++;
                    }
                }
            }

            void gridFilteredFeatures( std::vector<Vector2f>& features, size_t maxnum )
            {
                features.clear();
                features.reserve( maxnum );
                size_t numPerCell = maxnum / Math::sqr( _grid.n );

                if( numPerCell == 0 )
                    numPerCell = 1;

                Cell* cell = _grid.grid;
                for( size_t y = 0; y < _grid.n; y++ ){
                    for( size_t x = 0; x < _grid.n; x++ ){
                        size_t num = Math::min( numPerCell, cell->features.size() );
                        if( num ){
                            for( size_t i = 0; i < num; ++i ){
                                features.push_back( cell->features[ i ]->pt );
                            }
                        }
                        cell++;
                    }
                }
            }

            void clear()
            {
                Cell* cell = _grid.grid;
                for( size_t y = 0; y < _grid.n; y++ ){
                    for( size_t x = 0; x < _grid.n; x++ ){
                        cell->features.clear();
                        cell++;
                    }
                }
            }

        private:
            struct Cell
            {
                void addFeature( const Feature2Df* f )
                {
                    // start from the end
                    std::vector<const Feature2Df*>::iterator it = features.begin();
                    const std::vector<const Feature2Df*>::iterator itEnd = features.end();

                    while( it != itEnd ){
                        if( ( *it )->score < f->score )
                            break;
                        it++;
                    }
                    features.insert( it, f );
                }
                std::vector<const Feature2Df*> features;
            };

            struct Grid
            {
                Grid( size_t nx, size_t w, size_t h ) :
                    n( nx )
                {
                    grid = new Cell[ n * n ];

                    cellWidth = ( float )w / ( float )n;
                    cellHeight = ( float )h / ( float )n;
                }

                ~Grid()
                {
                    delete[] grid;
                }

                const Cell& cell( size_t y, size_t x ) const
                {
                    return grid[ y * n + x ];
                }

                void addFeature( const Feature2Df* f )
                {
                    size_t x = f->pt.x / cellWidth;
                    size_t y = f->pt.y / cellHeight;

                    grid[ y * n + x ].addFeature( f );
                }

                size_t n;
                float  cellWidth;
                float  cellHeight;

                Cell* grid;

            };

            Grid	_grid;
    };
}

#endif
