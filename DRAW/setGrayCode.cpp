#include "setGrayCode.h"
#include "paintView.h"
#include <QPainter>

setGrayCode::setGrayCode(Datapool *i_data):drawInterface(i_data){}

setGrayCode::~setGrayCode(){
    m_data = NULL;
}

bool setGrayCode::setParameters(QDomNode &config){
    return true;
}

bool setGrayCode::init(){ return true;}

bool setGrayCode::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}
bool setGrayCode::paint(QImage *image){
    if(m_data->grayImage != NULL) {
/*        if(   m_data->grayImage->width()!= image->width()
           || m_data->grayImage->height()!= image->height() ) {
            delete image;
            image = new QImage(m_data->grayImage->width(),
                               m_data->grayImage->height(), QImage::Format_ARGB32);
        }*/

        paintView::copyBytes(image, m_data->grayImage);
    }

    return true;
}


