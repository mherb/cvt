#include <cvt/io/KittiVOParser.h>
#include <cvt/util/DataIterator.h>
#include <cvt/util/Exception.h>
#include <cvt/io/FileSystem.h>

namespace cvt {

    KittiVOParser::KittiVOParser( const cvt::String& folder ) :
        _iter( 0 )
    {
        cvt::String leftFolder( folder );
        cvt::String rightFolder( folder );
        cvt::String calibFile( folder );
        cvt::String timesFile( folder );
        cvt::String poseFile( folder );

        leftFolder  += "/image_0/";
        rightFolder += "/image_1/";
        calibFile   += "/calib.txt";
        timesFile   += "/times.txt";
        poseFile    += "/poses.txt";

        checkFileExistence( leftFolder );
        checkFileExistence( rightFolder );
        checkFileExistence( calibFile );
        checkFileExistence( timesFile );
        checkFileExistence( poseFile );

        std::vector<cvt::String>   filesLeft;
        std::vector<cvt::String>   filesRight;
        std::vector<cvt::Matrix4d> poses;
        std::vector<double>        stamps;

        loadImageNames( filesLeft, leftFolder );
        loadImageNames( filesRight, rightFolder );

        std::cout << "Loading poses: " << std::endl;
        loadPoses( poses, poseFile );
        std::cout << "Loading stamps: " << std::endl;
        loadStamps( stamps, timesFile );
        std::cout << "Loading calibFile: " << std::endl;
        loadCalibration( calibFile );

        // check sizes:
        size_t n = filesLeft.size();
        if( n != filesRight.size() ){
            throw CVTException( "Unequal number of images in subfolders" );
        }

        if( n != poses.size() ){
            throw CVTException( "Unequal number of images and poses" );
        }

        if( n != stamps.size() ){
            throw CVTException( "Unequal number of images and stamps" );
        }

        _sequence.resize( n );
        for( size_t i = 0; i < n; i++ ){
            _sequence[ i ].timestamp    = stamps[ i ];
            _sequence[ i ].leftFile     = filesLeft[ i ];
            _sequence[ i ].rightFile    = filesRight[ i ];
            _sequence[ i ].hasPose      = true;
            _sequence[ i ].pose         = poses[ i ];
        }

        _curSample = &_sequence[ _iter ];
        loadImages();
    }

    KittiVOParser::~KittiVOParser()
    {
    }

    void KittiVOParser::loadNext()
    {
        _iter++;
        _curSample = &_sequence[ _iter ];
        loadImages();
    }

    bool KittiVOParser::hasNext() const
    {
        return _iter < _sequence.size();
    }    

    void KittiVOParser::checkFileExistence( const cvt::String& file )
    {
        if( !cvt::FileSystem::exists( file ) ){
            cvt::String msg;
            msg.sprintf( "File <%s> not found!\n", file.c_str() );
            throw CVTException( msg.c_str() );
        }
    }

    void KittiVOParser::loadImages()
    {
        _left.load( _curSample->leftFile );
        _right.load( _curSample->rightFile );
    }

    void KittiVOParser::loadImageNames( std::vector<cvt::String>& names, const cvt::String& folder )
    {
        cvt::FileSystem::filesWithExtension( folder, names, "png" );

        std::sort( names.begin(), names.end() );
    }

    void KittiVOParser::loadPoses( std::vector<cvt::Matrix4d>& poses, const cvt::String& file )
    {
        cvt::Data d;
        if( !cvt::FileSystem::load( d, file ) ){
            throw CVTException( "Error when loading pose file!" );
        }

        cvt::DataIterator iter( d );

        cvt::Matrix4d p;
        p.setIdentity();

        cvt::String line;
        while( iter.nextLine( line ) ){
            std::vector<cvt::String> lineTokens;
            line.tokenize( lineTokens, ' ' );

            if( lineTokens.size() != 12 ){
                throw CVTException( "Corrupt Pose File" );
            }

            p[ 0 ][ 0 ] = lineTokens[ 0  ].toDouble();
            p[ 0 ][ 1 ] = lineTokens[ 1  ].toDouble();
            p[ 0 ][ 2 ] = lineTokens[ 2  ].toDouble();
            p[ 0 ][ 3 ] = lineTokens[ 3  ].toDouble();

            p[ 1 ][ 0 ] = lineTokens[ 4  ].toDouble();
            p[ 1 ][ 1 ] = lineTokens[ 5  ].toDouble();
            p[ 1 ][ 2 ] = lineTokens[ 6  ].toDouble();
            p[ 1 ][ 3 ] = lineTokens[ 7  ].toDouble();

            p[ 2 ][ 0 ] = lineTokens[ 8  ].toDouble();
            p[ 2 ][ 1 ] = lineTokens[ 9  ].toDouble();
            p[ 2 ][ 2 ] = lineTokens[ 10 ].toDouble();
            p[ 2 ][ 3 ] = lineTokens[ 11 ].toDouble();

            poses.push_back( p );
        }
    }

