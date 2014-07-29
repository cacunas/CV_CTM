#ifndef SETBACKGROUND_H
#define SETBACKGROUND_H

#include "Datapool.h"
#include "drawInterface.h"


class setBackground: public drawInterface
{
public:
    setBackground(Datapool *i_data);

    ~setBackground();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
};

#endif // setBackground_H
