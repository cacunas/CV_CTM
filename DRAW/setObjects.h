#ifndef SETOBJECTS_H
#define SETOBJECTS_H

#include <QPainter>

#include "drawInterface.h"
#include "src/MobileObject2D.h"

class setObjects: public drawInterface
{
public:
    setObjects(Datapool *i_data);


    ~setObjects();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
    void displayShow2Dmobile2D(QPainter &painter, SpMobileObject2D mobileObject);
    void displayShow2DTrajectoryBuffer2D(QPainter &painter, SpMobileObject2D mobileObject);

};

#endif // SETOBJECTS_H
