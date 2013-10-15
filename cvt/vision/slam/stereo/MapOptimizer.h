#ifndef CVT_MAPOPTIMIZER_H
#define CVT_MAPOPTIMIZER_H

#include <cvt/vision/SparseBundleAdjustment.h>
#include <cvt/util/Thread.h>

namespace cvt
{
    class MapOptimizer : public Thread<SlamMap>
    {
        public:
            MapOptimizer();
            ~MapOptimizer();

            void execute( SlamMap* map );
            bool isRunning() const;

        private:            
            TerminationCriteria<double>	_termCrit;
            bool						_isRunning;
    };

    inline MapOptimizer::MapOptimizer() :
        _isRunning( false )
    {
        _termCrit.setCostThreshold( 0.05 );
        _termCrit.setMaxIterations( 40 );
    }

    inline MapOptimizer::~MapOptimizer()
    {
        if( _isRunning )
            join();
    }

    inline void MapOptimizer::execute( SlamMap* map )
    {
        SparseBundleAdjustment sba;
        _isRunning = true;
        sba.optimize( *map, _termCrit );
        _isRunning = false;
    }

    inline bool MapOptimizer::isRunning() const
    {
        return _isRunning;
    }
}

#endif
