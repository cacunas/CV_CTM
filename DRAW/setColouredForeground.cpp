#include "setColouredForeground.h"
#include "paintView.h"
#include <QPainter>

setColouredForeground::setColouredForeground(Datapool *i_data):drawInterface(i_data){}

setColouredForeground::~setColouredForeground(){
    m_data = NULL;
}

bool setColouredForeground::setParameters(QDomNode &config){
    return true;
}

bool setColouredForeground::init(){ return true;}

bool setColouredForeground::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}
bool setColouredForeground::paint(QImage *image){
    if(m_data->rFgImage != NULL)
        *image =  m_data->rFgImage->convertToFormat(QImage::Format_ARGB32);
    return true;
}


