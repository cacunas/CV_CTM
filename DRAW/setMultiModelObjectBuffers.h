#ifndef SETMMOBJECTBUFFERS_H
#define SETMMOBJECTBUFFERS_H

#include <QPainter>

#include "drawInterface.h"
#include "src/RMMT/RMMMobileObject.h"

class setMultiModelObjectBuffers: public drawInterface
{
public:
    setMultiModelObjectBuffers(Datapool *i_data);


    ~setMultiModelObjectBuffers();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);

    void displayShowRMMMobileBuffer(QPainter &painter, std::deque<SpReliabilitySingleModelInterface> &models);
    void displayShowRMMMobileBuffer(QPainter &painter, SpRMMMobileObject mobileObject);

};

#endif // SETMMOBJECTBUFFERS_H
