#ifndef SETFOREGROUND_H
#define SETFOREGROUND_H

#include "Datapool.h"
#include "drawInterface.h"


class setForeground: public drawInterface
{
public:
    setForeground(Datapool *i_data);

    ~setForeground();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
};

#endif // setForeground_H
