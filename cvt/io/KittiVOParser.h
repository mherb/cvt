#ifndef CVT_KITTIVOPARSER_H
#define CVT_KITTIVOPARSER_H

#include <cvt/util/String.h>
#include <cvt/math/Matrix.h>
#include <cvt/gfx/Image.h>

namespace cvt {

    class KittiVOParser
    {
        public:
            KittiVOParser( const cvt::String& folder );
            ~KittiVOParser();

            bool hasNext() const;
            const cvt::Matrix4d& calibLeft()  const { return _calibLeft; }
            const cvt::Matrix4d& calibRight() const { return _calibRight; }

            void            loadNext();
            const Image&    left()  const { return _left; }
            const Image&    right() const { return _right; }
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
