/*
			CVT - Computer Vision Tools Library

 	 Copyright (c) 2012, Philipp Heise, Sebastian Klose

 	THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 	PARTICULAR PURPOSE.
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include <QtGui>
#include <QtCore>

#include <cvt/gfx/Image.h>

namespace cvt {
	class Display : public QWidget
	{
		Q_OBJECT
		public:
			Display(QWidget * parent = 0);

			void addImage(cvt::Image & img);

		private:
			QGraphicsScene * scene;
			QGraphicsView * view;	
	};

}

#endif
