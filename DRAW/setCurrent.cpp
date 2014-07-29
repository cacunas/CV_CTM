#include "setCurrent.h"
#include "paintView.h"
#include <QPainter>

setCurrent::setCurrent(Datapool *i_data):drawInterface(i_data){}

setCurrent::~setCurrent(){
    m_data = NULL;
}

bool setCurrent::setParameters(QDomNode &config){
    return true;
}

bool setCurrent::init(){ return true;}

bool setCurrent::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}
bool setCurrent::paint(QImage *image){
    if(m_data->currentImage != NULL) {
        if(   m_data->currentImage->width()!= image->width()
           || m_data->currentImage->height()!= image->height() ) {
            delete image;
            image = new QImage(m_data->currentImage->width(),
                               m_data->currentImage->height(), QImage::Format_ARGB32);
        }

        paintView::copyBytes(image, m_data->currentImage);
    }

    return true;
}


