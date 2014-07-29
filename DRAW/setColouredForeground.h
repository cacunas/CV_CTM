#ifndef SETCOLOUREDFOREGROUND_H
#define SETCOLOUREDFOREGROUND_H

#include "Datapool.h"
#include "drawInterface.h"


class setColouredForeground: public drawInterface
{
public:
    setColouredForeground(Datapool *i_data);

    ~setColouredForeground();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
};

#endif // SETCURRENT_H
