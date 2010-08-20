#include "Display.h"

namespace cvt {

	Display::Display(QWidget * parent):
		QWidget(parent)
	{
		scene = new QGraphicsScene();
		view = new QGraphicsView(scene);

		QVBoxLayout * vLayout = new QVBoxLayout();
		vLayout->addWidget(view);
		setLayout(vLayout);
	}

	void Display::addImage(cvt::Image & img)
	{
		QGraphicsPixmapItem * pi = 0;
		switch (img.order()) {
			case CVT_BGRA:
			case CVT_RGBA:
				{    
					QImage i((uchar*)img.data(), 
							 img.width(), 
							 img.height(), 
							 img.stride(), 
							 QImage::Format_ARGB32);

					pi = scene->addPixmap(QPixmap::fromImage(i));
					break;
				}	    
			case CVT_GRAY:				
			case CVT_GRAYALPHA:
				return;
				break;
			default:
				return;
				break;
		}

		if(pi){
			pi->setFlag(QGraphicsItem::ItemIsMovable,true);
			pi->setFlag(QGraphicsItem::ItemIsSelectable,true);
			pi->setFlag(QGraphicsItem::ItemIsPanel,true);
			pi->setPos(0, 0);
		}
	}

}
