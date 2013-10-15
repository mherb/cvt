#include <cvt/math/JointMeasurements.h>
#include <iostream>

namespace cvt
{
	void JointMeasurements::addMeasurementForEntity( size_t e0, size_t e1, size_t m )
	{
		MapType & map = _jointMeasForEntity[ e0 ];
		MapIterType iter = map.find( e1 );
		if( iter == map.end() ){
			std::set<size_t> tmpSet;
			tmpSet.insert( m );
			map.insert( std::pair<size_t, std::set<size_t> >( e1, tmpSet ) );
			_numBlocks++;		
		} else {
			iter->second.insert( m );
		}
	}

	void JointMeasurements::dumpMap() const
	{
		for( size_t i = 0; i < _jointMeasForEntity.size(); i++ ){
			const MapType & map = _jointMeasForEntity[ i ];

			ConstMapIterType mapIter = map.begin();
			const ConstMapIterType mapEnd = map.end();
			
			std::cout << "Constraints for camera " << i << std::endl;
			while( mapIter != mapEnd ){
				std::cout << "\t cam " << mapIter->first << ": ";
				std::set<size_t>::const_iterator pIter = mapIter->second.begin();
				const std::set<size_t>::const_iterator pIterEnd = mapIter->second.end();
				
				while( pIter != pIterEnd ){
					std::cout << *pIter << " "; 
					++pIter;
				}
				std::cout << std::endl;
				++mapIter;
			}
		}
		std::cout << std::endl;
	}
}
