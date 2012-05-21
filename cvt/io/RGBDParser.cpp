/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#include <cvt/io/RGBDParser.h>

namespace cvt
{

    RGBDParser::RGBDParser( const String& folder, double maxStampDiff ) :
        _maxStampDiff( maxStampDiff ), // this is 50ms
        _folder( folder ),
        _idx( 0 )
    {
        if( _folder[ _folder.length() - 1 ] != '/' )
            _folder += "/";

        std::vector<double> rgbStamps;
        std::vector<double> depthStamps;

        String assocFile = _folder + "associated.txt";
        bool associated = false;
        if( FileSystem::exists( assocFile ) ){
            // load the associated
            std::cout << "Using preassociated file" << std::endl;
            loadDepthAndRGB( rgbStamps, depthStamps );
            associated = true;
        } else {
            loadRGBFilenames( rgbStamps );
            loadDepthFilenames( depthStamps );
        }
        loadGroundTruth();

        std::cout << "RGB: " << _rgbFiles.size() << std::endl;
        std::cout << "Depth: " << _depthFiles.size() << std::endl;
        std::cout << "Stamps: " << _stamps.size() << std::endl;

        sortOutData( rgbStamps, depthStamps, associated );
        std::cout << "RGB: " << _rgbFiles.size() << std::endl;
        std::cout << "Depth: " << _depthFiles.size() << std::endl;
        std::cout << "Stamps: " << _stamps.size() << std::endl;

    }

    void RGBDParser::loadNext()
    {
        if( _idx >= _stamps.size() ){
            std::cout << "End of data !" << std::endl;
            return;
        }
        _sample.stamp	= _stamps[ _idx ];
        _sample.rgb.load( _rgbFiles[ _idx ] );
        _sample.depth.load( _depthFiles[ _idx ] );
        _sample.pose = _groundTruthPoses[ _idx ];
        _sample.poseValid = _poseValid[ _idx ];
        _idx++;
    }

    void RGBDParser::loadGroundTruth()
    {
        String file;
        file.sprintf( "%sgroundtruth.txt", _folder.c_str() );

        Data gtFile;
        FileSystem::load( gtFile, file );

        DataIterator iter( gtFile );

        Matrix4f p; double stamp;
        while( readNext( p, stamp, iter ) ){
            _groundTruthPoses.push_back( p );
            _stamps.push_back( stamp );
        }
    }

    void RGBDParser::loadRGBFilenames( std::vector<double> & rgbStamps )
    {
        String file;
        file.sprintf( "%srgb.txt", _folder.c_str() );

        Data rgbFile;
        FileSystem::load( rgbFile, file );
        DataIterator iter( rgbFile );

        String name;
        double stamp;
        while( readNextFilename( name, stamp, iter ) ){
            rgbStamps.push_back( stamp );
            _rgbFiles.push_back( _folder + name );
        }
    }

    void RGBDParser::loadDepthAndRGB( std::vector<double>& rgbStamps, std::vector<double>& depthStamps )
    {
        _rgbFiles.clear();
        _depthFiles.clear();

        String file = _folder + "associated.txt";
        Data assocFile;
        FileSystem::load( assocFile, file );
        DataIterator iter( assocFile );

        while( iter.hasNext() ){
            std::vector<String> tokens;
            iter.tokenizeNextLine( tokens, " " );

            if( tokens.size() == 4 ){
                double t = tokens[ 0 ].toDouble();
                if( tokens[ 1 ].hasPrefix( "rgb" ) ){
                    // rgb then depth
                    rgbStamps.push_back( t );
                    _rgbFiles.push_back( _folder + tokens[ 1 ] );
                    depthStamps.push_back( tokens[ 2 ].toDouble() );
                    _depthFiles.push_back( _folder + tokens[ 3 ] );
                } else {
                    depthStamps.push_back( t );
                    _depthFiles.push_back( _folder + tokens[ 1 ] );
                    rgbStamps.push_back( tokens[ 2 ].toDouble() );
                    _rgbFiles.push_back( _folder + tokens[ 3 ] );
                }
            }
        }
    }

    void RGBDParser::loadDepthFilenames( std::vector<double>& depthStamps )
    {
        String file;
        file.sprintf( "%sdepth.txt", _folder.c_str() );

        Data depthFile;
        FileSystem::load( depthFile, file );
        DataIterator iter( depthFile );

        String name;
        double stamp;
        while( readNextFilename( name, stamp, iter ) ){
            depthStamps.push_back( stamp );
            _depthFiles.push_back( _folder + name );
        }
    }

