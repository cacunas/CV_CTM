#include "setTemporalVariance.h"
#include "paintView.h"
#include <QPainter>

setTemporalVariance::setTemporalVariance(Datapool *i_data):drawInterface(i_data){}

setTemporalVariance::~setTemporalVariance(){
		m_data = NULL;
}

bool setTemporalVariance::setParameters(QDomNode &config){
		return true;
}

bool setTemporalVariance::init(){ return true;}

bool setTemporalVariance::draw(QImage *image){
		if(!paint(image))
				return false;
		return true;
}
bool setTemporalVariance::paint(QImage *image){
  if(m_data->currentImage != NULL)
    if(m_data->temporalVariance == NULL) {
      int w = m_data->currentImage->width(), h = m_data->currentImage->height();
      memset(image->bits(), 0, 4*w*h*sizeof(uchar));
    } else
       *image = m_data->temporalVariance->convertToFormat(QImage::Format_ARGB32);

		return true;
}
