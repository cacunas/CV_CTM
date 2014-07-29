#include "setMultiModelObjectTrajectory2D.h"

setMultiModelObjectTrajectory2D::setMultiModelObjectTrajectory2D(Datapool *i_data): drawInterface(i_data){
    //image = NULL;
}


setMultiModelObjectTrajectory2D::~setMultiModelObjectTrajectory2D() { }

//Set module configuration parameters
bool setMultiModelObjectTrajectory2D::setParameters(QDomNode &config) { return true; }

//Initialization after reading parameters
bool setMultiModelObjectTrajectory2D::init() {
    return true;
}

//Function executed at each frame
bool setMultiModelObjectTrajectory2D::draw(QImage *image) {
    if(!paint(image))
        return false;
    return true;
}

bool setMultiModelObjectTrajectory2D::paint(QImage *image) {

    QPainter painter;
    painter.begin(image);
    if(!m_data->RMMobjects.empty()) {
        std::deque<SpRMMMobileObject>::iterator end_it = m_data->RMMobjects.end(), iter;
        SpRMMMobileObject o;
        for(iter=m_data->RMMobjects.begin();iter != end_it; iter++) {
            //Draw current 2D info
            o = *iter;
            displayShowRMMMobileTrajectory2D(painter, o);
        }
    }
    painter.end();
    return true;
}


void setMultiModelObjectTrajectory2D::displayShowRMMMobileTrajectory2D(QPainter &painter, SpRMMMobileObject mobileObject) {

    int i, size;
    if( (size = mobileObject->trajectory2D.size()) == 0 )
        return;

    int x1, y1, x2, y2;
    std::deque<SpTrajectoryPoint>::iterator points = mobileObject->trajectory2D.begin();

    //Show first point
    x1 = (int) (*points)->x;
    y1 = (int) (*points)->y;

    //Oldest red
    painter.setPen(QColor(255,0,0,255)); //red
    painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);

    //Draw next points and lines connecting them
    painter.setPen(QColor(0,0,255,255)); //blue

    for(i=1, points++; i<size ; i++, points++) {
        x2 = x1;
        y2 = y1;

        x1 = (int) (*points)->x;
        y1 = (int) (*points)->y;

        painter.drawLine(x1, y1, x2, y2);
        if(i<size-1)
            painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);
    }

    if(size > 1) {


        painter.setPen(QColor(255,255,255,255)); //white
        painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);


        //Draw final estimated point and line for next frame
/*        painter.setPen(QColor(0,255,0,255)); //green

        x2 = x1;
        y2 = y1;
        mobileObject->get2DPositionProjection(x1, y1);

        painter.drawLine(x1, y1, x2, y2);
        painter.drawEllipse(x1 - 1, y1 - 1, 3, 3);*/
    }


}