    void KittiVOParser::loadStamps( std::vector<double>& stamps, const cvt::String& file )
    {
        cvt::Data d;
        if( !cvt::FileSystem::load( d, file ) ){
            throw CVTException( "Error when loading times file!" );
        }

        cvt::DataIterator iter( d );
        cvt::String line;
        while( iter.nextLine( line ) ){
            stamps.push_back( line.toDouble() );
        }
    }

    void KittiVOParser::loadCalibration( const cvt::String& calibFile )
    {
        cvt::Data d;
        if( !cvt::FileSystem::load( d, calibFile ) ){
            throw CVTException( "Error when loading times file!" );
        }

        cvt::DataIterator iter( d );
        cvt::String line;

        bool found0 = false;
        bool found1 = false;

        while( iter.nextLine( line ) && !( found0 && found1 ) ){
            std::vector<cvt::String> lineTokens;
            line.tokenize( lineTokens, ' ' );

            if( lineTokens.size() != 13 ){
                throw CVTException( "Corrupt calib File" );
            }

            if( !found0 && lineTokens[ 0 ] == "P0:" ){
                found0 = true;
                _calibLeft.setIdentity();
                _calibLeft[ 0 ][ 0 ] = lineTokens[ 1  ].toDouble();
                _calibLeft[ 0 ][ 1 ] = lineTokens[ 2  ].toDouble();
                _calibLeft[ 0 ][ 2 ] = lineTokens[ 3  ].toDouble();
                _calibLeft[ 0 ][ 3 ] = lineTokens[ 4  ].toDouble();

                _calibLeft[ 1 ][ 0 ] = lineTokens[ 5  ].toDouble();
                _calibLeft[ 1 ][ 1 ] = lineTokens[ 6  ].toDouble();
                _calibLeft[ 1 ][ 2 ] = lineTokens[ 7  ].toDouble();
                _calibLeft[ 1 ][ 3 ] = lineTokens[ 8  ].toDouble();

                _calibLeft[ 2 ][ 0 ] = lineTokens[ 9  ].toDouble();
                _calibLeft[ 2 ][ 1 ] = lineTokens[ 10 ].toDouble();
                _calibLeft[ 2 ][ 2 ] = lineTokens[ 11 ].toDouble();
                _calibLeft[ 2 ][ 3 ] = lineTokens[ 12 ].toDouble();                
                continue;
            }

            if( !found1 && lineTokens[ 0 ] == "P1:" ){
                found1 = true;
                _calibRight.setIdentity();
                _calibRight[ 0 ][ 0 ] = lineTokens[ 1  ].toDouble();
                _calibRight[ 0 ][ 1 ] = lineTokens[ 2  ].toDouble();
                _calibRight[ 0 ][ 2 ] = lineTokens[ 3  ].toDouble();
                _calibRight[ 0 ][ 3 ] = lineTokens[ 4  ].toDouble();

                _calibRight[ 1 ][ 0 ] = lineTokens[ 5  ].toDouble();
                _calibRight[ 1 ][ 1 ] = lineTokens[ 6  ].toDouble();
                _calibRight[ 1 ][ 2 ] = lineTokens[ 7  ].toDouble();
                _calibRight[ 1 ][ 3 ] = lineTokens[ 8  ].toDouble();

                _calibRight[ 2 ][ 0 ] = lineTokens[ 9  ].toDouble();
                _calibRight[ 2 ][ 1 ] = lineTokens[ 10 ].toDouble();
                _calibRight[ 2 ][ 2 ] = lineTokens[ 11 ].toDouble();
                _calibRight[ 2 ][ 3 ] = lineTokens[ 12 ].toDouble();                
                continue;
            }
        }

        if( !found0 || !found1 ){
            throw CVTException( "Could not find calibration file!" );
        }
    }


}
