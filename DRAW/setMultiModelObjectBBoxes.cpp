#include "setMultiModelObjectBBoxes.h"

setMultiModelObjectBBoxes::setMultiModelObjectBBoxes(Datapool *i_data): drawInterface(i_data){
    //image = NULL;
}


setMultiModelObjectBBoxes::~setMultiModelObjectBBoxes() { }

//Set module configuration parameters
bool setMultiModelObjectBBoxes::setParameters(QDomNode &config) { return true; }

//Initialization after reading parameters
bool setMultiModelObjectBBoxes::init() {
    return true;
}

//Function executed at each frame
bool setMultiModelObjectBBoxes::draw(QImage *image) {
    if(!paint(image))
        return false;
    return true;
}

bool setMultiModelObjectBBoxes::paint(QImage *image) {

    QPainter painter;
    painter.begin(image);
    if(!m_data->RMMobjects.empty()) {
        std::deque<SpRMMMobileObject>::iterator end_it = m_data->RMMobjects.end(), iter;
        SpRMMMobileObject o;
        for(iter=m_data->RMMobjects.begin();iter != end_it; iter++) {
            //Draw current 2D info
            o = *iter;
            displayShowRMMMobileBBox(painter, o);
        }
    }
    painter.end();
    return true;
}


void setMultiModelObjectBBoxes::displayShowRMMMobileBBox(QPainter &painter, SpRMMMobileObject mobileObject) {

    Rectangle<int> &r = mobileObject->multiModel.binterface.bbox;

    if(mobileObject->currentVisualState & MM_NOT_VISIBLE_MASK)
        painter.setPen(QColor(0, 0, 0, 255));//black
    else if(mobileObject->currentVisualState & MM_PART_OF_BIGGER)
        painter.setPen(QColor(0, 0, 255, 255));//blue
    else if(mobileObject->currentVisualState & MM_PARTIALLY_DETECTED)
        painter.setPen(QColor(255, 255, 0, 255));//brown
    else
        painter.setPen(QColor(255, 0, 0, 255));//red

    painter.drawLine(r.xleft, r.ytop, r.xright, r.ytop);
    painter.drawLine(r.xright, r.ytop, r.xright, r.ybottom);
    painter.drawLine(r.xright, r.ybottom, r.xleft, r.ybottom);
    painter.drawLine(r.xleft, r.ybottom, r.xleft, r.ytop);

}

