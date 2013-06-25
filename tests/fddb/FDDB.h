#include <cvt/util/String.h>
#include <cvt/util/Data.h>
#include <cvt/util/DataIterator.h>
#include <cvt/io/FileSystem.h>
#include <cvt/geom/Ellipse.h>

#include <vector>


namespace cvt {
	class FDDBEntry {
		public:
			FDDBEntry( const String& file, size_t size ) : _file( file ), _size( size )
			{
				_ellipse = new Ellipsef[ _size ];
			}

			FDDBEntry( const FDDBEntry& entry ) : _file( entry._file ), _size( entry._size )
			{
				_ellipse = new Ellipsef[ _size ];
				for( size_t i = 0; i < _size; i++ )
					_ellipse[ i ] = entry[ i ];
			}

			~FDDBEntry()
			{
				delete[] _ellipse;
			}

			const String& file() const
			{
				return _file;
			}

			size_t size() const
			{
				return _size;
			}

			const Ellipsef& operator[]( size_t i ) const
			{
				return _ellipse[ i ];
			}

			Ellipsef& operator[]( size_t i )
			{
				return _ellipse[ i ];
			}

		private:
			String		_file;
			size_t		_size;
			Ellipsef*	_ellipse;
	};

	class FDDB {
		public:
			FDDB()
			{
			}

			FDDB( const String& file, const String& imgpath )
			{
				parse( file, imgpath );
			}

			size_t size() const
			{
				return _entries.size();
			}

			const FDDBEntry& operator[]( size_t i ) const
			{
				return _entries[ i ];
			}

			void clear()
			{
				_entries.clear();
			}
	
			void add( const FDDBEntry& entry )
			{
				_entries.push_back( entry );
			}

			const float averageArea() const
			{
				float n = 0;
				float a = 0;

				size_t size = _entries.size();
				for( size_t i = 0; i < size; i++ ) {
					size_t size2 = _entries[ i ].size();
					n += size2;
					for( size_t k = 0; k < size2; k++ )
						a += _entries[ i ][ k ].area();
				}

				return a / n;
			}

			void averageSemiMajorMinor( float& major, float& minor ) const
			{
				float n = 0;
				major = minor = 0;

				size_t size = _entries.size();
				for( size_t i = 0; i < size; i++ ) {
					size_t size2 = _entries[ i ].size();
					n += size2;
					for( size_t k = 0; k < size2; k++ ) {
						major += _entries[ i ][ k ].semiMajor();
						minor += _entries[ i ][ k ].semiMinor();
					}
				}
				major /= n;
				minor /= n;
			}


			void filterArea( FDDB& output, float min, float max ) const
			{
				size_t size = _entries.size();
				for( size_t i = 0; i < size; i++ ) {

					size_t size2 = _entries[ i ].size();
					size_t nsize = 0;
					for( size_t k = 0; k < size2; k++ ) {
						float area = _entries[ i ][ k ].area();
						if( area > min && area < max )
							nsize++;
					}
					if( nsize ) {
						FDDBEntry entry( _entries[ i ].file(), nsize );
						nsize = 0;
						for( size_t k = 0; k < size2; k++ ) {
							float area = _entries[ i ][ k ].area();
							if( area > min && area < max )
								entry[ nsize++ ] = _entries[ i ][ k ];
						}
						output.add( entry );
					}
				}
			}


		private:
			void parse( const String& file, const String& imgpath )
			{
			  Data d;
			  FileSystem::load( d, file );
			  String delim = " \n\t";
			  String str1, str2;

			  DataIterator iter( d );

			  while( iter.hasNext() ) 
			  {
				iter.nextLine( str1 );
				iter.nextLine( str2 );
				add( FDDBEntry( imgpath + str1, str2.toInteger() ) );
				FDDBEntry& entry = _entries.back();
				for( size_t i = 0; i < entry.size(); i++ ) {
					iter.nextToken( str1, delim );
					entry[ i ].semiMajor() = str1.to<float>();
					iter.nextToken( str1, delim );
					entry[ i ].semiMinor() = str1.to<float>();
					iter.nextToken( str1, delim );
					entry[ i ].orientation() = str1.to<float>();
					iter.nextToken( str1, delim );
					entry[ i ].center().x = str1.to<float>();
					iter.nextToken( str1, delim );
					entry[ i ].center().y = str1.to<float>();
					iter.skipInverse("\n\r");
					iter.skip("\n\r");
				}
			  }
			}

			std::vector<FDDBEntry> _entries;
	};
}
