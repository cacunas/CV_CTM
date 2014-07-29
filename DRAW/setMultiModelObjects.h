#ifndef SETMMOBJECTS_H
#define SETMMOBJECTS_H

#include <QPainter>

#include "drawInterface.h"
#include "src/RMMT/RMMMobileObject.h"

class setMultiModelObjects: public drawInterface
{
public:
    setMultiModelObjects(Datapool *i_data);


    ~setMultiModelObjects();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);

    void displayShowRMMMobile(QPainter &painter, std::deque<SpReliabilitySingleModelInterface> &models);
    void displayShowRMMMobile(QPainter &painter, SpRMMMobileObject mobileObject);

};

#endif // SETMMOBJECTS_H
