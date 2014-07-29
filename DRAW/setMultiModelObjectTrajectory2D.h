#ifndef SETMMT2D_H
#define SETMMT2D_H

#include <QPainter>

#include "drawInterface.h"
#include "src/RMMT/RMMMobileObject.h"

class setMultiModelObjectTrajectory2D: public drawInterface
{
public:
    setMultiModelObjectTrajectory2D(Datapool *i_data);


    ~setMultiModelObjectTrajectory2D();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);

    void displayShowRMMMobileTrajectory2D(QPainter &painter, SpRMMMobileObject mobileObject);

};

#endif // SETMMT2D_H
