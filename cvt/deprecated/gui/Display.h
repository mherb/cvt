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
