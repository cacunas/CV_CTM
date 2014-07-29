#ifndef SETMMBBOXES_H
#define SETMMBBOXES_H

#include <QPainter>

#include "drawInterface.h"
#include "src/RMMT/RMMMobileObject.h"

class setMultiModelObjectBBoxes: public drawInterface
{
public:
    setMultiModelObjectBBoxes(Datapool *i_data);


    ~setMultiModelObjectBBoxes();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);

    void displayShowRMMMobileBBox(QPainter &painter, SpRMMMobileObject mobileObject);

};

#endif // SETMMBBOXES_H
