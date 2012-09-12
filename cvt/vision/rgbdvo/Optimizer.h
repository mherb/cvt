/*
            CVT - Computer Vision Tools Library

     Copyright (c) 2012, Philipp Heise, Sebastian Klose

    THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
    PARTICULAR PURPOSE.
*/

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

namespace cvt {
    template <class T>
    class Optimizer
    {
        public:
            Optimizer();

            void setMaxIterations( size_t iter ) { _maxIter = iter; }

            virtual void optimize() = 0;

        private:
            size_t _maxIter;
    };

    template <class T>
    inline Optimizer<T>::Optimizer() :
        _maxIter( 10 )
    {
    }

}

#endif // OPTIMIZER_H
