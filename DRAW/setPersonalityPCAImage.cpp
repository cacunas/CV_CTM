#include "setPersonalityPCAImage.h"
#include "paintView.h"
#include <QPainter>

setPersonalityPCAImage::setPersonalityPCAImage(Datapool *i_data):drawInterface(i_data){}

setPersonalityPCAImage::~setPersonalityPCAImage(){
    m_data = NULL;
}

bool setPersonalityPCAImage::setParameters(QDomNode &config){
    return true;
}

bool setPersonalityPCAImage::init(){ return true;}

bool setPersonalityPCAImage::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}
bool setPersonalityPCAImage::paint(QImage *image){
    if(m_data->personalityPCAImage != NULL)
        *image =  m_data->personalityPCAImage->convertToFormat(QImage::Format_ARGB32);
    return true;
}


