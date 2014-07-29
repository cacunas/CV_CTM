#include "setBlobs.h"
#include <QPainter>

setBlobs::setBlobs(Datapool *i_data):drawInterface(i_data){}

setBlobs::~setBlobs(){
    m_data = NULL;
}

bool setBlobs::setParameters(QDomNode &config){
    return true;
}
bool setBlobs::init(){ return true;}

bool setBlobs::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}
bool setBlobs::paint(QImage *image){
    QPainter painter;
    if(image != NULL){
        painter.begin(image);
        if(!m_data->blobs.empty()) {
            std::vector<Blob>::iterator end_it = m_data->blobs.end(), iter;
            Blob *bp;
            painter.setPen(QColor(255, 0, 0, 255));//rojo
            // Por cada blob se crea un rectangulo de color rojo (del setPen)
            for(iter=m_data->blobs.begin();iter != end_it; iter++) {
                bp = &*iter;
                painter.drawLine(BLOB_XLEFT(bp), BLOB_YTOP(bp), BLOB_XLEFT(bp), BLOB_YBOTTOM(bp));
                painter.drawLine(BLOB_XLEFT(bp), BLOB_YBOTTOM(bp), BLOB_XRIGHT(bp), BLOB_YBOTTOM(bp));
                painter.drawLine(BLOB_XRIGHT(bp), BLOB_YBOTTOM(bp), BLOB_XRIGHT(bp), BLOB_YTOP(bp));
                painter.drawLine(BLOB_XRIGHT(bp), BLOB_YTOP(bp), BLOB_XLEFT(bp), BLOB_YTOP(bp));
                // Rellena el rectangulo de color blanco
                //painter.setBrush(QColor(255, 255, 255, 255));//blanco
                //painter.drawRect();
            }
        }
        painter.end();
    }
    return true;
}


