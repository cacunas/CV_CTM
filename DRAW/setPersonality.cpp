#include "setPersonality.h"
#include "paintView.h"
#include <QPainter>

setPersonality::setPersonality(Datapool *i_data):drawInterface(i_data){}

setPersonality::~setPersonality(){
    m_data = NULL;
}

bool setPersonality::setParameters(QDomNode &config){
    return true;
}

bool setPersonality::init(){ return true;}

bool setPersonality::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}
bool setPersonality::paint(QImage *image){
    if(m_data->persoImage != NULL)
        *image =  m_data->persoImage->convertToFormat(QImage::Format_ARGB32);
    return true;
}


