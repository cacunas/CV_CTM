#include "setObjects.h"

setObjects::setObjects(Datapool *i_data): drawInterface(i_data){
    //image = NULL;
}


setObjects::~setObjects(){}

//Set module configuration parameters
bool setObjects::setParameters(QDomNode &config){
    /*
    QDomNode n;
    if(config.isNull()) { //Parameter set for module not defined
        afterModule = '-';
    }
    else {
        if( ( n = getParameterNode("afterModule", config) ).isNull() )
            afterModule = '-';
        else
            afterModule = getParameterValue(n);
    }
    */
    return true;
}

//Initialization after reading parameters
bool setObjects::init(){
    return true;
}

//Function executed at each frame
bool setObjects::draw(QImage *image){
    if(!paint(image))
        return false;
    return true;
}

bool setObjects::paint(QImage *image){

    QPainter painter;
    painter.begin(image);
//    if(!m_data->objects.empty()) {
//        std::deque<SpMobileObject>::iterator end_it = m_data->objects.end(), iter;
//        SpMobileObject o;
//        for(iter=m_data->objects.begin();iter != end_it; iter++) {
    if(!m_data->objects2D.empty()) {
        std::deque<SpMobileObject2D>::iterator end_it = m_data->objects2D.end(), iter;
        SpMobileObject2D o;
        for(iter=m_data->objects2D.begin();iter != end_it; iter++) {
            //Draw current 2D info
            o = *iter;
            displayShow2Dmobile2D(painter, o);
            displayShow2DTrajectoryBuffer2D(painter, o);
        }
    }
    painter.end();
    return true;
}

void setObjects::displayShow2Dmobile2D(QPainter &painter, SpMobileObject2D mobileObject) {

    int
        X   = (int) mobileObject->t2DSpatialData.X,
        Y   = (int) mobileObject->t2DSpatialData.Y,
        W_2 = (int) (mobileObject->t2DDimData.W / 2.0),
        H_2 = (int) (mobileObject->t2DDimData.H / 2.0);

    if(mobileObject->currentVisualState & MM_NOT_VISIBLE_MASK)
        painter.setPen(QColor(0, 0, 0, 255));//black
    else if(mobileObject->currentVisualState & MM_PART_OF_BIGGER)
        painter.setPen(QColor(0, 0, 255, 255));//blue
    else if(mobileObject->currentVisualState & MM_PARTIALLY_DETECTED)
        painter.setPen(QColor(255, 255, 0, 255));//brown
    else
        painter.setPen(QColor(255, 255, 255, 255));//brown

    painter.drawLine(X - W_2, Y - H_2, X + W_2, Y - H_2);
    painter.drawLine(X + W_2, Y - H_2, X + W_2, Y + H_2);
    painter.drawLine(X + W_2, Y + H_2, X - W_2, Y + H_2);
    painter.drawLine(X - W_2, Y + H_2, X - W_2, Y - H_2);
}

void setObjects::displayShow2DTrajectoryBuffer2D(QPainter &painter, SpMobileObject2D mobileObject) {
    int i, size;
    if( (size = mobileObject->trajectory2D.size()) == 0 )
        return;

    int x1, y1, x2, y2;
    std::deque<SpTrajectoryPoint>::iterator points = mobileObject->trajectory2D.begin();

    //Show first point
    x1 = (int) (*points)->x;
    y1 = (int) (*points)->y;

    //Oldest white
    painter.setPen(QColor(255,255,255,255)); //white
    painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);

    //Draw next points and lines connecting them
    painter.setPen(QColor(0,0,255,255)); //blue

    for(i=1, points++; i<size ; i++, points++) {
        x2 = x1;
        y2 = y1;

        x1 = (int) (*points)->x;
        y1 = (int) (*points)->y;

        painter.drawLine(x1, y1, x2, y2);
        painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);
    }

    if(size > 1) {
        //Draw final estimated point and line for next frame
        painter.setPen(QColor(0,255,0,255)); //green

        x2 = x1;
        y2 = y1;
        x1 = (int) (mobileObject->trajectory2D.back()->x + mobileObject->t2DSpatialData.VX * MobileObject2D::secDiffSequence[0]);
        y1 = (int) (mobileObject->trajectory2D.back()->y + mobileObject->t2DSpatialData.VY * MobileObject2D::secDiffSequence[0]);

        painter.drawLine(x1, y1, x2, y2);
        painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);
    }
}
