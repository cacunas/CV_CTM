#include "setMultiModelObjectCurrentInvolvedArea.h"

setMultiModelObjectCurrentInvolvedArea::setMultiModelObjectCurrentInvolvedArea(Datapool *i_data): drawInterface(i_data){
}


setMultiModelObjectCurrentInvolvedArea::~setMultiModelObjectCurrentInvolvedArea() { }

//Set module configuration parameters
bool setMultiModelObjectCurrentInvolvedArea::setParameters(QDomNode &config) { return true; }

//Initialization after reading parameters
bool setMultiModelObjectCurrentInvolvedArea::init() {
    return true;
}

//Function executed at each frame
bool setMultiModelObjectCurrentInvolvedArea::draw(QImage *image) {
    if(!paint(image))
        return false;
    return true;
}

bool setMultiModelObjectCurrentInvolvedArea::paint(QImage *image) {

    QPainter painter;
    painter.begin(image);
    if(!m_data->RMMobjects.empty()) {
        std::deque<SpRMMMobileObject>::iterator end_it = m_data->RMMobjects.end(), iter;
        SpRMMMobileObject o;
        for(iter=m_data->RMMobjects.begin();iter != end_it; iter++) {
            //Draw current 2D info
            o = *iter;
            displayShowRMMMobileInvolvedArea(painter, o);
        }
    }
    painter.end();
    return true;
}


void setMultiModelObjectCurrentInvolvedArea::displayShowRMMMobileInvolvedArea(QPainter &painter, SpRMMMobileObject mobileObject) {

    Rectangle<int> &r = mobileObject->iestimator;

    painter.setPen(QColor(255, 255, 0, 255));//yellow

    painter.drawLine(r.xleft, r.ytop, r.xright, r.ytop);
    painter.drawLine(r.xright, r.ytop, r.xright, r.ybottom);
    painter.drawLine(r.xright, r.ybottom, r.xleft, r.ybottom);
    painter.drawLine(r.xleft, r.ybottom, r.xleft, r.ytop);

}

