#ifndef CVT_MMVSTEREOPARSER_H
#define CVT_MMVSTEREOPARSER_H

#include <cvt/util/String.h>
#include <cvt/io/FileSystem.h>
#include <cvt/util/Data.h>
#include <cvt/util/DataIterator.h>
#include <cvt/vision/CameraCalibration.h>

#include <vector>

namespace cvt {

	class MMVStereoSample {
		public:
			MMVStereoSample( const CameraCalibration& cam, const String& imgpath );
			MMVStereoSample( const MMVStereoSample& );

			const CameraCalibration& calib() const;
			const String& imagePath() const;

		private:
			CameraCalibration _calib;
			String			  _imgpath;
	};

	inline MMVStereoSample::MMVStereoSample( const CameraCalibration& cam, const String& imgpath ) :
		_calib( cam ),
		_imgpath( imgpath )
	{
	}

	inline MMVStereoSample::MMVStereoSample( const MMVStereoSample& other ) :
		_calib( other._calib ),
		_imgpath( other._imgpath )
	{
	}

	const CameraCalibration& MMVStereoSample::calib() const
	{
		return _calib;
	}

	const String& MMVStereoSample::imagePath() const
	{
		return _imgpath;
	}


	class MMVStereoParser {
		public:
			MMVStereoParser( const String& paramsfile, const String& imagepath );
			~MMVStereoParser();

			size_t size() const;

			const MMVStereoSample& operator[]( size_t idx ) const;

		private:
			void load( const String& paramfile );
			String						 _imgpath;
			std::vector<MMVStereoSample> _data;
	};

	inline MMVStereoParser::MMVStereoParser( const String& paramsfile, const String& imagepath ) :
		_imgpath( imagepath )
	{
		load( paramsfile );
	}

	inline MMVStereoParser::~MMVStereoParser()
	{
	}

	inline void MMVStereoParser::load( const String& file )
	{
		Data data;
		FileSystem::load( data, file );
		DataIterator it( data );

		size_t numentries = it.nextLong();

		for( size_t i = 0; i < numentries; i++ ) {
			String file;
			Matrix3f K, R;
			Vector3f t;
			CameraCalibration calib;

			it.nextToken( file, " \n\t\r" );

			for( int y = 0; y < 3; y++ ) {
				for( int x = 0; x < 3; x++ ) {
					K[ y ][ x ] = it.nextDouble();
				}
			}
			K[ 2 ][ 2 ] = 1.0f;
			calib.setIntrinsics( K );

			for( int y = 0; y < 3; y++ ) {
				for( int x = 0; x < 3; x++ ) {
					R[ y ][ x ] = it.nextDouble();
				}
			}

			t[ 0 ] = it.nextDouble();
			t[ 1 ] = it.nextDouble();
			t[ 2 ] = it.nextDouble();

			Matrix4f E;
			E = R.toMatrix4();
			E[ 0 ][ 3 ] = t[ 0 ];
			E[ 1 ][ 3 ] = t[ 1 ];
			E[ 2 ][ 3 ] = t[ 2 ];
			calib.setExtrinsics( E );

			_data.push_back( MMVStereoSample( calib, _imgpath + file ) );
		}
	}


	inline const MMVStereoSample& MMVStereoParser::operator[]( size_t idx ) const
	{
		return _data[ idx ];
	}

	inline size_t MMVStereoParser::size() const
	{
		return _data.size();
	}

}

#endif
