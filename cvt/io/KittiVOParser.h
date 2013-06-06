/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_KITTIVOPARSER_H
#define CVT_KITTIVOPARSER_H

#include <cvt/util/String.h>
#include <cvt/math/Matrix.h>
#include <cvt/gfx/Image.h>
#include <cvt/io/StereoInput.h>

namespace cvt {

    class KittiVOParser : public StereoInput
    {
        public:
            KittiVOParser( const cvt::String& folder );
            ~KittiVOParser();

            const Image&    left()  const { return _left; }
            const Image&    right() const { return _right; }
            bool            nextFrame();
            const StereoCameraCalibration& stereoCalib() const { return _calib; }

            const cvt::Matrix4d& projectionLeft()  const { return _calibLeft; }
            const cvt::Matrix4d& projectionRight() const { return _calibRight; }

            bool            hasNext() const;
            size_t          size()  const { return _sequence.size(); }
            bool            hasPose() const { return _curSample->hasPose; }
            const Matrix4d& pose()    const { return _curSample->pose; }
            double          stamp()   const { return _curSample->timestamp; }

        private:
            struct Sample {
                double          timestamp;
                cvt::String     leftFile;
                cvt::String     rightFile;
                bool            hasPose;
                cvt::Matrix4d   pose;
            };

            size_t              _iter;
            std::vector<Sample> _sequence;
            cvt::Matrix4d       _calibLeft;
            cvt::Matrix4d       _calibRight;
            StereoCameraCalibration _calib;

            Image               _left;
            Image               _right;
            Sample*             _curSample;

            void checkFileExistence( const cvt::String& file );
            void loadImageNames( std::vector<cvt::String>& names, const cvt::String& folder );
            void loadPoses( std::vector<cvt::Matrix4d>& poses, const cvt::String& file );
            void loadStamps( std::vector<double>& stamps, const cvt::String& file );
            void loadCalibration( const cvt::String& calibFile );
            void loadImages();

    };

}

#endif // KITTYVODATA_H
