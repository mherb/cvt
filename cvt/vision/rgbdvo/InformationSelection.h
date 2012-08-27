/*
            CVT - Computer Vision Tools Library
            
     Copyright (c) 2012, Philipp Heise, Sebastian Klose
     
    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_INFORMATIONSELECTION_H
#define CVT_INFORMATIONSELECTION_H

#include <set>
#include <algorithm>

namespace cvt {

    template <class JType>
    class InformationSelection
    {
        public:
            InformationSelection( size_t numpixels );
            ~InformationSelection();

            const std::set<size_t>& selectInformation( JType* j, size_t n );

            void setNumPixels( size_t n ){ _numPixels = n; }

        private:
            /* number of pixels to select */
            size_t              _numPixels;

            /* ids of the selected pixels */
            std::set<size_t>    _ids;

            /* sort input decending by parameter */
            std::vector<std::vector<size_t> > _sortedIds;

            class IndexComparator {
                public:
                    IndexComparator( const JType* data, size_t idx ) :
                        _data( data ), _idx( idx )
                    {}

                    bool operator()( size_t id0, size_t id1 )
                    {
                        return Math::abs( _data[ id0 ][ _idx ] ) > Math::abs( _data[ id1 ][ _idx ] );
                    }

                private:
                    const JType* _data;
                    size_t       _idx;
            };

    };

    template <class JType>
    inline InformationSelection<JType>::InformationSelection( size_t numPixels ) :
        _numPixels( numPixels )
    {
        _sortedIds.resize( 6 );
    }

    template <class JType>
    inline InformationSelection<JType>::~InformationSelection(){}

    template <class JType>
    inline const std::set<size_t>& InformationSelection<JType>::selectInformation( JType* j, size_t n )
    {
        _ids.clear();

        // sort the first _numpixel parts for each dimension
        _sortedIds[ 0 ].clear();
        _sortedIds[ 0 ].resize( n );
        for( size_t i = 0; i < n; i++ ) _sortedIds[ 0 ][ i ] = i;
        for( size_t i = 1; i < _sortedIds.size(); i++ ) {
            _sortedIds[ i ] = _sortedIds[ 0 ];
            IndexComparator cmp( j, i );
            std::partial_sort( _sortedIds[ i ].begin(),
                               _sortedIds[ i ].begin() + _numPixels,
                               _sortedIds[ i ].end(), cmp );
        }

        // select the best information
        std::vector<size_t> idForDim( 6, 0 );
        size_t currDim = 0;// alternate dimensions while selecting
        size_t idx = 0; // idx into id vector
        while( _ids.size() < _numPixels ){
            idx = idForDim[ currDim ];
            const std::vector<size_t>& currVec = _sortedIds[ currDim ];
            while( _ids.find( currVec[ idx ] ) != _ids.end() ){
                idx++;
                idForDim[ currDim ] = idx;
            }

            _ids.insert( currVec[ idx ] );
            currDim++;

            if( currDim >= _sortedIds.size() )
                currDim = 0;
        }

        return _ids;
    }

}

#endif // INFORMATIONSELECTION_H
