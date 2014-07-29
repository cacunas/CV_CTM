#include "setBackground.h"
#include "paintView.h"
#include <QPainter>

setBackground::setBackground(Datapool *i_data):drawInterface(i_data){}

setBackground::~setBackground(){
    m_data = NULL;
}

bool setBackground::setParameters(QDomNode &config){
    return true;
}

bool setBackground::init(){ return true;}

bool setBackground::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}
bool setBackground::paint(QImage *image){
    if(m_data->bgImage != NULL)
        *image = *m_data->bgImage;
    return true;
}


