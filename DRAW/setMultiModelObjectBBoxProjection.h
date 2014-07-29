#ifndef SETMMBBOXPROJECTION_H
#define SETMMBBOXPROJECTION_H

#include <QPainter>

#include "drawInterface.h"
#include "src/RMMT/RMMMobileObject.h"

class setMultiModelObjectBBoxProjection: public drawInterface
{
public:
    setMultiModelObjectBBoxProjection(Datapool *i_data);


    ~setMultiModelObjectBBoxProjection();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);

    void displayShowRMMMobileBBoxProjection(QPainter &painter, SpRMMMobileObject mobileObject);

};

#endif // SETMMBBOXES_H
