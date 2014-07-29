#include "setForeground.h"
#include "paintView.h"
#include <QPainter>

setForeground::setForeground(Datapool *i_data):drawInterface(i_data){}

setForeground::~setForeground(){
    m_data = NULL;
}

bool setForeground::setParameters(QDomNode &config){
    return true;
}

bool setForeground::init(){ return true;}

bool setForeground::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}
bool setForeground::paint(QImage *image){
    if(m_data->currentImage != NULL)
        if(m_data->fgImage == NULL) {
            int w = m_data->currentImage->width(), h = m_data->currentImage->height();
            memset(image->bits(), 0, 4*w*h*sizeof(uchar));
        } else
            *image = m_data->fgImage->convertToFormat(QImage::Format_ARGB32);
    return true;
}


