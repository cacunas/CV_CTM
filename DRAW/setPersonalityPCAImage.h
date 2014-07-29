#ifndef SETPERSONALITYPCAIMAGE_H
#define SETPERSONALITYPCAIMAGE_H

#include "Datapool.h"
#include "drawInterface.h"

class setPersonalityPCAImage: public drawInterface
{
public:
    setPersonalityPCAImage(Datapool *i_data);

    ~setPersonalityPCAImage();

    //Set module configuration parameters
    bool setParameters(QDomNode &config);

    //Initialization after reading parameters
    bool init();

    //Function executed at each frame
    bool draw(QImage *image);

    bool paint(QImage *image);
};

#endif // SETPERSONALITYPCAIMAGE
