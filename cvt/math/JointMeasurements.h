#ifndef CVT_JOINTMEASUREMENTS_H
#define CVT_JOINTMEASUREMENTS_H

#include <map>

namespace cvt
{
	class JointMeasurements
	{
		public:

			typedef std::map< size_t, std::set<size_t> > MapType;
			typedef std::pair<size_t, std::set<size_t> > IterType;
			typedef std::map<size_t, std::set<size_t> >::const_iterator MapIteratorType;

			JointMeasurements();

			void resize( size_t n );

			void addMeasurementForEntity( size_t e0, size_t e1, size_t m );

			/* Iterators */
			MapIteratorType secondEntityIteratorBegin( size_t e0 ) const { return _jointMeasForEntity[ e0 ].begin(); }
			MapIteratorType secondEntityIteratorEnd( size_t e0 ) const { return _jointMeasForEntity[ e0 ].end(); }

			size_t	numBlocks() const { return _numBlocks; }
		private:
			/*
				vector: numEntities
			    map:	id ->				secondEntity
						std::set<size_t>	set of joint measurement ids		
			 */
			std::vector< MapType > _jointMeasForEntity;
			size_t				   _numBlocks;

	};

	inline JointMeasurements::JointMeasurements() :
		_numBlocks( 0 )
	{
	}

	inline void JointMeasurements::resize( size_t n )
	{
		_jointMeasForEntity.clear();
		_jointMeasForEntity.resize( n );
		_numBlocks = 0;
	}

	inline void JointMeasurements::addMeasurementForEntity( size_t e0, size_t e1, size_t m )
	{
		// TODO: assert that e0 > e1
		IterType tmpValue( e1, std::set<size_t>() );
		IterType setForEntityPair;

		/* this will either insert a new set, or return the currently stored */
		setForEntityPair = _jointMeasForEntity[ e0 ].insert( tmpValue );
		setForEntityPair.second.insert( m );

		if( setForEntityPair.second ){
			// newly inserted set for the second entity
			_numBlocks++;		
		}

	}
}

#endif
