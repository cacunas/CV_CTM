#ifndef SETPERSONALITY_H
#define SETPERSONALITY_H

#include "Datapool.h"
#include "drawInterface.h"


class setPersonality: public drawInterface
{
public:
    setPersonality(Datapool *i_data);

    ~setPersonality();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
};

#endif // SETCURRENT_H
