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
		switch (img.order().id ) {
			case ICHANNELORDER_BGRA:
			case ICHANNELORDER_RGBA:
				{    
					QImage i((uchar*)img.map(), 
							 img.width(), 
							 img.height(), 
							 img.stride(), 
							 QImage::Format_ARGB32);
					img.unmap();
					pi = scene->addPixmap(QPixmap::fromImage(i));
					break;
				}	    
			case ICHANNELORDER_GRAY:				
			case ICHANNELORDER_GRAYALPHA:
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
