#ifndef SETMMCURINVAREA_H
#define SETMMCURINVAREA_H

#include <QPainter>

#include "drawInterface.h"
#include "src/RMMT/RMMMobileObject.h"

class setMultiModelObjectCurrentInvolvedArea: public drawInterface
{
public:
    setMultiModelObjectCurrentInvolvedArea(Datapool *i_data);


    ~setMultiModelObjectCurrentInvolvedArea();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);

    void displayShowRMMMobileInvolvedArea(QPainter &painter, SpRMMMobileObject mobileObject);

};

#endif // SETMMCURINVAREA_H
