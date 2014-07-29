#include "setReliabilityMap.h"
#include "paintView.h"
#include <QPainter>

setReliabilityMap::setReliabilityMap(Datapool *i_data):drawInterface(i_data){}

setReliabilityMap::~setReliabilityMap(){
		m_data = NULL;
}

bool setReliabilityMap::setParameters(QDomNode &config){
		return true;
}

bool setReliabilityMap::init(){ return true;}

bool setReliabilityMap::draw(QImage *image){
		if(!paint(image))
				return false;
		return true;
}
bool setReliabilityMap::paint(QImage *image){
		if(m_data->reliabilityMap != NULL)
		{
				if(   m_data->reliabilityMap->width()!= image->width()
					 || m_data->reliabilityMap->height()!= image->height() )
				{
						delete image;
						image = new QImage(m_data->reliabilityMap->width(),
															 m_data->reliabilityMap->height(), QImage::Format_ARGB32);
				}

				paintView::copyBytes(image, m_data->reliabilityMap);
		}

		return true;
}

