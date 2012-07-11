/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef CVT_RGBDTEMPLATE_H
#define CVT_RGBDTEMPLATE_H

#include <cvt/math/Vector.h>

namespace cvt {

    template <typename T>
    class RGBDTemplate
    {
        public:
            typedef cvt::Vector3<T>         WorldPointType;
			typedef cvt::Matrix4<T>			MatrixType;

            // Todo: This is Model Dependent!
            // e.g. if we use affine illumination or less parameters
            typedef Eigen::Matrix<T, 6, 1>  JacType;

            RGBDTemplate();
            ~RGBDTemplate();

            void update( const Image& gray, const Image& depth, const MatrixType& pose );

            const WorldPointType*   points()    const { return &_points3d[ 0 ]; }
            size_t                  numPoints() const { return _points3d.size(); }
            const JacType*          jacobians() const { return &_jacobians[ 0 ]; }

        private:
            std::vector<float>          _pixelValues;
            std::vector<Vector3<T> >    _points3d;
            std::vector<JacType>        _jacobians;

            void computeImageGradients( Image& gx, Image& gy, const Image& gray )
            {
                IKernel kx = IKernel::HAAR_HORIZONTAL_3;
                IKernel ky = IKernel::HAAR_VERTICAL_3;
                IKernel gaussx = IKernel::GAUSS_HORIZONTAL_3;
                IKernel gaussy = IKernel::GAUSS_VERTICAL_3;

                //const float scale = -1.0f;
                const float scale = -0.5f;
                kx.scale( scale );
                ky.scale( scale );

                gx.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );
                gy.reallocate( gray.width(), gray.height(), IFormat::GRAY_FLOAT );

                // sobel
                gray.convolve( gx, kx, gaussy );
                gray.convolve( gy, gaussx, ky );

                // normal
                //gray.convolve( gx, kx );
                //gray.convolve( gy, ky );
            }

    };


    template <class T>        
	void RGBDTemplate<T>::update( const Image& gray, const Image& depth, const MatrixType& pose )
	{
		/* we distinguish three types: 
		  * forward: points from the current Image are used:   min( sum( T( w( x, p ) ) - I( x ) ) ) 
		  * backward: points from the template Image are used: min( sum( T( x ) - I( w( x, p ) ) ) )
		  * inverse compositional:							   min( sum( T( w( x, dp ) ) - I( w( x, p ) ) ) )

		  * As we want a generic framework, we should put the logic for that into seperate classes: 
		  * forward:  points are warped from current image to Template: P_t = W_tc * P_c
		  * backward: points are warped from template image to current: P_c = W_ct * P_t
		  * inverse compositional: points are warped from the template into the current image: P_c = W_ct * P_t
		*/

		/* Forward: Reference is the current image
			* compute gradients of Template -> template is the previous
			* get the 3D points from the depth image
		    * warp them to the other frame using the pose (3D pts)
			* project them
			* evaluate the jacobians
			* interpolate the current pixel values
		 */
	}

}

#endif // RGBDTEMPLATE_H
