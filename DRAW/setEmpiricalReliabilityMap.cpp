#include "setEmpiricalReliabilityMap.h"
#include "paintView.h"
#include <QPainter>

setEmpiricalReliabilityMap::setEmpiricalReliabilityMap(Datapool *i_data):drawInterface(i_data){}

setEmpiricalReliabilityMap::~setEmpiricalReliabilityMap(){
		m_data = NULL;
}

bool setEmpiricalReliabilityMap::setParameters(QDomNode &config){
		return true;
}

bool setEmpiricalReliabilityMap::init(){ return true;}

bool setEmpiricalReliabilityMap::draw(QImage *image){
		if(!paint(image))
				return false;
		return true;
}
bool setEmpiricalReliabilityMap::paint(QImage *image){
		if(m_data->empiricalReliabilityMap != NULL)
		{
				if(   m_data->empiricalReliabilityMap->width()!= image->width()
					 || m_data->empiricalReliabilityMap->height()!= image->height() )
				{
						delete image;
						image = new QImage(m_data->empiricalReliabilityMap->width(),
															 m_data->empiricalReliabilityMap->height(), QImage::Format_ARGB32);
				}

				paintView::copyBytes(image, m_data->empiricalReliabilityMap);
		}

		return true;
}

