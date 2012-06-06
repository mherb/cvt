/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
 */
#ifndef CVT_KLT_PATCH_H
#define CVT_KLT_PATCH_H

#include <cvt/vision/alignment/PatchBase.h>

namespace cvt
{
    template<size_t pSize, class PoseType> class KLTPatch;

    template <size_t pSize, class PoseType>
    struct PatchTraits< KLTPatch<pSize, PoseType> >
    {
        typedef PoseType PType;
        typedef Eigen::Matrix<float, PType::NPARAMS, PType::NPARAMS> HessType;
        typedef Eigen::Matrix<float, PoseType::NPARAMS, 1>           JacType;
        static const size_t PatchSize = pSize;
    };

    template <size_t pSize, class PoseType>
    class KLTPatch : public PatchBase<KLTPatch<pSize, PoseType> >
    {
        public:
            typedef typename PatchTraits<KLTPatch<pSize, PoseType> >::HessType HessType;
            typedef typename PatchTraits<KLTPatch<pSize, PoseType> >::JacType  JacType;
            static const size_t PatchSize = pSize;
            typedef PatchBase< KLTPatch<pSize, PoseType> > Base;

            KLTPatch(){}

            bool alignWithImage( const Image &img );
            void currentCenter( Vector2f& center )  const;
            void initPose( const Vector2f& pos );

            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        private:

            KLTPatch( const KLTPatch& );
            KLTPatch& operator= (const KLTPatch& );
    };

    template <size_t pSize, class PoseType>
    inline bool KLTPatch<pSize, PoseType>::alignWithImage( const Image& img )
    {
        JacType jSum;
        typename PoseType::ParameterVectorType delta;

        float diffSum = 0.0f;

        const float maxDiff = 0.7 * Math::sqr( pSize * 255.0f );

        IMapScoped<const uint8_t> imageMap( img );
        const uint8_t* current = imageMap.ptr();
        size_t currStride      = imageMap.stride();

        size_t maxIters = 10;

        size_t iter = 0;
        while( iter < maxIters ){
            jSum.setZero();
            diffSum = 0.0f;

            //pose matrix
            Matrix3f pose;
            EigenBridge::toCVT( pose, Base::_pose.transformation() );

            // first test if all points transform into the image
            if( !this->patchIsInImage( pose, img.width(), img.height() ) ){
                return false;
            }

            diffSum = this->buildSystem( jSum, pose, current, currStride );

            if( diffSum > maxDiff )
                return false;

            // solve for the delta:
            delta = Base::_inverseHessian * jSum;
            Base::_pose.applyInverse( -delta );

            iter++;
        }
        return true;
    }

    template <size_t pSize, class PoseType>
    inline void KLTPatch<pSize, PoseType>::currentCenter( Vector2f& center ) const
    {
        const Eigen::Matrix3f& tmp = Base::_pose.transformation();
        center.x = tmp( 0, 2 );
        center.y = tmp( 1, 2 );
    }

    template <size_t pSize, class PoseType>
    inline void KLTPatch<pSize, PoseType>::initPose( const Vector2f& pos )
    {
        Matrix3f m;
        m.setIdentity();
        m[ 0 ][ 2 ] = pos.x;
        m[ 1 ][ 2 ] = pos.y;
        Base::_pose.set( m );
    }

}

#endif
