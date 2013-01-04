/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/
#ifndef CVT_ALIGNMENTCOSTFUNCTION_H
#define CVT_ALIGNMENTCOSTFUNCTION_H

namespace cvt {

    class AlignmentCostFunction
    {
        public:
            AlignmentCostFunction();

            /**
             *  \brief evaluate the jacobians and the residuals
             */
            void eval();

            /**
             *  \brief evaluate the residuals only (e.g. to update the costs)
             */
            void evalResiduals();

            /**
             *  \brief get "estimated" parameters
             */
            void parameters();

            /**
             *  \brief set parameters
             *
             *  set the current parameters of the cost function, e.g. for initialization
             *  or in case of reset when the last computed update shall be reverted
             */
            void setParameters();

        private:
            // reference frame (RGBD)
            // current input data
            //

    };

}

#endif
