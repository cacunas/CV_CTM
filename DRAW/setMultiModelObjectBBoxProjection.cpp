#include "setMultiModelObjectBBoxProjection.h"

setMultiModelObjectBBoxProjection::setMultiModelObjectBBoxProjection(Datapool *i_data): drawInterface(i_data){
    //image = NULL;
}


setMultiModelObjectBBoxProjection::~setMultiModelObjectBBoxProjection() { }

//Set module configuration parameters
bool setMultiModelObjectBBoxProjection::setParameters(QDomNode &config) { return true; }

//Initialization after reading parameters
bool setMultiModelObjectBBoxProjection::init() {
    return true;
}

//Function executed at each frame
bool setMultiModelObjectBBoxProjection::draw(QImage *image) {
    if(!paint(image))
        return false;
    return true;
}

bool setMultiModelObjectBBoxProjection::paint(QImage *image) {

    QPainter painter;
    painter.begin(image);
    if(!m_data->RMMobjects.empty()) {
        std::deque<SpRMMMobileObject>::iterator end_it = m_data->RMMobjects.end(), iter;
        SpRMMMobileObject o;
        for(iter=m_data->RMMobjects.begin();iter != end_it; iter++) {
            //Draw current 2D info
            o = *iter;
            displayShowRMMMobileBBoxProjection(painter, o);
        }
    }
    painter.end();
    return true;
}


void setMultiModelObjectBBoxProjection::displayShowRMMMobileBBoxProjection(QPainter &painter, SpRMMMobileObject mobileObject) {
    double R;
    Rectangle<int> r = mobileObject->getVisualEvidenceEstimator(R);

    painter.setPen(QColor(0, 255, 0, 255));//green

    painter.drawLine(r.xleft, r.ytop, r.xright, r.ytop);
    painter.drawLine(r.xright, r.ytop, r.xright, r.ybottom);
    painter.drawLine(r.xright, r.ybottom, r.xleft, r.ybottom);
    painter.drawLine(r.xleft, r.ybottom, r.xleft, r.ytop);

    painter.drawEllipse(RECT_XCENTER(&r) - 1, RECT_YCENTER(&r) - 1, 3, 3);

}

