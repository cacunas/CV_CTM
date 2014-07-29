#ifndef SETCURRENT_H
#define SETCURRENT_H

#include "Datapool.h"
#include "drawInterface.h"


class setCurrent: public drawInterface
{
public:
    setCurrent(Datapool *i_data);

    ~setCurrent();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
};

#endif // SETCURRENT_H