    // read next line from the gtfile
    bool RGBDParser::readNext( Matrix4f& pose, double& stamp, DataIterator& iter )
    {
        if( !iter.hasNext() )
            return false;

        std::vector<String> tokens;
        iter.tokenizeNextLine( tokens, " " );

        while( tokens.size() == 0 || tokens[ 0 ] == "#" ){
            if( !iter.hasNext() )
                return false;
            tokens.clear();
            iter.tokenizeNextLine( tokens, " " );
        }

        // now we should have a valid line with 8 tokens!
        if( tokens.size() != 8 ) std::cout << "File corrupt?!" << std::endl;

        stamp = tokens[ 0 ].toDouble();
        float tx = tokens[ 1 ].toFloat();
        float ty = tokens[ 2 ].toFloat();
        float tz = tokens[ 3 ].toFloat();
        float qx = tokens[ 4 ].toFloat();
        float qy = tokens[ 5 ].toFloat();
        float qz = tokens[ 6 ].toFloat();
        float qw = tokens[ 7 ].toFloat();
        Quaternionf q( qx, qy, qz, qw );

        pose = q.toMatrix4();
        pose[ 0 ][ 3 ] = tx;
        pose[ 1 ][ 3 ] = ty;
        pose[ 2 ][ 3 ] = tz;        

        return true;
    }

    bool RGBDParser::readNextFilename( String& filename, double& stamp, DataIterator& iter )
    {
        if( !iter.hasNext() )
            return false;

        std::vector<String> tokens;
        iter.tokenizeNextLine( tokens, " " );


        while( tokens.size() == 0 || tokens[ 0 ] == "#" ){
            if( !iter.hasNext() ){
                return false;
            }
            tokens.clear();
            iter.tokenizeNextLine( tokens, " " );
        }

        // now we should have a valid line with 8 tokens!
        if( tokens.size() != 2 ) std::cout << "File corrupt?!" << std::endl;

        stamp = tokens[ 0 ].toDouble();
        filename = tokens[ 1 ];

        return true;
    }

    size_t RGBDParser::findClosestMatchInGTStamps( double val, size_t startIdx )
    {
        size_t bestIdx = startIdx;
        double best = Math::abs( _stamps[ startIdx ] - val );        
        for( size_t i = startIdx + 1; i < _stamps.size(); i++ ){
            double cur = Math::abs( _stamps[ i ] - val );
            if( cur < best ){
                best = cur;
                bestIdx = i;
            }
        }
        return bestIdx;
    }

    void RGBDParser::sortOutData( const std::vector<double>& rgbStamps,
                                  const std::vector<double>& depthStamps,
                                  bool rgbAndDepthAssocitated )
    {
        size_t rgbIdx = 0;

        std::vector<double> stamps;
        std::vector<Matrix4f> poses;
        std::vector<String> color;
        std::vector<String> depth;
        std::vector<bool> validity;

        while( rgbIdx < rgbStamps.size() ){
            // find best depth for this rgb
            size_t bestDepthIdx = rgbIdx;
            if( !rgbAndDepthAssocitated ){
                bestDepthIdx = findClosestMatchInDepthStamps( rgbStamps[ rgbIdx ], 0, depthStamps );
            }
            double depthDist = Math::abs( rgbStamps[ rgbIdx ] - depthStamps[ bestDepthIdx ] );
            if( depthDist < _maxStampDiff ){
                // now we have the match: rgbIdx & depthIdx and should search for the closest gt
                depth.push_back( _depthFiles[ bestDepthIdx ] );
                color.push_back( _rgbFiles[ rgbIdx ] );

                size_t bestGtIdx = findClosestMatchInGTStamps( rgbStamps[ rgbIdx ], 0 );
                if( Math::abs( _stamps[ bestGtIdx ] - rgbStamps[ rgbIdx ] ) < _maxStampDiff ){
                    // take it
                    stamps.push_back( _stamps[ bestGtIdx ] );
                    poses.push_back( _groundTruthPoses[ bestGtIdx ] );
                    validity.push_back( true );
                } else {
                    stamps.push_back( rgbStamps[ rgbIdx ] );
                    poses.push_back( poses.back() );
                    validity.push_back( false );
                    //std::cout.precision( 15 );
                    //std::cout << "Could not find match in GT stamps for: " << std::fixed << rgbStamps[ rgbIdx ] << std::endl;
                }
           } else {
                //std::cout.precision( 15 );
                //std::cout << "Could not find match in depth stamps for: " << std::fixed << rgbStamps[ rgbIdx ] << std::endl;
           }
           rgbIdx++;
        }
        std::cout << "Found: " << stamps.size() << " matches" << std::endl;

        _groundTruthPoses = poses;
        _stamps = stamps;
        _rgbFiles = color;
        _depthFiles = depth;
        _poseValid = validity;
    }


    size_t RGBDParser::findClosestMatchInDepthStamps( double stamp,
                                                      size_t iter,
                                                      const std::vector<double>& dStamps )
    {
        size_t bestIdx = iter;
        double best = Math::abs( dStamps[ iter ] - stamp );
        double last = best;
        for( size_t i = iter + 1; i < dStamps.size(); i++ ){
            double cur = Math::abs( dStamps[ i ] - stamp );
            if( cur < best ){
                best = cur;
                bestIdx = i;
            }

            // they are ordered, so if we get a worse match once, we won't get any better
            if( cur > last )
                break;
            last = cur;
        }
        return bestIdx;
    }
}
