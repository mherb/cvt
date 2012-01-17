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
			SparseBundleAdjustment		_sba;
			TerminationCriteria<double>	_termCrit;
			bool						_isRunning;
	};

	inline MapOptimizer::MapOptimizer() :
		_isRunning( false )
	{
		_termCrit.setCostThreshold( 0.5 );
		_termCrit.setMaxIterations( 10 );
	}

	inline MapOptimizer::~MapOptimizer()
	{
		if( _isRunning )
			join();
	}

	inline void MapOptimizer::execute( SlamMap* map )
	{
		_isRunning = true;
		_sba.optimize( *map, _termCrit );
		_isRunning = false;
	}

	inline bool MapOptimizer::isRunning() const
	{
		return _isRunning;
	}
}

#endif
