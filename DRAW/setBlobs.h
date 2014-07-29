#ifndef SETBLOBS_H
#define SETBLOBS_H

#include "Datapool.h"
#include "drawInterface.h"
#include "src/blob.h"


class setBlobs: public drawInterface
{
public:
    setBlobs(Datapool *i_data);

    ~setBlobs();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
};

#endif // SETBLOBS_H
