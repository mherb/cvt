/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef CVT_JOINTMEASUREMENTS_H
#define CVT_JOINTMEASUREMENTS_H

#include <map>
#include <set>
#include <vector>

namespace cvt
{
	class JointMeasurements
	{
		public:

			typedef std::map< size_t, std::set<size_t> > MapType;
			typedef MapType::iterator					 MapIterType;
			typedef MapType::const_iterator				 ConstMapIterType;

			JointMeasurements();

			void resize( size_t n );
			size_t size() const { return _jointMeasForEntity.size(); }

			void addMeasurementForEntity( size_t e0, size_t e1, size_t m );

			/* Iterators */
			ConstMapIterType secondEntityIteratorBegin( size_t e0 ) const { return _jointMeasForEntity[ e0 ].begin(); }
			ConstMapIterType secondEntityIteratorEnd( size_t e0 )   const { return _jointMeasForEntity[ e0 ].end(); }

			size_t	numBlocks() const { return _numBlocks; }
		private:
			/*
				vector: numEntities
			    map:	id ->				secondEntity
						std::set<size_t>	set of joint measurement ids		
			 */
			std::vector< MapType > _jointMeasForEntity;
			size_t				   _numBlocks;

			void dumpMap() const;

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

}

#endif
